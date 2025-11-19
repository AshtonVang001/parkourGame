#include "_bullets.h"

_bullets::_bullets()
{
    //ctor
    pos.x = pos.y = pos.z = 0;
    src.x = src.y = src.z = 0;
    //t = 0;

    des.x = 0;
    des.y = 20;
    des.z = -50;

    isAlive = false;
}

_bullets::~_bullets()
{
    //dtor
}

void _bullets::initBullet(char* filename)
{
    //if we use texture on bullets
}

void _bullets::drawBullet()
{
    glDisable(GL_TEXTURE_2D);                       //only if you are using a glutSphere

    glPushMatrix();
        if (isAlive) {
            glTranslatef(pos.x, pos.y, pos.z);
            glutSolidSphere(0.5, 20, 20);
        }
    glPopMatrix();

    glEnable(GL_TEXTURE_2D);
}

void _bullets::bulletActions()
{
    switch(actionTrigger)
    {
    case READY:
        pos.x = pos.y = pos.z = 0;
        des.x = des.y = des.z = 0;
        isAlive = false;
        t = 0;
        break;

    case SHOOT:                                     //applying parametric equation
        if (isAlive) {                              //p = p1 + t (p2 - p1)
            pos.x = src.x + t * (des.x - src.x);
            pos.y = src.y + t * (des.y - src.y);
            pos.z = src.z + t * (des.z - src.z);

            src.x = pos.x;
            src.y = pos.y;
            src.z = pos.z;

            if (t < 1) t+= 0.001;
            else actionTrigger = READY;
        }
        break;

    case HIT:
        break;
    }
}
