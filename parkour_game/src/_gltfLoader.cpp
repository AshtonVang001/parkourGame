#include "_gltfLoader.h"
#include "gltfModel.h"
#include "cgltf.h"
#include <iostream>
#include <SOIL2.h>

_gltfLoader::_gltfLoader() {}
_gltfLoader::~_gltfLoader() {
    if (data) {
        //cgltf_free(data);
        //data = nullptr;
    }
}

GltfModel* _gltfLoader::loadModel(const std::string& filename)
{
    GltfModel* model = new GltfModel();

    cgltf_options options = {};
    cgltf_data* data = nullptr;

    cgltf_result res = cgltf_parse_file(&options, filename.c_str(), &data);
    if (res != cgltf_result_success) {
        std::cerr << "Failed to parse GLTF file: " << filename << std::endl;
        return nullptr;
    }

    res = cgltf_load_buffers(&options, data, filename.c_str());
    if (res != cgltf_result_success) {
        std::cerr << "Failed to load GLTF buffers: " << filename << std::endl;
        //cgltf_free(data);
        return nullptr;
    }

    // ---- Store cgltf_data in the model ----
    model->setCgltfData(data);   // <-- this keeps a pointer inside your model

    std::cout << "Animations: " << data->animations_count << std::endl;


    // --- Extract first material texture (base color) if present ---
    if (data->materials_count > 0) {
        cgltf_material* mat = &data->materials[0];
        if (mat->has_pbr_metallic_roughness && mat->pbr_metallic_roughness.base_color_texture.texture) {
            cgltf_texture* tex = mat->pbr_metallic_roughness.base_color_texture.texture;
            cgltf_image* img = tex->image;

            if (img->uri == nullptr && img->buffer_view) {
                const unsigned char* buffer = (const unsigned char*)img->buffer_view->buffer->data + img->buffer_view->offset;
                size_t size = img->buffer_view->size;

                GLuint texID;
                glGenTextures(1, &texID);
                glBindTexture(GL_TEXTURE_2D, texID);

                int w, h, channels;
                unsigned char* imageData = SOIL_load_image_from_memory(buffer, (int)size, &w, &h, &channels, SOIL_LOAD_RGBA);
                if (imageData) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    SOIL_free_image_data(imageData);

                    model->textureID = texID;
                    std::cout << "GLTF: Embedded texture loaded, ID = " << texID << "\n";
                } else {
                    std::cerr << "GLTF: Failed to load embedded texture\n";
                }
            }
        }
    }

    // ---- MESH LOOP ----
    for (size_t m = 0; m < data->meshes_count; ++m)
    {
        cgltf_mesh& mesh = data->meshes[m];
        for (size_t p = 0; p < mesh.primitives_count; ++p)
        {
            cgltf_primitive& prim = mesh.primitives[p];
            if (prim.type != cgltf_primitive_type_triangles) continue;

            // --- Attributes ---
            for (size_t a = 0; a < prim.attributes_count; ++a)
            {
                cgltf_attribute& attr = prim.attributes[a];
                cgltf_accessor* accessor = attr.data;
                cgltf_buffer_view* view = accessor->buffer_view;
                const unsigned char* buffer = (const unsigned char*)view->buffer->data + view->offset + accessor->offset;

                if (attr.type == cgltf_attribute_type_position) {
                    for (size_t i = 0; i < accessor->count; ++i) {
                        const float* pos = (const float*)(buffer + i * sizeof(float) * 3);
                        model->vertices.push_back(pos[0]);
                        model->vertices.push_back(pos[1]);
                        model->vertices.push_back(pos[2]);
                    }
                } else if (attr.type == cgltf_attribute_type_texcoord) {
                    for (size_t i = 0; i < accessor->count; ++i) {
                        const float* uv = (const float*)(buffer + i * sizeof(float) * 2);
                        model->texcoords.push_back(uv[0]);
                        model->texcoords.push_back(uv[1]);
                    }
                } else if (attr.type == cgltf_attribute_type_normal) {
                    for (size_t i = 0; i < accessor->count; ++i) {
                        const float* n = (const float*)(buffer + i * sizeof(float) * 3);
                        model->normals.push_back(n[0]);
                        model->normals.push_back(n[1]);
                        model->normals.push_back(n[2]);
                    }
                }
            }

            // --- Indices ---
            if (prim.indices) {
                cgltf_accessor* accessor = prim.indices;
                for (size_t i = 0; i < accessor->count; ++i) {
                    unsigned int idx = 0;
                    cgltf_accessor_read_uint(accessor, i, &idx, 1);
                    model->indices.push_back(idx);
                }
            }
        }
    }

    // Upload to GPU
    model->uploadToGPU();
    model->setCgltfData(data);

    return model;
}
