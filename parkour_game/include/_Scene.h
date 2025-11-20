#ifndef _SCENE_H
#define _SCENE_H

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

class _Scene
{
public:
    _Scene();
    virtual ~_Scene();

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


    float time; // time in seconds
    float amplitude = 0.5f; // how much it moves up and down
    float speed = 2;     // how fast it oscillates
    float yOffset;

    float levelScale = 3;



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
    _sceneSwitcher *sceneSwitcher = new _sceneSwitcher();

    _bullets b[10];

    // ---- load models ----
    _gltfLoader loader;
    GltfModel* myGltfModel;
    GltfModel* myGltfModel2;
    GltfModel* ground;
    GltfModel* pedestalBase;
    GltfModel* pedestal;
    GltfModel* platform1;

    // ---- load model texture ----
    _textureLoader *testTexture = new _textureLoader();
    _textureLoader *testTexture2 = new _textureLoader();
    _textureLoader *groundTexture = new _textureLoader();


protected:

private:
};

#endif // _SCENE_H
