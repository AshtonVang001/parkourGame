// ---------------- Implementation ----------------
#include "GLTFLoader.h"

static std::string getBasePath(const std::string& path) {
    size_t p = path.find_last_of("/\\");
    if (p == std::string::npos) return ".";
    return path.substr(0, p);
}

bool GLTFModel::load(const std::string& filename) {
    //printf("[DEBUG] GLTFLoader::load() start, filename=%s\n", filename.c_str());

    freeGLResources();
    gltfBasePath = getBasePath(filename);
    bool result = loadCgltfFile(filename);

    //printf("[DEBUG] GLTFLoader::load() finished, result=%d\n", result);
    return result;
}

bool GLTFModel::loadCgltfFile(const std::string& filename) {
    //printf("[DEBUG] loadCgltfFile() start\n");
    cgltf_options options;
    memset(&options, 0, sizeof(options));
    cgltfData = nullptr;
    cgltf_result res = cgltf_parse_file(&options, filename.c_str(), &cgltfData);
    //printf("[DEBUG] cgltf_parse_file result=%d\n", res);
    if (res != cgltf_result_success) {
        std::cerr << "[ERROR] cgltf_parse_file failed\n";
        return false;
    }

    if (cgltf_load_buffers(&options, cgltfData, filename.c_str()) != cgltf_result_success) {
        std::cerr << "[ERROR] cgltf_load_buffers failed\n";
        cgltf_free(cgltfData);
        cgltfData = nullptr;
        return false;
    }

    //printf("[DEBUG] Building materials\n");
    buildMaterials(cgltfData, gltfBasePath);
    //printf("[DEBUG] Building meshes\n");
    buildMeshes(cgltfData);
    //printf("[DEBUG] Building nodes\n");
    buildNodes(cgltfData);
    //printf("[DEBUG] Building skins\n");
    buildSkins(cgltfData);
    //printf("[DEBUG] Building animations\n");
    buildAnimations(cgltfData);

    //printf("[DEBUG] Computing world matrices\n");
    computeWorldMatrices();

    //printf("[DEBUG] loadCgltfFile() finished\n");
    return true;
}

void GLTFModel::freeGLResources() {
    //printf("[DEBUG] freeGLResources() start\n");
    for (auto &p : meshParts) {
        if (p.vao) glDeleteVertexArrays(1, &p.vao);
        if (p.vbo) glDeleteBuffers(1, &p.vbo);
        if (p.ebo) glDeleteBuffers(1, &p.ebo);
    }
    meshParts.clear();

    for (auto &m : materials) {
        if (m.baseColorTex) glDeleteTextures(1, &m.baseColorTex);
    }
    materials.clear();

    nodes.clear();
    skins.clear();
    animations.clear();

    if (cgltfData) {
        cgltf_free(cgltfData);
        cgltfData = nullptr;
    }
    //printf("[DEBUG] freeGLResources() finished\n");
}

