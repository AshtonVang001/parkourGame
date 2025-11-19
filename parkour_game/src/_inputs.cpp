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




void _inputs::keyPressed(_skyBox* sky)
{
    switch(wParam) {
    case VK_LEFT:
        sky->rotation.y -= 1.0;
        break;
    case VK_RIGHT:
        sky->rotation.y += 1.0;
        break;
    case VK_UP:
        sky->rotation.x += 1.0;
        break;
    case VK_DOWN:
        sky->rotation.x -= 1.0;
        break;
    case VK_ADD:
        //prlx->
        break;
    case VK_SUBTRACT:
        //prlx->
        break;
    }
}

void _inputs::keyPressed(_camera* myCamera, float deltaTime)
{
    //cout << wParam << endl;           //to get keyboard input


    float moveSpeed = 40.0f * deltaTime;

    // Sprint
    if (keys[16])
        moveSpeed *= 1.5;

    // Forward / Back
    if (keys['W'])
        myCamera->camMoveFdBd(moveSpeed);
    if (keys['S'])
        myCamera->camMoveFdBd(-moveSpeed);

    // Left / Right
    if (keys['A'])
        myCamera->camMoveLtRt(moveSpeed);
    if (keys['D'])
        myCamera->camMoveLtRt(-moveSpeed);

    // Jump
    if (keys[VK_SPACE])  // Spacebar
        myCamera->jump();

    // Update vertical position
    myCamera->updateVertical(deltaTime);
}

/*
void _inputs::keyPressed(_3DModelLoader* Ply, _3DModelLoader* Wpn)
{
    switch(wParam) {
        case 83:
            Wpn->actionTrigger = Ply->actionTrigger = Ply->RUN;
            break;
        case 32:
            Wpn->actionTrigger = Ply->actionTrigger = Ply->JUMP;
            break;
        case 68:
            Wpn->actionTrigger = Ply->actionTrigger = Ply->ATTACK;
            break;
        case 65:
            Wpn->actionTrigger = Ply->actionTrigger = Ply->PAIN;
            break;
        default:
            Wpn->actionTrigger = Ply->actionTrigger = Ply->STAND;
            break;
    }
}
*/




void _inputs::keyUp()
{
    switch(wParam)
    {
        default: break;
    }
}





void _inputs::mouseEventDown(_model* mdl, double x, double y)
{
    prev_MouseX = x;
    prev_MouseY = y;

    switch(wParam)
    {
    case MK_LBUTTON:
        isRotationActive = true;                            //activate rotation flag
        break;
    case MK_RBUTTON:
        isTranslationActive = true;                         //activate translation flag
        break;
    case MK_MBUTTON:
        break;
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
    float sensitivity = 0.1f; // tweak for mouse speed

    double deltaX = x - prev_MouseX;
    double deltaY = y - prev_MouseY;

    myCamera->rotAngle.x += -deltaX * sensitivity; // yaw
    myCamera->rotAngle.y += -deltaY * sensitivity; // pitch (invert Y)

    myCamera->rotateXY(); // update des based on rotation

    prev_MouseX = x;
    prev_MouseY = y;
}
