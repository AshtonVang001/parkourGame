#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <vector>
#include <unordered_map>

#include <GL/glew.h>
#include <GL/gl.h>
#include <_common.h>
#include <cgltf.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

struct Triangle {
    vec3 a, b, c;
};

class GltfModel {
public:
    // geometry
    std::vector<float> vertices;    // x,y,z
    std::vector<float> normals;     // nx,ny,nz
    std::vector<float> texcoords;   // u,v
    std::vector<unsigned int> indices;
    std::vector<Triangle> triangles;



    // GL handles
    GLuint vbo = 0;
    GLuint nbo = 0;
    GLuint tbo = 0;
    GLuint ebo = 0;
    GLuint textureID = 0;

    // cgltf data pointer (owned by this model or by loader; do NOT free data while this model uses it)
    cgltf_data* data = nullptr;

    // node transform caches (one per cgltf node, filled in computeGlobalTransforms())
    std::vector<glm::mat4> nodeLocalTransforms;
    std::vector<glm::mat4> nodeGlobalTransforms;

    // animation helpers
    void updateAnimation(float timeInSeconds);             // public: advance animation (seconds)
    void applyAnimationToNodes(float timeInSeconds);       // apply keyframes to cgltf nodes

    // GPU
    void uploadToGPU();
    void draw();

    // utility: set cgltf_data pointer (call this if loader returned data and you want model to keep it)
    void setCgltfData(cgltf_data* d);

    void buildTriangleList();

private:
    glm::mat4 computeLocalMatrix(const cgltf_node* node) const;
    void computeGlobalTransforms(); // populates nodeGlobalTransforms by walking scene graph
    // small helper to find node index by pointer
    void ensureNodeTransformArrays();

    // helper to sample accessor floats and vec components
    float readAccessorFloat(const cgltf_accessor* acc, size_t index) const;
    void readAccessorVec3(const cgltf_accessor* acc, size_t index, float out3[3]) const;
    void readAccessorQuat(const cgltf_accessor* acc, size_t index, float out4[4]) const;
};
//new
