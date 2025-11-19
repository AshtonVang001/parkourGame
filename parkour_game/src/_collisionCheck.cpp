/**
#include "_collisionCheck.h"

_collisionCheck::_collisionCheck()
{
    //ctor
}

_collisionCheck::~_collisionCheck()
{
    //dtor
}

bool _collisionCheck::isLinearCol(vec3, vec3)
{

}

bool _collisionCheck::isRadialCol(vec2 p1, vec2 p2, float r1, float r2, float thrhld)
{
    return(sqrt(pow((p1.x - p2.x), 2) + pow((p1.y - p2.y), 2)) - (r1 + r2) < thrhld);
}

bool _collisionCheck::isSphereCol(vec3 p1, vec3 p2, float r1, float r2, float thrhld)
{
    return(sqrt(pow((p1.x - p2.x), 2) + pow((p1.y - p2.y), 2) + pow((p1.z - p2.z), 2)) - (r1 + r2) < thrhld);
}

bool _collisionCheck::isPlanoCol(vec2 p1, vec2 p2)
{

}

bool _collisionCheck::isCubicCol(vec3, vec3)
{

}
**/



#include "_collisionCheck.h"

// ---------- BASIC VEC2/VEC3 MATH (NO GLM) ----------
inline vec3 v3add(const vec3& a, const vec3& b) {
    return { a.x + b.x, a.y + b.y, a.z + b.z };
}

inline vec3 v3sub(const vec3& a, const vec3& b) {
    return { a.x - b.x, a.y - b.y, a.z - b.z };
}

inline vec3 v3mul(const vec3& a, float s) {
    return { a.x * s, a.y * s, a.z * s };
}

