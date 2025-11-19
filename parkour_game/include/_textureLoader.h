#ifndef _TEXTURELOADER_H
#define _TEXTURELOADER_H

#include <_common.h>
#include <SOIL2.h>

class _textureLoader
{
    public:
        _textureLoader();
        virtual ~_textureLoader();

        GLuint loadTexture(char *);               //to read img file
        void bindTexture();                     //to bind img to a model

        unsigned char *image;                   //to handle img data
        int width, height;                      //img width and height

        GLuint textID;                          //img data buffer handler

    protected:

    private:
};

#endif // _TEXTURELOADER_H
