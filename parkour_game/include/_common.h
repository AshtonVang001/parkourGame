#ifndef _COMMON_H
#define _COMMON_H

#include <GL/glew.h>

#include <windows.h>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fstream>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SOIL2.h>

#define PI 3.14159265358979

using namespace std;

static float deltaTime;

typedef struct{

    float x;
    float y;

} vec2;

typedef struct{

    float x;
    float y;
    float z;

} vec3;

typedef struct{

    float r;
    float g;
    float b;

} col3;

typedef struct{

    float r;
    float g;
    float b;
    float a;

} col4;






// ----- vec3 operators -----
inline vec3 operator+(const vec3& a, const vec3& b) {
    return { a.x + b.x, a.y + b.y, a.z + b.z };
}

inline vec3 operator-(const vec3& a, const vec3& b) {
    return { a.x - b.x, a.y - b.y, a.z - b.z };
}

inline vec3 operator*(const vec3& a, float s) {
    return { a.x * s, a.y * s, a.z * s };
}

inline vec3 operator*(float s, const vec3& a) {
    return { a.x * s, a.y * s, a.z * s };
}

inline vec3 operator/(const vec3& a, float s) {
    return { a.x / s, a.y / s, a.z / s };
}

// ----- dot product -----
inline float dot(const vec3& a, const vec3& b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

// ----- cross product -----
inline vec3 cross(const vec3& a, const vec3& b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

// ----- normalize -----
inline vec3 normalize(const vec3& v) {
    float len = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
    if (len == 0) return {0,0,0};
    return { v.x / len, v.y / len, v.z / len };
}



#endif // _COMMON_H
