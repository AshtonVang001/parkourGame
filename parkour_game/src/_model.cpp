#include "_model.h"
#include <_common.h>

_model::_model()
{
    //ctor
    rotateX = 0;
    rotateY = 0;
    rotateZ = 0;

    posX = 0;
    posY = 0;
    posZ = -8.0;

    scale = 1;
}

_model::~_model()
{
    //dtor
}

void _model::drawModel()
{
    glPushMatrix();                         //start group
    //glColor3f(1.0, 0.5, 0);                 //set colors

    glTranslated(posX, posY, posZ);         //translation
    glRotated(rotateX, 1, 0, 0);            //rotate around X-axis
    glRotated(rotateY, 0, 1, 0);            //rotate around Y-axis
    glRotated(rotateZ, 0, 0, 1);            //rotate around Z-axis

    glScaled(scale, scale, scale);          //scale (you can use multiple values to change individual parameters)

    glutSolidTeapot(1.5);                   //draw model
    glPopMatrix();                          //end group
}
