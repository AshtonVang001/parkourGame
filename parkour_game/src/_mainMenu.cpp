#include "_mainMenu.h"
#include "gltfModel.h"
#include "_gltfLoader.h"
#include <iostream>

_mainMenu::_mainMenu()
{
    myTime = new _timer();

    // Set all pointers null until initGL()
    myLight = nullptr;
    myModel = nullptr;
    myInput = nullptr;
    myTexture = nullptr;
    myPrlx = nullptr;
    mySkyBox = nullptr;
    mySprite = nullptr;
    mdl3D = nullptr;
    mdl3DW = nullptr;
    myCam = nullptr;
    myCol = nullptr;
    snds = nullptr;

    myGltfModel = nullptr;
}

_mainMenu::~_mainMenu()
{
    delete myTime;

    delete myLight;
    delete myModel;
    delete myInput;
    delete myTexture;
    delete myPrlx;
    delete mySkyBox;
    delete mySprite;
    delete mdl3D;
    delete mdl3DW;
    delete myCam;
    delete myCol;
    delete snds;
    delete myGltfModel;
}

void _mainMenu::reSizeScene(int width, int height)
{
    float aspectRatio = (float)width / (float)height;

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, aspectRatio, 0.1f, 1000.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    this->width = width;
    this->height = height;
}

void _mainMenu::initGL()
{
    // ---- Standard OpenGL setup ----
    glShadeModel(GL_SMOOTH);
    glClearColor(0,0,0,0);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ---- Create subsystems ----
    myLight  = new _light();
    myModel  = new _model();
    myInput  = new _inputs();
    myTexture = new _textureLoader();
    myPrlx   = new _parallax();
    mySkyBox = new _skyBox();
    mySprite = new _sprite();
    mdl3D    = new _3DModelLoader();
    mdl3DW   = new _3DModelLoader();
    myCam    = new _camera();
    myCol    = new _collisionCheck();
    snds     = new _sounds();

    myTime->startTime = clock();

    // ---- Light ----
    myLight->setLight(GL_LIGHT0);

    // ---- Camera ----
    myCam->camInit();

    // ---- Sounds ----
    snds->initSounds();
    snds->playSound("sounds/gameThemeForClass.mp3");

    snds->eng->setSoundVolume(0.3f);

    menuTex->loadTexture("images/menuTexture.png");
    menuUI->loadTexture("images/menuUI.png");
}



void _mainMenu::updateScene()
{
    myTime->updateDeltaTime();

    static float smoothDT = 0.16f;
    smoothDT = (smoothDT * 0.9f) + (myTime->deltaTime * 0.1f);

    //myCam->updateVertical(myTime->deltaTime);
    myCam->rotateXY();

    animTime += myTime->deltaTime;

    if (myInput && myCam) {
        myInput->keyPressed(myCam, smoothDT);
        //myCam->update(smoothDT, myCol, ground);
    }

    bgOffsetX += (bgTargetX - bgOffsetX) * bgMoveSpeed;
    bgOffsetY += (bgTargetY - bgOffsetY) * bgMoveSpeed;

    POINT cursor;
    GetCursorPos(&cursor);
    ScreenToClient(windowHandle, &cursor); // convert to window coords if needed

    updateBackgroundOffset(deltaTime, windowHandle, screenWidth, screenHeight);
}


