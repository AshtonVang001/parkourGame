#include "_inputs.h"

_inputs::_inputs()
{
    //ctor
    isRotationActive = false;
    isTranslationActive = false;
    //isScalingActive = false;
}

_inputs::~_inputs()
{
    //dtor
}

void _inputs::keyPressed(_camera* myCamera, float deltaTime)
{
    //cout << wParam << endl;           //to get keyboard input


    float moveSpeed = 40.0f * deltaTime;

    // ---- Sprint ----
    if (keys[16])
        moveSpeed *= 1.5;

    // ---- Forward / Back ----
    if (keys['W'])
        myCamera->camMoveFdBd(moveSpeed);
    if (keys['S'])
        myCamera->camMoveFdBd(-moveSpeed);

    // ---- Left / Right ----
    if (keys['A'])
        myCamera->camMoveLtRt(moveSpeed);
    if (keys['D'])
        myCamera->camMoveLtRt(-moveSpeed);

    // ---- Jump ----
    if (keys[VK_SPACE])  // Spacebar
        myCamera->jump();

    // ---- Update vertical pos ----
    myCamera->updateVertical(deltaTime);
}

void _inputs::keyUp()
{
    switch(wParam)
    {
        default: break;
    }
}

void _inputs::mouseEventUp()
{
    isRotationActive = false;                               //deactivate flags
    isTranslationActive = false;
}






void _inputs::mouseWheel(_model* mdl, double delta)
{
    mdl->posZ += delta / 100.0;                             //zoom model when wheel in action
}






void _inputs::mouseMove(_camera* myCamera, double x, double y)
{
    float sensitivity = 0.1f; // ---- mouse speed ----

    double deltaX = x - prev_MouseX;
    double deltaY = y - prev_MouseY;

    myCamera->rotAngle.x += -deltaX * sensitivity; // yaw
    myCamera->rotAngle.y += -deltaY * sensitivity; // pitch (invert Y)

    myCamera->rotateXY(); // ---- update des ----

    prev_MouseX = x;
    prev_MouseY = y;
}
