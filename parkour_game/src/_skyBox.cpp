#include "_skyBox.h"

_skyBox::_skyBox()
{
    //ctor
}

_skyBox::~_skyBox()
{
    //dtor
}

void _skyBox::skyBoxInit()
{
    xMin = yMin = 0;
    xMax = yMax = 1;

    glGenTextures(6, tex);

    rotation.x = rotation.y = rotation.z = 0;
    pos.x = pos.y = 0;
    pos.z = -9.0;

    boxSize.x = 100.0;
    boxSize.z = 400.0;
    boxSize.y = 100.0;
}

void _skyBox::drawSkyBox()
{
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glColor3f(1.0, 1.0, 1.0);                       //set environment to white
    glScalef(boxSize.x, boxSize.y, boxSize.z);
    glRotatef(rotation.x, 1,0,0);
    glRotatef(rotation.y, 0,1,0);
    glRotatef(rotation.z, 0,0,1);

    //front wall

    glBindTexture(GL_TEXTURE_2D, tex[0]);
    //glNormal3f();                                 //use this only if you are setting room with lighting
    glBegin(GL_QUADS);

        glTexCoord2f(xMin, yMin); glVertex3f(-1.0, 1.0, 1.0);
        glTexCoord2f(xMax, yMin); glVertex3f(1.0, 1.0, 1.0);
        glTexCoord2f(xMax, yMax); glVertex3f(1.0, -1.0, 1.0);
        glTexCoord2f(xMin, yMax); glVertex3f(-1.0, -1.0, 1.0);

    glEnd();

    //back wall

    glBindTexture(GL_TEXTURE_2D, tex[1]);
    //glNormal3f();                                 //use this only if you are setting room with lighting
    glBegin(GL_QUADS);

        glTexCoord2f(xMin, yMax); glVertex3f(1.0, -1.0, -1.0);
        glTexCoord2f(xMax, yMax); glVertex3f(-1.0, -1.0, -1.0);
        glTexCoord2f(xMax, yMin); glVertex3f(-1.0, 1.0, -1.0);
        glTexCoord2f(xMin, yMin); glVertex3f(1.0, 1.0, -1.0);

    glEnd();

    //top wall

    glBindTexture(GL_TEXTURE_2D, tex[2]);
    //glNormal3f();                                 //use this only if you are setting room with lighting
    glBegin(GL_QUADS);

        glTexCoord2f(xMin, yMax); glVertex3f(1.0, 1.0, -1.0);
        glTexCoord2f(xMin, yMin); glVertex3f(1.0, 1.0, 1.0);
        glTexCoord2f(xMax, yMin); glVertex3f(-1.0, 1.0, 1.0);
        glTexCoord2f(xMax, yMax); glVertex3f(-1.0, 1.0, -1.0);

    glEnd();

    //bottom wall

    glBindTexture(GL_TEXTURE_2D, tex[3]);
    //glNormal3f();                                 //use this only if you are setting room with lighting
    glBegin(GL_QUADS);

        glTexCoord2f(xMax, yMax); glVertex3f(-1.0, -1.0, 1.0);
        glTexCoord2f(xMin, yMax); glVertex3f(1.0, -1.0, 1.0);
        glTexCoord2f(xMin, yMin); glVertex3f(1.0, -1.0, -1.0);
        glTexCoord2f(xMax, yMin); glVertex3f(-1.0, -1.0, -1.0);

    glEnd();

    //right wall

        glBindTexture(GL_TEXTURE_2D, tex[4]);
    //glNormal3f();                                 //use this only if you are setting room with lighting
    glBegin(GL_QUADS);

        glTexCoord2f(xMin, yMin); glVertex3f(-1.0, 1.0, -1.0);
        glTexCoord2f(xMax, yMin); glVertex3f(-1.0, 1.0, 1.0);
        glTexCoord2f(xMax, yMax); glVertex3f(-1.0, -1.0, 1.0);
        glTexCoord2f(xMin, yMax); glVertex3f(-1.0, -1.0, -1.0);

    glEnd();

    //left wall

    glBindTexture(GL_TEXTURE_2D, tex[5]);
    //glNormal3f();                                 //use this only if you are setting room with lighting
    glBegin(GL_QUADS);

        glTexCoord2f(xMax, yMin); glVertex3f(1.0, 1.0, -1.0);
        glTexCoord2f(xMax, yMax); glVertex3f(1.0, -1.0, -1.0);
        glTexCoord2f(xMin, yMax); glVertex3f(1.0, -1.0, 1.0);
        glTexCoord2f(xMin, yMin); glVertex3f(1.0, 1.0, 1.0);

    glEnd();

    glPopMatrix();

    glEnable(GL_LIGHTING);
}
