#include "_Scene.h"
#include "gltfModel.h"
#include "_gltfLoader.h"
#include <iostream>

_Scene::_Scene()
{
    myTime = new _timer();
    clickCount = 0;

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

_Scene::~_Scene()
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

void _Scene::reSizeScene(int width, int height)
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

void _Scene::initGL()
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

    // ---- Load Textures ----
    myTexture->loadTexture("images/tex4.jpg");
    myPrlx->parallaxInit("images/prlx.jpg");

    // ---- Skybox ----
    mySkyBox->skyBoxInit();
    mySkyBox->tex[0] = mySkyBox->textures->loadTexture("images/back.png");
    mySkyBox->tex[1] = mySkyBox->textures->loadTexture("images/front.png");
    mySkyBox->tex[2] = mySkyBox->textures->loadTexture("images/top.png");
    mySkyBox->tex[3] = mySkyBox->textures->loadTexture("images/bottom.png");
    mySkyBox->tex[4] = mySkyBox->textures->loadTexture("images/right.png");
    mySkyBox->tex[5] = mySkyBox->textures->loadTexture("images/left.png");

    // ---- Sprite ----
    mySprite->spriteInit("images/eg-1.png", 6, 4);

    // ---- MD2 Models ----
    mdl3D->initModel("models/Tekk/tris.md2");
    mdl3DW->initModel("models/Tekk/weapon.md2");

    // ---- Camera ----
    myCam->camInit();

    // ---- Sounds ----
    snds->initSounds();
    snds->playSound("sounds/untitled.mp3");

    // ---- Load GLTF Model ----
    myGltfModel = loader.loadModel("models/monkE1.glb");
    myGltfModel2 = loader.loadModel("models/cuberotate.glb");
    ground = loader.loadModel("models/ground.glb");

    // ---- Load Model Texture ----
    GLuint texID = testTexture->loadTexture("images/test_texture.jpg");
    GLuint texID2 = testTexture->loadTexture("images/red.png");

    // ---- Bind Model Texture ----
    myGltfModel->textureID = texID;
    myGltfModel2->textureID = texID2;
    ground->textureID = texID;

    if (!myGltfModel) {
        std::cerr << "GLTF: Failed to load model\n";
    }
    else if (myGltfModel->vertices.empty() || myGltfModel->indices.empty()) {
        std::cerr << "GLTF: File parsed but contains no mesh\n";
    }
    else {
        std::cout << "GLTF: Uploading to GPU...\n";
        myGltfModel->uploadToGPU();
        std::cout << "GLTF: Ready\n";
    }

    if (myGltfModel2) {
        std::cout << "GLTF2 verts: " << myGltfModel2->vertices.size()
                << ", indices: " << myGltfModel2->indices.size()
                << ", textureID: " << myGltfModel2->textureID << "\n";
    }
}



void _Scene::updateScene()
{
    myTime->updateDeltaTime();

    static float smoothDT = 0.16f;
    smoothDT = (smoothDT * 0.9f) + (myTime->deltaTime * 0.1f);

    //myCam->updateVertical(myTime->deltaTime);     ----OLD----
    myCam->rotateXY();

    animTime += myTime->deltaTime;

    if (myInput && myCam) {
        myInput->keyPressed(myCam, smoothDT);
        //myCam->update(smoothDT, myCol, ground);       ----OLD----
    }
}


void _Scene::drawScene()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = (float)width / (float)height;
    gluPerspective(fov, aspect, 0.1f, 1000.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glDisable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);





    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // ---- Camera ----
    myCam->setUpCamera();

    // ---- Background parallax ----
    glPushMatrix();
        glScalef(4.33f, 4.33f, 1.0f);
        myPrlx->prlxScrollAuto("left", 0.001f);
        mySkyBox->drawSkyBox();
    glPopMatrix();

    // ---- Sprite animation ----
    glPushMatrix();
        if (myTime->getTicks() > 70) {
            mySprite->spriteActions();
            myTime->reset();
        }
    glPopMatrix();

    // ---- 3D MD2 Character ----
    glPushMatrix();
        glRotatef(90, 1, 0, 0);
        glRotatef(180, 0, 1, 0);
        glScalef(0.1f, 0.1f, 0.1f);

        mdl3D->Actions();
        //mdl3D->Draw();
        //mdl3DW->Draw();
    glPopMatrix();

    // ---- Bullets ----
    glPushMatrix();
    for (int i = 0; i < 10; i++) {
        if (b[i].isAlive) {
            b[i].drawBullet();
            b[i].bulletActions();
        }
    }
    glPopMatrix();

    // ---- Draw GLTF Models ----
    if (myGltfModel) {
        glPushMatrix();
            glTranslatef(0, 0, -20);
            glScalef(1, 1, 1); // Adjust size
            glColor3f(1,1,1);

            if (myGltfModel->textureID != 0) {
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, myGltfModel->textureID);
            }

            myGltfModel->draw();

            if (myGltfModel->textureID != 0) glBindTexture(GL_TEXTURE_2D, 0);
        glPopMatrix();
    }


    if (myGltfModel2) {
    glPushMatrix();
        glTranslatef(5, 0, -20);   // pos
        glScalef(1, 1, 1);         // scale
        glColor3f(1, 1, 1);

        // ---- Bind texture ----
        if (myGltfModel2->textureID != 0) {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, myGltfModel2->textureID);
        }

        // ---- Update animation every frame ----
        if (myGltfModel2->data && myGltfModel2->data->animations_count > 0) {
            myGltfModel2->updateAnimation(animTime);
        }

        // ---- Draw all nodes recursively ----
        myGltfModel2->draw();

        // ---- Unbind texture ----
        if (myGltfModel2->textureID != 0) glBindTexture(GL_TEXTURE_2D, 0);
            glPopMatrix();
    }

    glPushMatrix();
        glTranslatef(0, -3, 0);
        glScalef(1, 1, 1);
        glColor3f(1,1,1);
        ground->draw();
    glPopMatrix();
}

int _Scene::winMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_KEYDOWN:
        myInput->wParam = wParam;
        //myInput->keyPressed(mySkyBox);        ----OLD----
        //myInput->keyPressed(myCam);           ----OLD----
        myInput->keys[wParam] = true;
        break;

    case WM_KEYUP:
        myInput->wParam = wParam;
        myInput->keys[wParam] = false;
        break;

    case WM_LBUTTONDOWN:
        //myInput->mouseEventDown(model, LOWORD(lParam), HIWORD(lParam));           ----OLD----
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