inline float v3dot(const vec3& a, const vec3& b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

inline vec3 v3cross(const vec3& a, const vec3& b) {
    return {
        a.y*b.z - a.z*b.y,
        a.z*b.x - a.x*b.z,
        a.x*b.y - a.y*b.x
    };
}

inline vec3 v3normalize(const vec3& v) {
    float len = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
    if (len == 0) return {0,0,0};
    return { v.x/len, v.y/len, v.z/len };
}


// -------------------------------------------------------------

_collisionCheck::_collisionCheck() {}
_collisionCheck::~_collisionCheck() {}

// Basic original methods
bool _collisionCheck::isLinearCol(vec3 a, vec3 b)
{
    float d2 = sqr(a.x-b.x) + sqr(a.y-b.y) + sqr(a.z-b.z);
    return d2 < 0.0001f;
}

bool _collisionCheck::isRadialCol(vec2 p1, vec2 p2, float r1, float r2, float thrhld)
{
    float d = sqrt( sqr(p1.x - p2.x) + sqr(p1.y - p2.y) );
    return (d - (r1+r2)) < thrhld;
}

bool _collisionCheck::isSphereCol(vec3 p1, vec3 p2, float r1, float r2, float thrhld)
{
    float d = sqrt( sqr(p1.x-p2.x)+sqr(p1.y-p2.y)+sqr(p1.z-p2.z) );
    return (d - (r1+r2)) < thrhld;
}

bool _collisionCheck::isPlanoCol(vec2 p1, vec2 p2)
{
    return fabs(p1.x - p2.x) < 0.0001f &&
           fabs(p1.y - p2.y) < 0.0001f;
}

bool _collisionCheck::isCubicCol(vec3 a, vec3 b)
{
    return fabs(a.x - b.x) < 0.01f &&
           fabs(a.y - b.y) < 0.01f &&
           fabs(a.z - b.z) < 0.01f;
}


// -------------------------------------------------------------
// 3D line-segment vs line-segment distance
// -------------------------------------------------------------
bool _collisionCheck::isLinearCol(const vec3& p1, const vec3& p2,
                                  const vec3& q1, const vec3& q2,
                                  float threshold)
{
    vec3 u = v3sub(p2, p1);
    vec3 v = v3sub(q2, q1);
    vec3 w = v3sub(p1, q1);

    float a = v3dot(u,u);
    float b = v3dot(u,v);
    float c = v3dot(v,v);
    float d = v3dot(u,w);
    float e = v3dot(v,w);
    float D = a*c - b*b;

    float sc, sN, sD = D;
    float tc, tN, tD = D;

    const float SMALL = 1e-8f;

    if (D < SMALL) {
        sN = 0.0f;
        sD = 1.0f;
        tN = e;
        tD = c;
    } else {
        sN = (b*e - c*d);
        tN = (a*e - b*d);

        if (sN < 0) { sN = 0; tN = e; tD = c; }
        else if (sN > sD) { sN = sD; tN = e + b; tD = c; }
    }

    if (tN < 0) {
        tN = 0;
        if (-d < 0) sN = 0;
        else if (-d > a) sN = sD;
        else { sN = -d; sD = a; }
    }
    else if (tN > tD) {
        tN = tD;
        if ((-d + b) < 0) sN = 0;
        else if ((-d + b) > a) sN = sD;
        else { sN = (-d + b); sD = a; }
    }

    sc = (fabs(sN) < SMALL ? 0.0f : sN / sD);
    tc = (fabs(tN) < SMALL ? 0.0f : tN / tD);

    vec3 dP = v3sub(v3add(w, v3mul(u, sc)), v3mul(v, tc));

    float dist2 = v3dot(dP, dP);
    return dist2 <= threshold * threshold;
}


// -------------------------------------------------------------
// AABB helpers
// -------------------------------------------------------------
bool _collisionCheck::pointInAABB(const vec3& p, const vec3& mn, const vec3& mx)
{
    return (p.x >= mn.x && p.x <= mx.x &&
            p.y >= mn.y && p.y <= mx.y &&
            p.z >= mn.z && p.z <= mx.z);
}

bool _collisionCheck::aabbOverlap(const vec3& minA, const vec3& maxA,
                                  const vec3& minB, const vec3& maxB)
{
    if (maxA.x < minB.x || minA.x > maxB.x) return false;
    if (maxA.y < minB.y || minA.y > maxB.y) return false;
    if (maxA.z < minB.z || minA.z > maxB.z) return false;
    return true;
}


// -------------------------------------------------------------
// Ray / Triangle intersection (Möller–Trumbore)
// -------------------------------------------------------------
bool _collisionCheck::rayIntersectTriangle(const vec3& orig, const vec3& dir,
                                           const vec3& v0, const vec3& v1, const vec3& v2,
                                           float& outT, float& outU, float& outV)
{
    const float EPS = 1e-8f;

    vec3 edge1 = v3sub(v1, v0);
    vec3 edge2 = v3sub(v2, v0);

    vec3 pvec = v3cross(dir, edge2);
    float det = v3dot(edge1, pvec);

    if (fabs(det) < EPS) return false;

    float invDet = 1.0f / det;

    vec3 tvec = v3sub(orig, v0);
    outU = v3dot(tvec, pvec) * invDet;
    if (outU < 0 || outU > 1) return false;

    vec3 qvec = v3cross(tvec, edge1);
    outV = v3dot(dir, qvec) * invDet;
    if (outV < 0 || outU + outV > 1) return false;

    outT = v3dot(edge2, qvec) * invDet;
    return outT > EPS;
}


// -------------------------------------------------------------
// Raycast mesh (find nearest triangle hit)
// -------------------------------------------------------------
bool _collisionCheck::raycastMeshNearest(const vec3& orig, const vec3& dir,
                                         const std::vector<vec3>& triangles,
                                         float& hitT, vec3& hitPos)
{
    if (triangles.size() % 3 != 0) return false;

    bool hit = false;
    float bestT = 1e30f;

    for (size_t i = 0; i < triangles.size(); i += 3)
    {
        float t, u, v;
        if (rayIntersectTriangle(orig, dir,
                                 triangles[i], triangles[i+1], triangles[i+2],
                                 t, u, v))
        {
            if (t < bestT) {
                bestT = t;
                hit = true;
            }
        }
    }

    if (!hit) return false;

    hitT = bestT;
    hitPos = v3add(orig, v3mul(dir, bestT));
    return true;
}


// -------------------------------------------------------------
// Point to plane distance
// -------------------------------------------------------------
float _collisionCheck::pointPlaneSignedDistance(const vec3& point,
                                                const vec3& planePoint,
                                                const vec3& planeNormal)
{
    vec3 N = v3normalize(planeNormal);
    return v3dot( v3sub(point, planePoint), N );
}
