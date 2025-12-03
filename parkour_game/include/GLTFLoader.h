#pragma once
#define GLM_ENABLE_EXPERIMENTAL


#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>
#include <cassert>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "cgltf.h"
#include "SOIL2.h"
#include "_timer.h"

struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 uv;
    glm::vec4 joints;   // up to 4 joint indices
    glm::vec4 weights;  // up to 4 weights
};

struct GLTFMeshPart {
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;
    GLsizei indexCount = 0;
    GLenum indexType = GL_UNSIGNED_INT;
    int materialIndex = -1;
};

struct Material {
    GLuint baseColorTex = 0;
    glm::vec4 baseColorFactor = glm::vec4(1.0f);
};

struct Node {
    int parent = -1;
    std::vector<int> children;

    glm::vec3 localTranslation = glm::vec3(0.0f);
    glm::quat localRotation    = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 localScale       = glm::vec3(1.0f);

    glm::mat4 localMatrix = glm::mat4(1.0f);
    glm::mat4 worldMatrix = glm::mat4(1.0f);

    int meshIndex = -1;
    int skinIndex = -1;
};


struct Skin {
    int skeletonRoot = -1;
    std::vector<glm::mat4> inverseBindMatrices;
    std::vector<int> joints;
    std::vector<glm::mat4> jointMatrices;
};

struct AnimationSampler {
    std::vector<float> inputTimes;
    std::vector<glm::vec4> outputsVec4;
    cgltf_interpolation_type interp = cgltf_interpolation_type_linear;
};

struct AnimationChannel {
    int targetNode = -1;
    std::string path;
    int samplerIndex = -1;
};

struct Animation {
    float duration = 0.0f;
    std::vector<AnimationSampler> samplers;
    std::vector<AnimationChannel> channels;
};

class GLTFModel {
public:
    GLTFModel() = default;
    ~GLTFModel() { freeGLResources(); }

    bool load(const std::string& filename);
    void update(float timeSeconds);
    void draw();

    void setActiveAnimation(int idx) { activeAnimation = idx; animationTime = 0.0f; }
    int getAnimationCount() const { return (int)animations.size(); }

    std::vector<Node> nodes;
    std::vector<GLTFMeshPart> meshParts;
    std::vector<Material> materials;
    std::vector<Skin> skins;
    std::vector<Animation> animations;

private:
    bool loadCgltfFile(const std::string& filename);
    void freeGLResources();
    void buildMeshes(cgltf_data* data);
    void buildMaterials(cgltf_data* data, const std::string& basePath);
    void buildNodes(cgltf_data* data);
    void buildSkins(cgltf_data* data);
    void buildAnimations(cgltf_data* data);
    void computeWorldMatrices();
    glm::mat4 nodeLocalFromCgltf(cgltf_node* n);

    glm::vec4 sampleVec4(const AnimationSampler& s, float t) const;
    glm::quat sampleQuat(const AnimationSampler& s, float t) const;

    cgltf_data* cgltfData = nullptr;
    std::string gltfBasePath;

    int activeAnimation = 0;
    float animationTime = 0.0f;
};