void GLTFModel::buildMaterials(cgltf_data* data, const std::string& basePath) {
    //printf("[DEBUG] buildMaterials() start, count=%zu\n", data->materials_count);
    materials.clear();
    materials.resize(data->materials_count);

    for (cgltf_size mi = 0; mi < data->materials_count; ++mi) {
        //printf("[DEBUG] Building material %zu\n", mi);
        auto &dst = materials[mi];
        cgltf_material& src = data->materials[mi];
        if (src.pbr_metallic_roughness.base_color_factor) {
            auto f = src.pbr_metallic_roughness.base_color_factor;
            dst.baseColorFactor = glm::vec4(f[0], f[1], f[2], f[3]);
        } else {
            dst.baseColorFactor = glm::vec4(1.0f);
        }

        if (src.pbr_metallic_roughness.base_color_texture.texture) {
            cgltf_texture_view view = src.pbr_metallic_roughness.base_color_texture;
            if (view.texture && view.texture->image) {
                cgltf_image* img = view.texture->image;
                if (img->buffer_view) {
                    cgltf_buffer_view* bv = img->buffer_view;
                    cgltf_buffer* buf = bv->buffer;
                    unsigned char* imageData = (unsigned char*)buf->data + bv->offset;
                    size_t imageSize = bv->size;
                    GLuint tex = SOIL_load_OGL_texture_from_memory(imageData, (int)imageSize, 0, 0, SOIL_FLAG_INVERT_Y);
                    dst.baseColorTex = tex;
                    //printf("[DEBUG] Material %zu: texture loaded from buffer view\n", mi);
                } else if (img->uri && img->uri[0]) {
                    std::string uri = img->uri;
                    std::string fullPath;
                    if (uri.find("data:") == 0) {
                        std::cerr << "[WARN] embedded image via data URI not decoded\n";
                    } else {
                        fullPath = basePath + "/" + uri;
                        int texId = SOIL_load_OGL_texture(fullPath.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
                        dst.baseColorTex = texId;
                        //printf("[DEBUG] Material %zu: texture loaded from %s\n", mi, fullPath.c_str());
                    }
                }
            }
        }
    }
    //printf("[DEBUG] buildMaterials() finished\n");
}

void GLTFModel::buildMeshes(cgltf_data* data) {
    //printf("[DEBUG] buildMeshes() start\n");
    meshParts.clear();

    for (cgltf_size mi = 0; mi < data->meshes_count; ++mi) {
        cgltf_mesh* mesh = &data->meshes[mi];
        //printf("[DEBUG] Mesh %zu has %zu primitives\n", mi, mesh->primitives_count);

        for (cgltf_size prim_i = 0; prim_i < mesh->primitives_count; ++prim_i) {
            //printf("[DEBUG] Building primitive %zu of mesh %zu\n", prim_i, mi);
            cgltf_primitive* prim = &mesh->primitives[prim_i];

            std::vector<Vertex> vertices;
            std::vector<uint32_t> indices;

            cgltf_accessor* posAcc = nullptr;
            cgltf_accessor* normAcc = nullptr;
            cgltf_accessor* uvAcc = nullptr;
            cgltf_accessor* jointsAcc = nullptr;
            cgltf_accessor* weightsAcc = nullptr;

            for (cgltf_size a = 0; a < prim->attributes_count; ++a) {
                cgltf_attribute* attr = &prim->attributes[a];
                if (attr->type == cgltf_attribute_type_position) posAcc = attr->data;
                else if (attr->type == cgltf_attribute_type_normal) normAcc = attr->data;
                else if (attr->type == cgltf_attribute_type_texcoord) uvAcc = attr->data;
                else if (attr->type == cgltf_attribute_type_joints) jointsAcc = attr->data;
                else if (attr->type == cgltf_attribute_type_weights) weightsAcc = attr->data;
            }
            if (!posAcc) continue;

            size_t vcount = posAcc->count;
            vertices.resize(vcount);
            //printf("[DEBUG] Primitive %zu: vertex count=%zu\n", prim_i, vcount);

            for (size_t i = 0; i < vcount; ++i) {
                float const* p = (float const*)((uint8_t*)posAcc->buffer_view->buffer->data + posAcc->buffer_view->offset + posAcc->offset) + i * posAcc->stride / sizeof(float);
                vertices[i].pos = glm::vec3(p[0], p[1], p[2]);
            }

            //printf("[DEBUG] Primitive %zu: positions read\n", prim_i);

            if (normAcc) {
                for (size_t i = 0; i < vcount; ++i) {
                    float const* n = (float const*)((uint8_t*)normAcc->buffer_view->buffer->data + normAcc->buffer_view->offset + normAcc->offset) + i * normAcc->stride / sizeof(float);
                    vertices[i].normal = glm::vec3(n[0], n[1], n[2]);
                }
            }
            if (uvAcc) {
                for (size_t i = 0; i < vcount; ++i) {
                    float const* uv = (float const*)((uint8_t*)uvAcc->buffer_view->buffer->data + uvAcc->buffer_view->offset + uvAcc->offset) + i * uvAcc->stride / sizeof(float);
                    vertices[i].uv = glm::vec2(uv[0], uv[1]);
                }
            }
            if (jointsAcc && weightsAcc) {
                //printf("[DEBUG] Primitive %zu: joints & weights present\n", prim_i);
                for (size_t i = 0; i < vcount; ++i) {
                    uint16_t jointIdx[4] = {0,0,0,0};
                    if (jointsAcc->component_type == cgltf_component_type_r_8u) {
                        uint8_t const* src = (uint8_t const*)((uint8_t*)jointsAcc->buffer_view->buffer->data + jointsAcc->buffer_view->offset + jointsAcc->offset) + i * jointsAcc->stride;
                        for (int k=0;k<4;++k) jointIdx[k] = src[k];
                    } else if (jointsAcc->component_type == cgltf_component_type_r_16u) {
                        uint16_t const* src = (uint16_t const*)((uint8_t*)jointsAcc->buffer_view->buffer->data + jointsAcc->buffer_view->offset + jointsAcc->offset) + i * (jointsAcc->stride/2);
                        for (int k=0;k<4;++k) jointIdx[k] = src[k];
                    }
                    vertices[i].joints = glm::vec4((float)jointIdx[0], (float)jointIdx[1], (float)jointIdx[2], (float)jointIdx[3]);
                    float const* w = (float const*)((uint8_t*)weightsAcc->buffer_view->buffer->data + weightsAcc->buffer_view->offset + weightsAcc->offset) + i * weightsAcc->stride / sizeof(float);
                    vertices[i].weights = glm::vec4(w[0], w[1], w[2], w[3]);
                }
            }

            //printf("[DEBUG] Primitive %zu: building indices\n", prim_i);
            if (prim->indices) {
                cgltf_accessor* ia = prim->indices;
                indices.resize(ia->count);
                if (ia->component_type == cgltf_component_type_r_16u) {
                    uint16_t const* src = (uint16_t const*)((uint8_t*)ia->buffer_view->buffer->data + ia->buffer_view->offset + ia->offset);
                    for (cgltf_size i = 0; i < ia->count; ++i) indices[i] = src[i];
                } else {
                    uint32_t const* src = (uint32_t const*)((uint8_t*)ia->buffer_view->buffer->data + ia->buffer_view->offset + ia->offset);
                    for (cgltf_size i = 0; i < ia->count; ++i) indices[i] = src[i];
                }
            } else {
                indices.resize(vcount);
                for (size_t i = 0; i < vcount; ++i) indices[i] = (uint32_t)i;
            }

            //printf("[DEBUG] Primitive %zu: creating GL buffers\n", prim_i);
            GLTFMeshPart part;
            glGenVertexArrays(1, &part.vao);
            glBindVertexArray(part.vao);

            glGenBuffers(1, &part.vbo);
            glBindBuffer(GL_ARRAY_BUFFER, part.vbo);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

            glGenBuffers(1, &part.ebo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, part.ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

            GLsizei stride = sizeof(Vertex);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, pos));
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, normal));
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, uv));
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, joints));
            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, weights));

            glBindVertexArray(0);

            part.indexCount = (GLsizei)indices.size();
            part.indexType = GL_UNSIGNED_INT;
            part.materialIndex = (prim->material ? (int)(prim->material - data->materials) : -1);

            meshParts.push_back(part);
            //printf("[DEBUG] Primitive %zu: VAO created and stored\n", prim_i);
        }
    }

    //printf("[DEBUG] buildMeshes() finished\n");
}


