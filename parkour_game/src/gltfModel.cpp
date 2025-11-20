#include "gltfModel.h"
#include <iostream>
#include <cassert>
#include <glm/gtc/type_ptr.hpp>


void GltfModel::setCgltfData(cgltf_data* d)
{
    data = d;
    //ensureNodeTransformArrays();
}

void GltfModel::ensureNodeTransformArrays()
{
    if (!data) return;
    size_t n = static_cast<size_t>(data->nodes_count);
    nodeLocalTransforms.assign(n, glm::mat4(1.0f));
    nodeGlobalTransforms.assign(n, glm::mat4(1.0f));
}

glm::mat4 GltfModel::computeLocalMatrix(const cgltf_node* node) const
{
    glm::mat4 m(1.0f);

    if (!node) return m;

    if (node->has_matrix) {
        // cgltf stores matrices in column-major order in node->matrix[16]
        // glm::mat4 ctor expects column-major when filling by array in this way:
        glm::mat4 mat(
            node->matrix[0],  node->matrix[1],  node->matrix[2],  node->matrix[3],
            node->matrix[4],  node->matrix[5],  node->matrix[6],  node->matrix[7],
            node->matrix[8],  node->matrix[9],  node->matrix[10], node->matrix[11],
            node->matrix[12], node->matrix[13], node->matrix[14], node->matrix[15]
        );
        return mat;
    }

    glm::vec3 T(0.0f);
    glm::vec3 S(1.0f);
    glm::quat R(1.0f, 0.0f, 0.0f, 0.0f); // w,x,y,z

    if (node->has_translation) {
        T = glm::vec3(node->translation[0], node->translation[1], node->translation[2]);
    }
    if (node->has_scale) {
        S = glm::vec3(node->scale[0], node->scale[1], node->scale[2]);
    }
    if (node->has_rotation) {
        // cgltf rotation is (x, y, z, w)
        R = glm::quat(node->rotation[3], node->rotation[0], node->rotation[1], node->rotation[2]);
    }

    glm::mat4 trans = glm::translate(glm::mat4(1.0f), T);
    glm::mat4 rot = glm::toMat4(R);
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), S);

    return trans * rot * scale;
}

void GltfModel::computeGlobalTransforms()
{
    if (!data) return;
    ensureNodeTransformArrays();

    // build index map: node* -> index
    std::unordered_map<const cgltf_node*, size_t> nodeIndex;
    for (size_t i = 0; i < (size_t)data->nodes_count; ++i) {
        nodeIndex[&data->nodes[i]] = i;
    }

    // first fill local transforms from current cgltf nodes
    for (size_t i = 0; i < (size_t)data->nodes_count; ++i) {
        nodeLocalTransforms[i] = computeLocalMatrix(&data->nodes[i]);
        nodeGlobalTransforms[i] = glm::mat4(1.0f); // reset
    }

    // recursive lambda to compute global from a node and its children
    std::function<void(const cgltf_node*, const glm::mat4&)> recurse;
    recurse = [&](const cgltf_node* node, const glm::mat4& parentMat) {
        if (!node) return;
        size_t idx = nodeIndex.at(node);
        glm::mat4 local = nodeLocalTransforms[idx];
        glm::mat4 global = parentMat * local;
        nodeGlobalTransforms[idx] = global;

        // cgltf node children iteration (if any)
        for (cgltf_size i = 0; i < node->children_count; ++i) {
            recurse(node->children[i], global);
        }
    };

    // start at scene roots
    for (cgltf_size s = 0; s < data->scenes_count; ++s) {
        const cgltf_scene* scene = &data->scenes[s];
        for (cgltf_size r = 0; r < scene->nodes_count; ++r) {
            const cgltf_node* root = scene->nodes[r];
            recurse(root, glm::mat4(1.0f));
        }
    }
}

/* Helper readers: use cgltf_accessor_read_float (available in cgltf) */
float GltfModel::readAccessorFloat(const cgltf_accessor* acc, size_t index) const
{
    if (!acc) return 0.0f;
    float out = 0.0f;
    cgltf_accessor_read_float(acc, index, &out, 1);
    return out;
}

