#ifndef _MAINMENU_H
#define _MAINMENU_H

#include <_common.h>
#include <_light.h>
#include <_model.h>
#include <_inputs.h>
#include <_textureLoader.h>
#include <_parallax.h>
#include <_skyBox.h>
#include <_sprite.h>
#include <_timer.h>
#include <_3DModelLoader.h>
#include <_camera.h>
#include <_bullets.h>
#include <_collisionCheck.h>
#include <_sounds.h>
#include <_gltfLoader.h>
#include <_sceneSwitcher.h>

class _mainMenu
{
public:
    _mainMenu();
    virtual ~_mainMenu();

    void initGL();
    void reSizeScene(int width, int height);
    void drawScene();
    void updateScene();
    int winMsg(HWND, UINT, WPARAM, LPARAM);
    void mouseMapping(int, int);

    double msX, msY, msZ;
    int width, height;
    int clickCount;

    float animTime = 0.0f;

    float fov = 60.0f; // default is 45.0






    float bgOffsetX = 0.0f;
    float bgOffsetY = 0.0f;

    float bgTargetX = 0.0f;
    float bgTargetY = 0.0f;

    float bgMoveSpeed = 0.005f;
    float bgScale = 1.02f;

    float bgDelayTimer = 0.0f;
    float bgDelayDuration = 1.0f;
    float bgMoveFactor = 0.0f;

    void updateBackgroundOffset(float dt, HWND hWnd, int screenWidth, int screenHeight);

    int screenWidth;
    int screenHeight;
    HWND windowHandle;

    bool helpOpen = false;




    _timer *myTime;
    _light *myLight;
    _inputs *myInput;
    _camera *myCam;
    _sounds *snds;


    //load UI textures;
    _textureLoader *menuTex = new _textureLoader();
    _textureLoader *menuUI = new _textureLoader();
    _textureLoader *helpMenuTex = new _textureLoader();
    _textureLoader *helpMenuUI = new _textureLoader();



    //load models
    _gltfLoader loader;
    GltfModel* myGltfModel;

    //load model texture
    _textureLoader *testTexture = new _textureLoader();



protected:

private:
};

#endif // _MAINMENU_H