glm::mat4 GLTFModel::nodeLocalFromCgltf(cgltf_node* n) {
    //printf("[DEBUG] nodeLocalFromCgltf() called\n");
    glm::mat4 m(1.0f);
    if (n->has_matrix) {
        m = glm::make_mat4(n->matrix);
    } else {
        glm::vec3 t(0.0f), s(1.0f);
        glm::quat r(1,0,0,0);
        if (n->translation) t = glm::vec3(n->translation[0], n->translation[1], n->translation[2]);
        if (n->scale) s = glm::vec3(n->scale[0], n->scale[1], n->scale[2]);
        if (n->rotation) r = glm::quat(n->rotation[3], n->rotation[0], n->rotation[1], n->rotation[2]);
        m = glm::translate(glm::mat4(1.0f), t) * glm::mat4_cast(r) * glm::scale(glm::mat4(1.0f), s);
    }
    return m;
}

void GLTFModel::buildNodes(cgltf_data* data) {
    //printf("[DEBUG] buildNodes() start, nodes_count=%zu\n", data->nodes_count);
    nodes.clear();
    nodes.resize(data->nodes_count);

    for (cgltf_size i = 0; i < data->nodes_count; ++i) {
        cgltf_node* n = &data->nodes[i];
        Node node;

        glm::vec3 t(0.0f);
        glm::vec3 s(1.0f);
        glm::quat r(1.0f,0.0f,0.0f,0.0f);

        if (n->has_matrix) {
            node.localMatrix = glm::make_mat4(n->matrix);
            t = glm::vec3(node.localMatrix[3]);
        } else {
            if (n->translation) t = glm::vec3(n->translation[0], n->translation[1], n->translation[2]);
            if (n->scale) s = glm::vec3(n->scale[0], n->scale[1], n->scale[2]);
            if (n->rotation) r = glm::quat(n->rotation[3], n->rotation[0], n->rotation[1], n->rotation[2]);
            node.localMatrix = glm::translate(glm::mat4(1.0f), t) * glm::mat4_cast(r) * glm::scale(glm::mat4(1.0f), s);
        }

        node.localTranslation = t;
        node.localRotation    = r;
        node.localScale       = s;

        node.meshIndex = n->mesh ? (int)(n->mesh - data->meshes) : -1;
        node.skinIndex = -1;
        node.parent = -1;
        node.children.clear();

        nodes[i] = node;
        //printf("[DEBUG] Node %zu built, meshIndex=%d\n", i, node.meshIndex);
    }

    for (cgltf_size s = 0; s < data->scenes_count; ++s) {
        cgltf_scene* sc = &data->scenes[s];
        for (cgltf_size n = 0; n < sc->nodes_count; ++n) {
            std::function<void(int,int)> recurse = [&](int nodeIndex, int parentIndex) {
                if (parentIndex >= 0) {
                    nodes[nodeIndex].parent = parentIndex;
                    nodes[parentIndex].children.push_back(nodeIndex);
                }
                cgltf_node* ng = &data->nodes[nodeIndex];
                for (cgltf_size c = 0; c < ng->children_count; ++c) {
                    int childIndex = (int)(ng->children[c] - data->nodes);
                    recurse(childIndex, nodeIndex);
                }
            };
            int rootIndex = (int)(sc->nodes[n] - data->nodes);
            recurse(rootIndex, -1);
        }
    }

    //printf("[DEBUG] buildNodes() finished\n");
}

