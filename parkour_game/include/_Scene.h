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

#endif // _SCENE_H
































/**
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

class GltfModel;

class _Scene
{
    public:
        _Scene();           //constructor
        virtual ~_Scene();  //destructor

        _light *myLight = new _light();                         //light settings
        _model *myModel = new _model();                         //create a model
        _inputs *myInput = new _inputs();                       //activate inputs
        _textureLoader *myTexture = new _textureLoader();       //for loading images
        _parallax *myPrlx = new _parallax();
        _skyBox *mySkyBox = new _skyBox();
        _sprite *mySprite = new _sprite();
        _timer *myTime = new _timer();
        _3DModelLoader *mdl3D = new _3DModelLoader();
        _3DModelLoader *mdl3DW = new _3DModelLoader();
        _camera *myCam = new _camera();
        _collisionCheck *myCol = new _collisionCheck();

        _gltfLoader loader;
        GltfModel* myGltfModel = nullptr;


        _sounds *snds = new _sounds();

        _bullets b[10];

        int clickCount = 0;

        void reSizeScene(int width, int height);                //resize window
        void initGL();                                          //initialize GL graphics
        void drawScene();                                       //render scene
        int winMsg(HWND, UINT, WPARAM, LPARAM);                 //to get keyboard interrupts & pass it to inputs
        void mouseMapping(int, int);

        double msX, msY, msZ;

        int width, height;                                      //keep record of screen size
    protected:

    private:
};

#endif // _SCENE_H
**/
