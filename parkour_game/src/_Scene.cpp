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
    myGltfModel = loader.loadModel("models/monkE3.glb");
    myGltfModel2 = loader.loadModel("models/catSkull.glb");
    ground = loader.loadModel("models/levelFloor.glb");
    pedestalBase = loader.loadModel("models/levelPedestalBase.glb");
    pedestal = loader.loadModel("models/levelPedestal.glb");

    // ---- Load Model Texture ----
    GLuint texID = testTexture->loadTexture("images/test_texture.jpg");
    GLuint texID2 = testTexture->loadTexture("images/pedestal.jpg");
    GLuint texID3 = testTexture->loadTexture("images/bone2.jpg");


    // ---- Extra platform (reuse ground model as simple platform instance)
    platform1 = loader.loadModel("models/ground.glb");

    if (platform1) {
        // Use ground/test texture instead of the red texture so platform matches scene
        platform1->textureID = texID;
        platform1->buildTriangleList();
        platform1->uploadToGPU();
    }
    // ---- Bind Model Texture ----
    myGltfModel->textureID = texID;     //monke
    myGltfModel2->textureID = texID3;   //skull
    ground->textureID = texID2;
    pedestalBase->textureID = texID2;
    pedestal->textureID = texID2;

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

    myCam->rotateXY();

    animTime += myTime->deltaTime;

    if (myInput && myCam) {
        myInput->keyPressed(myCam, smoothDT);
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

    // Helper lambda to transform a model's triangles by its node root transform
    // plus the scene translate/scale, and test raycast against the transformed tris.
    auto testTransformed = [&](const GltfModel* model,
                               float sx, float sy, float sz,
                               float tx, float ty, float tz) {
        if (!model) return;
        const auto& srcTris = model->triangles;
        if (srcTris.empty()) return;

        // Build combined matrix: Mouter = Translate(tx,ty,tz) * Scale(sx,sy,sz)
        glm::mat4 Mouter = glm::translate(glm::mat4(1.0f), glm::vec3(tx, ty, tz))
                         * glm::scale(glm::mat4(1.0f), glm::vec3(sx, sy, sz));

        // model root transform (if present)
        glm::mat4 Mnode = glm::mat4(1.0f);
        if (!model->nodeGlobalTransforms.empty()) {
            Mnode = model->nodeGlobalTransforms[0];
        }

        glm::mat4 M = Mouter * Mnode; // final transform applied to model-space vertices

        std::vector<Triangle> temp;
        temp.reserve(srcTris.size());

        for (const Triangle& tri : srcTris) {
            Triangle ttri;
            glm::vec4 a = M * glm::vec4(tri.a.x, tri.a.y, tri.a.z, 1.0f);
            glm::vec4 b = M * glm::vec4(tri.b.x, tri.b.y, tri.b.z, 1.0f);
            glm::vec4 c = M * glm::vec4(tri.c.x, tri.c.y, tri.c.z, 1.0f);
            ttri.a = { a.x, a.y, a.z };
            ttri.b = { b.x, b.y, b.z };
            ttri.c = { c.x, c.y, c.z };
            temp.push_back(ttri);
        }

        float localT; vec3 localHit;
        if (myCol->raycastMeshNearest(rayStart, rayDir, temp, localT, localHit)) {
            if (localT < bestT) { bestT = localT; bestHit = localHit; anyHit = true; }
        }
    };

    // Do not include ground in collision tests; only platform1 is used for collisions
    // (If you want the ground back later, re-enable building its triangle list.)

    // platform1: translate(-8.0f, -3.0f, -8.0f); smaller scale to reduce footprint
    if (platform1) testTransformed(platform1, 1.0f, 0.3f, 0.5f, -8.0f, -3.0f, -8.0f);

    if (anyHit) {
        // Add a small tolerance so the camera doesn't sink slightly below the platform
        const float collisionEpsilon = 0.1f; // adjust as needed
        myCam->groundY = bestHit.y + collisionEpsilon;
    }
    else
        myCam->groundY = -9999;

        if (myInput && myCam) {
        // Save previous camera position so we can test the swept segment
        vec3 prevEye = myCam->eye;

        // Let input try to move the camera
        myInput->keyPressed(myCam, smoothDT);

        // New camera position after input
        vec3 newEye = myCam->eye;

        // Compute movement vector
        glm::vec3 prevG(prevEye.x, prevEye.y, prevEye.z);
        glm::vec3 newG(newEye.x, newEye.y, newEye.z);
        glm::vec3 move = newG - prevG;
        float moveLen = glm::length(move);

        const float moveEps = 1e-6f;
        const float clampEps = 0.1f; // how far from the hit point the camera should stop

        if (moveLen > moveEps && platform1 && myCol) {
            glm::vec3 dir = move / moveLen; // normalized movement direction

            // Build transformed triangle list for platform1 (same transform used in draw())
            std::vector<Triangle> temp;
            const auto& srcTris = platform1->triangles;
            temp.reserve(srcTris.size());

            // Outer transform applied in draw: Translate(-8,-3,-8) then Scale(1,0.3,0.5)
            glm::mat4 Mouter = glm::translate(glm::mat4(1.0f), glm::vec3(-8.0f, -3.0f, -8.0f))
                             * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 0.3f, 0.5f));

            // model node transform if present
            glm::mat4 Mnode = glm::mat4(1.0f);
            if (!platform1->nodeGlobalTransforms.empty()) Mnode = platform1->nodeGlobalTransforms[0];

            glm::mat4 M = Mouter * Mnode;

            for (const Triangle& tri : srcTris) {
                Triangle ttri;
                glm::vec4 a = M * glm::vec4(tri.a.x, tri.a.y, tri.a.z, 1.0f);
                glm::vec4 b = M * glm::vec4(tri.b.x, tri.b.y, tri.b.z, 1.0f);
                glm::vec4 c = M * glm::vec4(tri.c.x, tri.c.y, tri.c.z, 1.0f);
                ttri.a = { a.x, a.y, a.z };
                ttri.b = { b.x, b.y, b.z };
                ttri.c = { c.x, c.y, c.z };
                temp.push_back(ttri);
            }

            // Convert to project vec3 and raycast from previous position along movement dir
            vec3 rayStart = prevEye;
            vec3 rayDir = { dir.x, dir.y, dir.z };

            float hitT = 0.0f;
            vec3 hitPos = {0,0,0};

            if ( myCol->raycastMeshNearest(rayStart, rayDir, temp, hitT, hitPos) ) {
                // If the hit occurs within our movement length, clamp camera to just before hit
                if (hitT <= moveLen + 1e-4f) {
                    myCam->eye.x = hitPos.x - dir.x * clampEps;
                    myCam->eye.y = hitPos.y - dir.y * clampEps;
                    myCam->eye.z = hitPos.z - dir.z * clampEps;

                    // keep look direction consistent
                    myCam->des.x = myCam->eye.x + myCam->lookDir.x;
                    myCam->des.y = myCam->eye.y + myCam->lookDir.y;
                    myCam->des.z = myCam->eye.z + myCam->lookDir.z;
                }
            }
        }

        //myCam->update(smoothDT, myCol, ground);
    }
}



