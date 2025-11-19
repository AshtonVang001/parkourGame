#ifndef _PARALLAX_H
#define _PARALLAX_H

#include <_common.h>
#include <_textureLoader.h>
#include <_timer.h>

class _parallax
{
    public:
        _parallax();
        virtual ~_parallax();

        _textureLoader *btex = new _textureLoader();
        _timer *myTime = new _timer();

        void drawParallax(float, float);                    //width & height
        void parallaxInit(char*);                           //initialize and load image
        void prlxScrollAuto(string, float);                 //direction and speed

        float xMin, xMax, yMin, yMax;                       //texture coordinates

    protected:

    private:
};

#endif // _PARALLAX_H