void _mainMenu::drawScene()
{
    menuTex->bindTexture();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 1, 0, 1, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Compute quad coordinates with scale and offset
    float halfScaleX = bgScale / 2.0f;
    float halfScaleY = bgScale / 2.0f;

    float left   = 0.5f - halfScaleX + bgOffsetX;
    float right  = 0.5f + halfScaleX + bgOffsetX;
    float bottom = 0.5f - halfScaleY + bgOffsetY;
    float top    = 0.5f + halfScaleY + bgOffsetY;

    glBegin(GL_QUADS);
        glTexCoord2f(0, 1); glVertex2f(left, bottom);
        glTexCoord2f(1, 1); glVertex2f(right, bottom);
        glTexCoord2f(1, 0); glVertex2f(right, top);
        glTexCoord2f(0, 0); glVertex2f(left, top);
    glEnd();

    glEnable(GL_DEPTH_TEST);










    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // setup orthographic projection for 0..1 screen space
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, 1, 0, 1, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // bind the logo texture
    menuUI->bindTexture();

    // draw quad
    float logoWidth  = 0.3f;
    float logoHeight = 0.2f;
    float logoLeft   = 0.5f - logoWidth / 2.0f;
    float logoRight  = 0.5f + logoWidth / 2.0f;
    float logoBottom = 0.7f - logoHeight / 2.0f;
    float logoTop    = 0.7f + logoHeight / 2.0f;

    glBegin(GL_QUADS);
        glTexCoord2f(0, 1); glVertex2f(0, 0);
        glTexCoord2f(1, 1); glVertex2f(1, 0);
        glTexCoord2f(1, 0); glVertex2f(1, 1);
        glTexCoord2f(0, 0); glVertex2f(0, 1);
    glEnd();

    // restore projection/modelview
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glEnable(GL_DEPTH_TEST);


}

int _mainMenu::winMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_KEYDOWN:
        myInput->wParam = wParam;
        myInput->keyPressed(mySkyBox);
        //myInput->keyPressed(myCam);
        myInput->keys[wParam] = true;
        break;

    case WM_KEYUP:
        myInput->wParam = wParam;
        myInput->keys[wParam] = false;
        break;

    case WM_LBUTTONDOWN:
        //myInput->mouseEventDown(model, LOWORD(lParam), HIWORD(lParam));
        clickCount = clickCount % 10;

        b[clickCount].src.x = mdl3D->pos.x;
        b[clickCount].src.y = mdl3D->pos.y;
        b[clickCount].src.z = mdl3D->pos.z;

        b[clickCount].des.x = msX;
        b[clickCount].des.y = -msY;
        b[clickCount].des.z = msZ;

        b[clickCount].t = 0;
        b[clickCount].actionTrigger = b[clickCount].SHOOT;
        b[clickCount].isAlive = true;

        clickCount++;

        snds->playSound("sounds/untitled2.mp3");
        break;

    case WM_MOUSEMOVE:
        myInput->mouseMove(myCam, LOWORD(lParam), HIWORD(lParam));
        break;

    case WM_MOUSEWHEEL:
        myInput->mouseWheel(myModel, (double)GET_WHEEL_DELTA_WPARAM(wParam));
        break;
    }

    return 0;
}

void _mainMenu::updateBackgroundOffset(float dt, HWND hWnd, int screenWidth, int screenHeight)
{
    // Step 1: increment delay timer
    if (bgDelayTimer < bgDelayDuration)
    {
        bgDelayTimer += dt;
        bgMoveFactor = bgDelayTimer / bgDelayDuration; // ramps from 0 -> 1
        if (bgMoveFactor > 1.0f) bgMoveFactor = 1.0f;
    }
    else
    {
        bgMoveFactor = 1.0f;
    }

    // Step 2: get mouse position
    POINT cursor;
    GetCursorPos(&cursor);
    ScreenToClient(hWnd, &cursor);

    float nx = ((float)cursor.x / screenWidth) - 0.5f;
    float ny = ((float)cursor.y / screenHeight) - 0.5f;

    float maxOffsetX = (bgScale - 1.0f) / 2.0f;
    float maxOffsetY = (bgScale - 1.0f) / 2.0f;

    bgTargetX = nx * maxOffsetX * 2.0f;
    bgTargetY = -ny * maxOffsetY * 2.0f;

    // Step 3: slowly interpolate toward target, scaled by bgMoveFactor
    bgOffsetX -= (bgTargetX - bgOffsetX) * bgMoveSpeed * bgMoveFactor;
    bgOffsetY -= (bgTargetY - bgOffsetY) * bgMoveSpeed * bgMoveFactor;
}