void _Scene::drawScene()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = (float)width / (float)height;
    gluPerspective(fov, aspect, 0.1f, 10000.0f);

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


    // ---- Skybox ----
    glPushMatrix();
        glScalef(4.33f, 4.33f, 1.0f);
        mySkyBox->drawSkyBox();
    glPopMatrix();


    // ---- Draw GLTF Models ----
    glPushMatrix();

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

    // Ground drawing disabled — only `platform1` should be visible as the single platform
        glTranslatef(0, 0, -20);
        glScalef(1, 1, 1);
        glColor3f(1,1,1);
        //myGltfModel->draw();
    glPopMatrix();

    //animate skull up & down
    time = (float)glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    yOffset = amplitude * sin(time * speed);

    //skulls
    glPushMatrix();
        glTranslatef(4.5, 7 + yOffset, -16);
        glScalef(1.6, 1.6, 1.6);
        glColor3f(1,1,1);
        glRotatef(-20, 0, 1, 0);
        glRotatef(30, 1, 0, 0);
        myGltfModel2->draw();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(-4.5, 7 - yOffset, -16);
        glScalef(1.6, 1.6, 1.6);
        glColor3f(1,1,1);
        glRotatef(20, 0, 1, 0);
        glRotatef(30, 1, 0, 0);
        myGltfModel2->draw();
    glPopMatrix();


    //level
    glPushMatrix();
        glTranslatef(0, -4, 0);
        glScalef(levelScale, levelScale, levelScale);
        glColor3f(1,1,1);
        glRotatef(180, 0, 1, 0);
        ground->draw();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0, -4, 0);
        glScalef(levelScale, levelScale, levelScale);
        glColor3f(1,1,1);
        pedestalBase->draw();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0, -4, 0);
        glScalef(levelScale, levelScale, levelScale);
        glColor3f(1,1,1);
        pedestal->draw();
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
