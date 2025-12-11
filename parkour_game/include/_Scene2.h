#ifndef _SCENE_2_H
#define _SCENE_2_H

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
#include <GLTFLoader.h>
#include <shader.h>
#include <_videoLoader.h>
#include <_hitboxes.h>


class _Scene2
{
public:
    _Scene2();
    virtual ~_Scene2();

    // ---- Functions ----
    void initGL();
    void reSizeScene(int width, int height);
    void drawScene();
    void updateScene();
    void mouseMapping(int, int);
    void resetScene();
    void checkCameraStep(_camera *, float);
    int winMsg(HWND, UINT, WPARAM, LPARAM);


    // ---- Variables ----
    // int
    int width, height;
    int clickCount;
    int windowWidth = GetSystemMetrics(SM_CXSCREEN);
    int windowHeight = GetSystemMetrics(SM_CYSCREEN);

    // double
    double msX, msY, msZ;

    // float
    float animTime = 0.0f;
    float fov = 60.0f; // default is 45.0
    float time; // time in seconds
    float amplitude = 0.5f; // how much it moves up and down
    float speed = 2;     // how fast it oscillates
    float yOffset;
    float levelScale = 3;
    float jointAngle = 0;

    // bool
    bool levelComplete = false;
    bool startLevel = false;

    bool showHitBoxes = false;


    // ---- Objects ----
    _timer *myTime;
    _light *myLight;
    _inputs *myInput;
    _textureLoader *myTexture;
    _skyBox *mySkyBox;
    _camera *myCam;
    _collisionCheck *myCol;
    _sounds *snds;
    _sceneSwitcher *sceneSwitcher = new _sceneSwitcher();
    _hitboxes *myHitboxes;

    bool deathOverlayActive = false;
    float deathTimer = 0.0f;
    float deathScale = 1.4f;
    _textureLoader *myDeathOverlay;
    _textureLoader *myDeathOverlayText;
    GLuint deathOverlayBG;
    GLuint deathOverlayText;

    VideoLoader *testVideo = new VideoLoader();
    VideoLoader *cutScene1 = new VideoLoader();
    VideoLoader *introCutscene = new VideoLoader();

    GLTFModel myNewModel;
    GLTFModel orb;
    GLTFModel levelPlatforms;
    GLTFModel rings;
    GLTFModel movePlatform;

    std::vector<GLTFModel*> platforms = {
    &levelPlatforms,
    };

    Shader* gltfShader = nullptr;



    // ---- load models (OLD) ----
    _gltfLoader loader;
    GltfModel* myGltfModel2;


    // ---- load model texture ----
    _textureLoader *testTexture = new _textureLoader();


protected:

private:
};

#endif // _SCENE_2_H