void GLTFModel::buildSkins(cgltf_data* data) {
    //printf("[DEBUG] buildSkins() start, skins_count=%zu\n", data->skins_count);
    skins.clear();
    skins.resize(data->skins_count);
    for (cgltf_size si = 0; si < data->skins_count; ++si) {
        cgltf_skin* s = &data->skins[si];
        Skin skin;
        skin.skeletonRoot = s->skeleton ? (int)(s->skeleton - data->nodes) : -1;
        skin.joints.resize(s->joints_count);
        skin.inverseBindMatrices.resize(s->joints_count);
        skin.jointMatrices.resize(s->joints_count);

        for (cgltf_size j = 0; j < s->joints_count; ++j) {
            skin.joints[j] = (int)(s->joints[j] - data->nodes);
        }

        if (s->inverse_bind_matrices) {
            cgltf_accessor* acc = s->inverse_bind_matrices;
            for (cgltf_size k = 0; k < acc->count; ++k) {
                float const* msrc = (float const*)((uint8_t*)acc->buffer_view->buffer->data + acc->buffer_view->offset + acc->offset) + k * acc->stride/sizeof(float);
                glm::mat4 ib = glm::make_mat4(msrc);
                skin.inverseBindMatrices[k] = ib;
            }
        } else {
            for (size_t k = 0; k < skin.inverseBindMatrices.size(); ++k) skin.inverseBindMatrices[k] = glm::mat4(1.0f);
        }
        skins[si] = skin;
        //printf("[DEBUG] Skin %zu built, joints_count=%zu\n", si, s->joints_count);
    }
    //printf("[DEBUG] buildSkins() finished\n");
}

