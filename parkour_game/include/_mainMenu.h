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



    // Current offset of the background
    float bgOffsetX = 0.0f;
    float bgOffsetY = 0.0f;

    // Target offset (where it wants to move based on mouse)
    float bgTargetX = 0.0f;
    float bgTargetY = 0.0f;

    // Speed of movement (0 < speed <= 1)
    float bgMoveSpeed = 0.005f;

    // Image scale relative to screen (1.0 = fits exactly)
    float bgScale = 1.02f; // 10% larger than screen

    float bgDelayTimer = 0.0f;
    float bgDelayDuration = 1.0f;
    float bgMoveFactor = 0.0f;

    void updateBackgroundOffset(float dt, HWND hWnd, int screenWidth, int screenHeight);

    int screenWidth;
    int screenHeight;
    HWND windowHandle;



    _textureLoader *menuTex = new _textureLoader();
    _textureLoader *menuUI = new _textureLoader();


    _timer *myTime;

    _light *myLight;
    _model *myModel;
    _inputs *myInput;
    _textureLoader *myTexture;
    _parallax *myPrlx;
    _skyBox *mySkyBox;
    _sprite *mySprite;
    _3DModelLoader *mdl3D;
    _3DModelLoader *mdl3DW;
    _camera *myCam;
    _collisionCheck *myCol;
    _sounds *snds;

    _bullets b[10];

    //load models
    _gltfLoader loader;
    GltfModel* myGltfModel;
    GltfModel* myGltfModel2;
    GltfModel* ground;

    //load model texture
    _textureLoader *testTexture = new _textureLoader();
    _textureLoader *testTexture2 = new _textureLoader();
    _textureLoader *groundTexture = new _textureLoader();


protected:

private:
};

#endif // _MAINMENU_H
