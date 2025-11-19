#pragma once

#include <string>
#include <_common.h>
#include "cgltf.h"
#include <iostream>
#include <SOIL2.h>

class GltfModel;

class _gltfLoader {
public:
    _gltfLoader();
    ~_gltfLoader();

    GltfModel* loadModel(const std::string& filename);
    cgltf_data* data = nullptr;
};