void GLTFModel::buildAnimations(cgltf_data* data) {
    //printf("[DEBUG] buildAnimations() start, animations_count=%zu\n", data->animations_count);
    animations.clear();
    animations.resize(data->animations_count);
    for (cgltf_size ai = 0; ai < data->animations_count; ++ai) {
        cgltf_animation* anim = &data->animations[ai];
        Animation out;
        out.samplers.resize(anim->samplers_count);

        for (cgltf_size si = 0; si < anim->samplers_count; ++si) {
            cgltf_animation_sampler* s = &anim->samplers[si];
            AnimationSampler as;
            as.interp = s->interpolation;
            cgltf_accessor* inAcc = s->input;
            as.inputTimes.resize(inAcc->count);
            for (cgltf_size k = 0; k < inAcc->count; ++k) {
                float const* src = (float const*)((uint8_t*)inAcc->buffer_view->buffer->data + inAcc->buffer_view->offset + inAcc->offset) + k * inAcc->stride / sizeof(float);
                as.inputTimes[k] = src[0];
                if (as.inputTimes[k] > out.duration) out.duration = as.inputTimes[k];
            }

            cgltf_accessor* outAcc = s->output;
            as.outputsVec4.resize(outAcc->count);
            for (cgltf_size k = 0; k < outAcc->count; ++k) {
                float const* src = (float const*)((uint8_t*)outAcc->buffer_view->buffer->data + outAcc->buffer_view->offset + outAcc->offset) + k * outAcc->stride / sizeof(float);
                if (outAcc->type == cgltf_type_vec3) as.outputsVec4[k] = glm::vec4(src[0], src[1], src[2], 0.0f);
                else if (outAcc->type == cgltf_type_vec4) as.outputsVec4[k] = glm::vec4(src[0], src[1], src[2], src[3]);
                else as.outputsVec4[k] = glm::vec4(src[0], 0, 0, 0);
            }
            out.samplers[si] = as;
        }

        out.channels.resize(anim->channels_count);
        for (cgltf_size ci = 0; ci < anim->channels_count; ++ci) {
            cgltf_animation_channel* ch = &anim->channels[ci];
            AnimationChannel c;
            c.targetNode = (int)(ch->target_node - data->nodes);
            switch (ch->target_path) {
                case cgltf_animation_path_type_translation: c.path = "translation"; break;
                case cgltf_animation_path_type_rotation: c.path = "rotation"; break;
                case cgltf_animation_path_type_scale: c.path = "scale"; break;
                default: c.path = "unknown"; break;
            }
            c.samplerIndex = (int)(ch->sampler - anim->samplers);
            out.channels[ci] = c;
        }
        animations[ai] = out;
        //printf("[DEBUG] Animation %zu built, duration=%f, samplers=%zu, channels=%zu\n", ai, out.duration, out.samplers.size(), out.channels.size());
    }
    //printf("[DEBUG] buildAnimations() finished\n");
}

glm::vec4 GLTFModel::sampleVec4(const AnimationSampler& s, float t) const {
    if (s.inputTimes.empty()) return glm::vec4(0,0,0,0); // fallback

    // find keyframe interval
    size_t i = 0;
    while (i + 1 < s.inputTimes.size() && t > s.inputTimes[i + 1]) i++;

    // if past last key, return last value
    if (i + 1 == s.inputTimes.size()) {
        return s.outputsVec4.back();
    }

    // interpolate between key i and i+1
    float t0 = s.inputTimes[i];
    float t1 = s.inputTimes[i + 1];
    float factor = (t - t0) / (t1 - t0);

    const glm::vec4 &v0 = s.outputsVec4[i];
    const glm::vec4 &v1 = s.outputsVec4[i + 1];

    return glm::mix(v0, v1, factor); // linear interpolation
}