void GltfModel::readAccessorVec3(const cgltf_accessor* acc, size_t index, float out3[3]) const
{
    if (!acc) { out3[0] = out3[1] = out3[2] = 0.0f; return; }
    cgltf_accessor_read_float(acc, index, out3, 3);
}

void GltfModel::readAccessorQuat(const cgltf_accessor* acc, size_t index, float out4[4]) const
{
    if (!acc) { out4[0] = out4[1] = out4[2] = out4[3] = 0.0f; return; }
    cgltf_accessor_read_float(acc, index, out4, 4);
}

void GltfModel::applyAnimationToNodes(float timeInSeconds)
{
    if (!data || data->animations_count == 0) return;

    // use the first animation for now
    cgltf_animation* anim = &data->animations[0];

    for (cgltf_size ch = 0; ch < anim->channels_count; ++ch) {
        const cgltf_animation_channel* channel = &anim->channels[ch];
        const cgltf_animation_sampler* sampler = channel->sampler;
        const cgltf_accessor* input = sampler->input;
        const cgltf_accessor* output = sampler->output;

        if (!input || !output) continue;
        if (input->count == 0) continue;

        // find keyframe interval [k0, k1] such that t in [t0,t1]
        size_t k0 = 0, k1 = 0;
        float t0 = readAccessorFloat(input, 0);
        if (timeInSeconds <= t0) {
            k0 = k1 = 0;
        } else {
            // default to last
            k1 = static_cast<size_t>(input->count - 1);
            for (size_t k = 0; k < (size_t)input->count - 1; ++k) {
                float ta = readAccessorFloat(input, k);
                float tb = readAccessorFloat(input, k + 1);
                if (timeInSeconds >= ta && timeInSeconds <= tb) {
                    k0 = k;
                    k1 = k + 1;
                    break;
                }
            }
        }

        float ta = readAccessorFloat(input, k0);
        float tb = readAccessorFloat(input, k1);
        float factor = 0.0f;
        if (k0 == k1) factor = 0.0f;
        else factor = (timeInSeconds - ta) / (tb - ta);

        // apply to node target
        cgltf_node* target = channel->target_node;
        if (!target) continue;

        if (channel->target_path == cgltf_animation_path_type_translation) {
            float v0[3], v1[3];
            readAccessorVec3(output, k0, v0);
            readAccessorVec3(output, k1, v1);
            float vx = v0[0] + (v1[0] - v0[0]) * factor;
            float vy = v0[1] + (v1[1] - v0[1]) * factor;
            float vz = v0[2] + (v1[2] - v0[2]) * factor;
            target->translation[0] = vx;
            target->translation[1] = vy;
            target->translation[2] = vz;
            target->has_translation = 1;
        }
        else if (channel->target_path == cgltf_animation_path_type_scale) {
            float v0[3], v1[3];
            readAccessorVec3(output, k0, v0);
            readAccessorVec3(output, k1, v1);
            float sx = v0[0] + (v1[0] - v0[0]) * factor;
            float sy = v0[1] + (v1[1] - v0[1]) * factor;
            float sz = v0[2] + (v1[2] - v0[2]) * factor;
            target->scale[0] = sx;
            target->scale[1] = sy;
            target->scale[2] = sz;
            target->has_scale = 1;
        }
        else if (channel->target_path == cgltf_animation_path_type_rotation) {
            float q0[4], q1[4];
            readAccessorQuat(output, k0, q0);
            readAccessorQuat(output, k1, q1);
            // cgltf stores quat as x,y,z,w
            glm::quat A(q0[3], q0[0], q0[1], q0[2]);
            glm::quat B(q1[3], q1[0], q1[1], q1[2]);
            glm::quat M = glm::slerp(A, B, factor);
            // write back as x,y,z,w
            target->rotation[0] = M.x;
            target->rotation[1] = M.y;
            target->rotation[2] = M.z;
            target->rotation[3] = M.w;
            target->has_rotation = 1;
        }
    }
}

