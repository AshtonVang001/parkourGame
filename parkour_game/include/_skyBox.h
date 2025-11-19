#ifndef _SKYBOX_H
#define _SKYBOX_H

#include <_common.h>
#include <_textureLoader.h>


class _skyBox
{
    public:
        _skyBox();
        virtual ~_skyBox();

        _textureLoader *textures = new _textureLoader();

        GLuint tex[6];              //all the images
        vec3 pos;                   //position of the skybox
        vec3 rotation;              //rotation of the skybox
        vec3 boxSize;               //size of the skybox

        float xMin, yMin, xMax, yMax;

        void skyBoxInit();          //load images
        void drawSkyBox();

    protected:

    private:
};

#endif // _SKYBOX_H