glm::quat GLTFModel::sampleQuat(const AnimationSampler& s, float t) const {
    if (s.inputTimes.empty()) return glm::quat(1,0,0,0); // fallback

    // find keyframe interval
    size_t i = 0;
    while (i + 1 < s.inputTimes.size() && t > s.inputTimes[i + 1]) i++;

    // if past last key, return last rotation
    if (i + 1 == s.inputTimes.size()) {
        const glm::vec4 &v = s.outputsVec4.back();
        return glm::normalize(glm::quat(v.w, v.x, v.y, v.z));
    }

    // interpolate between key i and i+1
    float t0 = s.inputTimes[i];
    float t1 = s.inputTimes[i + 1];
    float factor = (t - t0) / (t1 - t0);

    const glm::vec4 &v0 = s.outputsVec4[i];
    const glm::vec4 &v1 = s.outputsVec4[i + 1];

    glm::quat q0(v0.w, v0.x, v0.y, v0.z);
    glm::quat q1(v1.w, v1.x, v1.y, v1.z);

    return glm::normalize(glm::slerp(q0, q1, factor));
}



void GLTFModel::computeWorldMatrices() {
    //printf("[DEBUG] computeWorldMatrices() start\n");
    for (size_t i = 0; i < nodes.size(); ++i) {
        if (nodes[i].parent == -1) {
            std::function<void(int,const glm::mat4&)> dfs = [&](int idx,const glm::mat4& parentM) {
                nodes[idx].worldMatrix = parentM * nodes[idx].localMatrix;
                for (int c : nodes[idx].children) dfs(c, nodes[idx].worldMatrix);
            };
            dfs((int)i, glm::mat4(1.0f));
        }
    }

    for (auto &skin : skins) {
        for (size_t j = 0; j < skin.joints.size(); ++j) {
            int nodeIdx = skin.joints[j];
            glm::mat4 jointWorld = nodes[nodeIdx].worldMatrix;
            skin.jointMatrices[j] = jointWorld * skin.inverseBindMatrices[j];
        }
    }
    //printf("[DEBUG] computeWorldMatrices() finished\n");
}

void GLTFModel::update(float timeSeconds) {
    //printf("[DEBUG] update() start, timeSeconds=%f\n", timeSeconds);

    if (activeAnimation < 0 || activeAnimation >= (int)animations.size()) {
        computeWorldMatrices();
        //printf("[DEBUG] update() finished, no active animation\n");
        return;
    }

    Animation& anim = animations[activeAnimation];
    animationTime += timeSeconds;

    if (anim.duration > 0.0f) {
        while (animationTime > anim.duration) animationTime -= anim.duration;
    }

    //printf("[DEBUG] animationTime=%f / duration=%f\n", animationTime, anim.duration);

    struct TRS { glm::vec3 T; glm::quat R; glm::vec3 S; bool hasT=false, hasR=false, hasS=false; };
    std::vector<TRS> trs(nodes.size());

    // Sample all channels
    for (size_t ci = 0; ci < anim.channels.size(); ++ci) {
        auto &ch = anim.channels[ci];
        auto &sam = anim.samplers[ch.samplerIndex];

        //printf("[DEBUG] Animation channel %zu targets node %d, path=%s, sampler has %zu keys\n",
        //       ci, ch.targetNode, ch.path.c_str(), sam.outputsVec4.size());

        for (size_t k = 0; k < sam.outputsVec4.size(); ++k)
            //printf("[DEBUG]   sampler.outputsVec4[%zu] = (%f,%f,%f,%f)\n",
            //       k, sam.outputsVec4[k].x, sam.outputsVec4[k].y, sam.outputsVec4[k].z, sam.outputsVec4[k].w);

        if (ch.path == "translation") {
            glm::vec4 v = sampleVec4(sam, animationTime);
            trs[ch.targetNode].T = glm::vec3(v.x, v.y, v.z);
            trs[ch.targetNode].hasT = true;
            //printf("[DEBUG]   sampled translation = (%f,%f,%f)\n", v.x, v.y, v.z);
        } else if (ch.path == "rotation") {
            glm::quat q = sampleQuat(sam, animationTime);
            trs[ch.targetNode].R = q;
            trs[ch.targetNode].hasR = true;
            //printf("[DEBUG]   sampled rotation = (%f,%f,%f,%f)\n", q.w, q.x, q.y, q.z);
        } else if (ch.path == "scale") {
            glm::vec4 v = sampleVec4(sam, animationTime);
            trs[ch.targetNode].S = glm::vec3(v.x, v.y, v.z);
            trs[ch.targetNode].hasS = true;
            //printf("[DEBUG]   sampled scale = (%f,%f,%f)\n", v.x, v.y, v.z);
        }
    }

    // Apply sampled transforms to nodes
    for (size_t i = 0; i < nodes.size(); ++i) {
        glm::vec3 T = trs[i].hasT ? trs[i].T : nodes[i].localTranslation;
        glm::quat R = trs[i].hasR ? trs[i].R : nodes[i].localRotation;
        glm::vec3 S = trs[i].hasS ? trs[i].S : nodes[i].localScale;

        nodes[i].localMatrix = glm::translate(glm::mat4(1.0f), T)
                             * glm::mat4_cast(R)
                             * glm::scale(glm::mat4(1.0f), S);

        //printf("[DEBUG] Node %zu localMatrix updated: T=(%f,%f,%f) R=(%f,%f,%f,%f) S=(%f,%f,%f)\n",
        //       i, T.x, T.y, T.z, R.w, R.x, R.y, R.z, S.x, S.y, S.z);
    }

    computeWorldMatrices();

    // Update skin matrices
    for (size_t si = 0; si < skins.size(); ++si) {
        Skin &skin = skins[si];
        for (size_t j = 0; j < skin.jointMatrices.size(); ++j) {
            glm::mat4 &m = skin.jointMatrices[j];
            //printf("[DEBUG] Skin %zu joint %zu matrix:\n", si, j);
            //printf("[ %f %f %f %f ]\n", m[0][0], m[0][1], m[0][2], m[0][3]);
            //printf("[ %f %f %f %f ]\n", m[1][0], m[1][1], m[1][2], m[1][3]);
            //printf("[ %f %f %f %f ]\n", m[2][0], m[2][1], m[2][2], m[2][3]);
            //printf("[ %f %f %f %f ]\n", m[3][0], m[3][1], m[3][2], m[3][3]);
        }
    }

    //printf("[DEBUG] update() finished\n");
}




