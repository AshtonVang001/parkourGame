#include "_Scene.h"
#include "gltfModel.h"
#include "_gltfLoader.h"
#include <iostream>
#include <vector>
#include <cfloat>

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
    platform1 = nullptr;
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
    delete platform1;
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

    // ---- Bind Model Texture
    myGltfModel->textureID = texID;
    myGltfModel2->textureID = texID2;
    ground->textureID = texID;
    ground->buildTriangleList();

    // ---- Extra platform (reuse ground model as simple platform instance)
    platform1 = loader.loadModel("models/ground.glb");

    if (platform1) {
        // Use ground/test texture instead of the red texture so platform matches scene
        platform1->textureID = texID;
        platform1->buildTriangleList();
        platform1->uploadToGPU();
    }

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


/*
void _Scene::updateScene()
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
}
*/


void _Scene::updateScene()
{
    myTime->updateDeltaTime();

    myCam->rotateXY();

    animTime += myTime->deltaTime;

    animTime += myTime->deltaTime;

    // Raycast down from camera to detect ground height
    vec3 rayStart = myCam->eye;
    vec3 rayDir   = {0, -1, 0};

    static float smoothDT = 0.16f;
    smoothDT = (smoothDT * 0.9f) + (myTime->deltaTime * 0.1f);


    float t;
    vec3 hitPos;

    // Check ground and additional platforms; transform triangles to world space first
    float bestT = FLT_MAX;
    vec3 bestHit = {0,0,0};
    bool anyHit = false;

    // Helper lambda to transform triangles by scale and translate and test raycast
    auto testTransformed = [&](const std::vector<Triangle>& srcTris,
                               float sx, float sy, float sz,
                               float tx, float ty, float tz) {
        if (srcTris.empty()) return;

        std::vector<Triangle> temp;
        temp.reserve(srcTris.size());

        for (const Triangle& tri : srcTris) {
            Triangle ttri;
            ttri.a.x = tri.a.x * sx + tx; ttri.a.y = tri.a.y * sy + ty; ttri.a.z = tri.a.z * sz + tz;
            ttri.b.x = tri.b.x * sx + tx; ttri.b.y = tri.b.y * sy + ty; ttri.b.z = tri.b.z * sz + tz;
            ttri.c.x = tri.c.x * sx + tx; ttri.c.y = tri.c.y * sy + ty; ttri.c.z = tri.c.z * sz + tz;
            temp.push_back(ttri);
        }

        float localT; vec3 localHit;
        if (myCol->raycastMeshNearest(rayStart, rayDir, temp, localT, localHit)) {
            if (localT < bestT) { bestT = localT; bestHit = localHit; anyHit = true; }
        }
    };

    // Ground transform (draw uses translate(0, -3, 0) and scale(1,1,1))
    if (ground) testTransformed(ground->triangles, 1.0f, 1.0f, 1.0f, 0.0f, -3.0f, 0.0f);

    // platform1: translate(-8.0f, -3.0f, -8.0f); smaller scale to reduce footprint
    if (platform1) testTransformed(platform1->triangles, 1.0f, 0.3f, 0.5f, -8.0f, -3.0f, -8.0f);

    if (anyHit)
        myCam->groundY = bestHit.y;
    else
        myCam->groundY = -9999;

        if (myInput && myCam) {
        myInput->keyPressed(myCam, smoothDT);
        //myCam->update(smoothDT, myCol, ground);
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

    // Make sure lighting/blend state is what the game expects
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glDisable(GL_BLEND); // optional: enable later where needed
    glEnable(GL_TEXTURE_2D);





    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Camera
    myCam->setUpCamera();

    // Background parallax
    glPushMatrix();
        glScalef(4.33f, 4.33f, 1.0f);
        myPrlx->prlxScrollAuto("left", 0.001f);
        mySkyBox->drawSkyBox();
    glPopMatrix();

    // Sprite animation
    glPushMatrix();
        if (myTime->getTicks() > 70) {
            mySprite->spriteActions();
            myTime->reset();
        }
    glPopMatrix();

    // 3D MD2 Character
    glPushMatrix();
        glRotatef(90, 1, 0, 0);
        glRotatef(180, 0, 1, 0);
        glScalef(0.1f, 0.1f, 0.1f);

        mdl3D->Actions();
        //mdl3D->Draw();
        //mdl3DW->Draw();
    glPopMatrix();

    // Bullets
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
            glScalef(1, 1, 1); // Adjust size as needed
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
        glTranslatef(5, 0, -20);   // position
        glScalef(1, 1, 1);         // scale
        glColor3f(1, 1, 1);

        // Bind texture if available
        if (myGltfModel2->textureID != 0) {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, myGltfModel2->textureID);
        }

        // Update animation every frame
        if (myGltfModel2->data && myGltfModel2->data->animations_count > 0) {
            myGltfModel2->updateAnimation(animTime);
        }

        // Draw all nodes recursively using global transforms
        myGltfModel2->draw();

        // Unbind texture
        if (myGltfModel2->textureID != 0) glBindTexture(GL_TEXTURE_2D, 0);
            glPopMatrix();
    }





    /**
    glPushMatrix();
    glTranslatef(0,0,-5);
    glRotatef(animTime * 50.0f, 0,1,0); // spin around Y
    if (myGltfModel2->textureID != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, myGltfModel2->textureID);
    }
    myGltfModel2->draw();
    glPopMatrix();
    **/




    // Draw extra platforms
    if (platform1) {
        glPushMatrix();
            // Left platform (moved further left and forward) - smaller footprint
            glTranslatef(-8.0f, -3.0f, -8.0f);
            glScalef(1.0f, 0.3f, 0.5f);
            glColor3f(1,1,1);
            if (platform1->textureID != 0) { glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D, platform1->textureID); }
            platform1->draw();
            if (platform1->textureID != 0) glBindTexture(GL_TEXTURE_2D, 0);
        glPopMatrix();
    }

    // (Only one platform is used now)

    // Draw ground
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
