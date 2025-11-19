#ifndef _BULLETS_H
#define _BULLETS_H

#include <_common.h>
#include <_textureLoader.h>
#include <_timer.h>

class _bullets
{
    public:
        _bullets();
        virtual ~_bullets();

        _textureLoader *tex = new _textureLoader();
        _timer *myTimer = new _timer();

        vec3 pos;
        vec3 des;
        vec3 src;

        vec3 scale;
        float t = 0;            //for parametric equation

        bool isAlive;

        void initBullet(char *);
        void drawBullet();
        void bulletActions();

        int actionTrigger;

        enum{READY, SHOOT, HIT};

    protected:

    private:
};

#endif // _BULLETS_H