void GLTFModel::draw() {
    //printf("[DEBUG] draw() start\n");
    GLint shaderProgram = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &shaderProgram);
    if (shaderProgram == 0) { printf("[WARN] No shader bound, skipping draw\n"); return; }

    if (!skins.empty()) {
        Skin& skin = skins[0];
        if (!skin.jointMatrices.empty()) {
            GLint loc = glGetUniformLocation(shaderProgram, "u_Joints");
            if (loc >= 0) glUniformMatrix4fv(loc, (GLsizei)skin.jointMatrices.size(), GL_FALSE, (const GLfloat*)skin.jointMatrices.data());
            //printf("[DEBUG] Uploaded skin joints\n");
        }
    }

    for (size_t i = 0; i < meshParts.size(); ++i) {
        GLTFMeshPart &p = meshParts[i];

        if (p.materialIndex >= 0 && p.materialIndex < (int)materials.size()) {
            Material &mat = materials[p.materialIndex];
            GLint locFactor = glGetUniformLocation(shaderProgram, "uBaseColorFactor");
            if (locFactor >= 0) glUniform4fv(locFactor, 1, glm::value_ptr(mat.baseColorFactor));

            bool hasTex = (mat.baseColorTex != 0);
            GLint locHasTex = glGetUniformLocation(shaderProgram, "uHasBaseColorTex");
            if (locHasTex >= 0) glUniform1i(locHasTex, hasTex ? 1 : 0);

            if (hasTex) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, mat.baseColorTex);
                GLint locTex = glGetUniformLocation(shaderProgram, "uBaseColorTex");
                if (locTex >= 0) glUniform1i(locTex, 0);
            }
        }

        glBindVertexArray(p.vao);
        glDrawElements(GL_TRIANGLES, p.indexCount, p.indexType, 0);
        //printf("[DEBUG] Drawn mesh part %zu, indices=%d\n", i, p.indexCount);
    }

    glBindVertexArray(0);
    //printf("[DEBUG] draw() finished\n");
}
