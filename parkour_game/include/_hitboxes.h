/*
#ifndef _HITBOXES_H
#define _HITBOXES_H

#include <_common.h>

class _hitboxes
{
    public:
        _hitboxes();
        virtual ~_hitboxes();

        void drawHitboxes();

    protected:

    private:
};

#endif // _HITBOXES_H
*/




#pragma once
#include <vector>
#include <_common.h>
#include <_timer.h>

struct Hitbox {
    vec3 center;
    vec3 halfSize;
};

class _hitboxes {
public:
    _hitboxes();
    ~_hitboxes();

    void drawHitboxes();
    void populateHitboxes();
    float raycastY(const vec3& origin);
    void debugBoxes();

    _timer* myTime = new _timer();


private:
    std::vector<Hitbox> boxes;
};
