#ifndef _COLLISIONCHECK_H
#define _COLLISIONCHECK_H

#include <_common.h>
#include <vector>
#include <cmath>
#include <gltfModel.h>

class _collisionCheck
{
public:
    _collisionCheck();
    virtual ~_collisionCheck();

    // ---- original versions ----
    bool isLinearCol(vec3, vec3);
    bool isRadialCol(vec2 p1, vec2 p2, float r1, float r2, float thrhld);
    bool isSphereCol(vec3 p1, vec3 p2, float r1, float r2, float thrhld);
    bool isPlanoCol(vec2 p1, vec2 p2);
    bool isCubicCol(vec3, vec3);

    // ---- collision helpers ----
    bool isLinearCol(const vec3& p1, const vec3& p2,
                     const vec3& q1, const vec3& q2,
                     float threshold = 0.01f);

    bool pointInAABB(const vec3& point, const vec3& minAABB, const vec3& maxAABB);
    bool aabbOverlap(const vec3& minA, const vec3& maxA,
                     const vec3& minB, const vec3& maxB);

    // ---- Ray / triangle ----
    bool rayIntersectTriangle(const vec3& orig, const vec3& dir,
                              const vec3& v0, const vec3& v1, const vec3& v2,
                              float& outT, float& outU, float& outV);

    bool raycastMeshNearest(const vec3& orig, const vec3& dir,
                        const std::vector<Triangle>& triangles,
                        float& hitT, vec3& hitPos);

    float pointPlaneSignedDistance(const vec3& point,
                                   const vec3& planePoint,
                                   const vec3& planeNormal);

private:
    float sqr(float x) { return x*x; }
};

#endif