void GltfModel::updateAnimation(float timeInSeconds)
{
    if (!data) return;
    if (data->animations_count == 0) return;

    // apply animation keyframes to nodes
    applyAnimationToNodes(timeInSeconds);

    // recompute local and global matrices
    computeGlobalTransforms();

    // (note) we do not skin vertices here, but you can use nodeGlobalTransforms + inverseBindMatrices later
}

void GltfModel::uploadToGPU() {
    // --- VERTICES ---
    if (!vertices.empty()) {
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    }

    // --- NORMALS ---
    if (!normals.empty()) {
        glGenBuffers(1, &nbo);
        glBindBuffer(GL_ARRAY_BUFFER, nbo);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);
    }

    // --- UVs ---
    if (!texcoords.empty()) {
        glGenBuffers(1, &tbo);
        glBindBuffer(GL_ARRAY_BUFFER, tbo);
        glBufferData(GL_ARRAY_BUFFER, texcoords.size() * sizeof(float), texcoords.data(), GL_STATIC_DRAW);
    }

    // --- INDICES ---
    if (!indices.empty()) {
        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    }
}


/**
void GltfModel::draw() {
    // Bind texture if available
    if (textureID != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureID);
    }

    // --- VERTICES ---
    if (vbo != 0) {
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, (void*)0);
    }

    // --- NORMALS ---
    if (nbo != 0) {
        glBindBuffer(GL_ARRAY_BUFFER, nbo);
        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(GL_FLOAT, 0, (void*)0);
    }

    // --- UVs ---
    if (tbo != 0) {
        glBindBuffer(GL_ARRAY_BUFFER, tbo);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, 0, (void*)0);
    }

    // --- INDICES ---
    if (ebo != 0 && !indices.empty()) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    } else if (!indices.empty()) {
        // fallback: client-side draw
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, indices.data());
    }

    if (vbo != 0) glDisableClientState(GL_VERTEX_ARRAY);
    if (nbo != 0) glDisableClientState(GL_NORMAL_ARRAY);
    if (tbo != 0) glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    if (textureID != 0) {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

//new
**/



void GltfModel::draw() {
    if (!data) return;

    // Bind texture if available
    if (textureID != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureID);
    }

    glPushMatrix();
    // Apply root node transform only
    if (!nodeGlobalTransforms.empty()) {
        glMultMatrixf(glm::value_ptr(nodeGlobalTransforms[0]));
    }

    // Draw all vertices
    if (vbo != 0) {
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, (void*)0);
    }

    if (nbo != 0) {
        glBindBuffer(GL_ARRAY_BUFFER, nbo);
        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(GL_FLOAT, 0, (void*)0);
    }

    if (tbo != 0) {
        glBindBuffer(GL_ARRAY_BUFFER, tbo);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, 0, (void*)0);
    }

    if (ebo != 0 && !indices.empty()) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    } else if (!indices.empty()) {
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, indices.data());
    }

    if (vbo != 0) glDisableClientState(GL_VERTEX_ARRAY);
    if (nbo != 0) glDisableClientState(GL_NORMAL_ARRAY);
    if (tbo != 0) glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glPopMatrix();

    if (textureID != 0) {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void GltfModel::buildTriangleList()
{
    triangles.clear();

    for (size_t i = 0; i < indices.size(); i += 3) {
        int i0 = indices[i];
        int i1 = indices[i+1];
        int i2 = indices[i+2];

        vec3 v0{vertices[i0*3+0], vertices[i0*3+1], vertices[i0*3+2]};
        vec3 v1{vertices[i1*3+0], vertices[i1*3+1], vertices[i1*3+2]};
        vec3 v2{vertices[i2*3+0], vertices[i2*3+1], vertices[i2*3+2]};
        Triangle t;
        t.a = v0;
        t.b = v1;
        t.c = v2;
        triangles.push_back(t);
    }
}


