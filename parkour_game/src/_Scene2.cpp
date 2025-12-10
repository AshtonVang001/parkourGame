#include "_Scene2.h"
#include "gltfModel.h"
#include "_gltfLoader.h"
#include <iostream>
#include <vector>
#include <cfloat>

_Scene2::_Scene2()
{
    myTime = new _timer();
    clickCount = 0;

    myLight = nullptr;
    myInput = nullptr;
    myTexture = nullptr;
    mySkyBox = nullptr;
    myCam = nullptr;
    myCol = nullptr;
    snds = nullptr;
    myHitboxes = nullptr;
    myDeathOverlay = nullptr;
    myDeathOverlayText = nullptr;

}

_Scene2::~_Scene2()
{
    delete myTime;
    delete myLight;
    delete myInput;
    delete myTexture;
    delete mySkyBox;
    delete myCam;
    delete myCol;
    delete snds;
    delete myHitboxes;
    delete myDeathOverlay;
    delete myDeathOverlayText;
}

void _Scene2::reSizeScene(int width, int height)
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

void _Scene2::initGL()
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
    myInput  = new _inputs();
    myTexture = new _textureLoader();
    mySkyBox = new _skyBox();
    myCam    = new _camera();
    myCol    = new _collisionCheck();
    snds     = new _sounds();
    myHitboxes = new _hitboxes();
    myDeathOverlay = new _textureLoader();
    myDeathOverlayText = new _textureLoader();

    myTime->startTime = clock();


    // ---- Light ----
    myLight->setLight(GL_LIGHT0);


    // ---- Skybox ----
    mySkyBox->skyBoxInit();
    mySkyBox->tex[0] = mySkyBox->textures->loadTexture("images/back2.png");
    mySkyBox->tex[1] = mySkyBox->textures->loadTexture("images/front2.png");
    mySkyBox->tex[2] = mySkyBox->textures->loadTexture("images/top2.png");
    mySkyBox->tex[3] = mySkyBox->textures->loadTexture("images/bottom2.png");
    mySkyBox->tex[4] = mySkyBox->textures->loadTexture("images/right2.png");
    mySkyBox->tex[5] = mySkyBox->textures->loadTexture("images/left2.png");

    // ---- UI textures ----
    deathOverlayBG = myDeathOverlay->loadTexture("images/DeathBG2.png");
    deathOverlayText = myDeathOverlayText->loadTexture("images/DeathText.png");


    // ---- Camera ----
    myCam->camInit();


    // ---- Sounds ----
    snds->initSounds();
    //snds->playSound("sounds/untitled.mp3");


    // ---- Load GLTF Model ----
    myGltfModel2 = loader.loadModel("models/catSkull.glb");


    // ---- Load Model Texture ----
    GLuint texID3 = testTexture->loadTexture("images/bone2.jpg");


    // ---- NEW MODEL LOADER ----
    myNewModel.load("models/endPlatform2.glb");
    myNewModel.setActiveAnimation(0);

    orb.load("models/Orb2.glb");
    orb.setActiveAnimation(0);

    levelPlatforms.load("models/levelplatforms2.glb");
    levelPlatforms.setActiveAnimation(0);

    spinPlatform.load("models/spinPlatform2.glb");
    spinPlatform.setActiveAnimation(0);

    movePlatform.load("models/movePlatform2.glb");
    movePlatform.setActiveAnimation(0);


    gltfShader = new Shader("shaders/gltf_skin.vert", "shaders/gltf_skin.frag");


    // ---- Bind Model Texture ----
    myGltfModel2->textureID = texID3;   //skull


    // ---- Video Loader ----
    cutScene1->load("videos/cutscene2", "cutscene", 1, 85, 24.0f, VideoMode::STREAM);
    cutScene1->playOnce();

}

void _Scene2::updateScene()
{
    myTime->updateDeltaTime();

    myCam->rotateXY();
    animTime += myTime->deltaTime;

    static float smoothDT = 0.16f;
    smoothDT = (smoothDT * 0.9f) + (myTime->deltaTime * 0.1f);

    myInput->keyPressed(myCam, smoothDT);


    // ---- Death Logic ----
    if (myCam->died && !deathOverlayActive)
    {
        deathOverlayActive = true;
        deathTimer = 2.0f;
    }

    if (deathOverlayActive)
    {
        deathTimer -= myTime->deltaTime;

        float t = deathTimer / 1.0f;
        deathScale = 1.1f + (1.2f - 1.1f) * t;

        if (deathTimer <= 0)
        {
            myCam->eye = {0,0,0};
            myCam->des = {0,0,0};
            myCam->verticalVel = 0;
            myCam->isJumping = false;
            myCam->died = false;

            deathOverlayActive = false;
            deathTimer = 0;
            deathScale = 1.2f;
        }
    }


}



void _Scene2::drawScene()
{

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = (float)width / (float)height;
    gluPerspective(60.0f, aspect, 0.1f, 10000.0f);

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


    // ---- Draw GLTF Models (OLD LOADER) ----

    //animate skull up & down

    // ---- Right ----
    float skull1X = 7.5f;
    float skull1Z = -16.0f;

    float playerX = myCam->eye.x;
    float playerZ = myCam->eye.z;

    float dx1 = playerX - skull1X;
    float dz1 = playerZ - skull1Z;

    float angleY1 = atan2(dx1, dz1) * (180.0f / M_PI);

    // animate skull up/down
    time  = (float)glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    yOffset = amplitude * sin(time * speed);

    glPushMatrix();
        glTranslatef(skull1X, 7 + yOffset, skull1Z);
        glRotatef(angleY1, 0, 1, 0); // <-- makes it look at player
        glRotatef(30, 1, 0, 0);
        glScalef(2.5, 2.5, 2.5);
        glColor3f(1,1,1);
        myGltfModel2->draw();
    glPopMatrix();

    // ---- Left ----
    float skull2X = -7.5f;
    float skull2Z = -16.0f;

    float dx2 = playerX - skull2X;
    float dz2 = playerZ - skull2Z;

    float angleY2 = atan2(dx2, dz2) * (180.0f / M_PI);

    glPushMatrix();
        glTranslatef(skull2X, 7 - yOffset, skull2Z);
        glRotatef(angleY2, 0, 1, 0);
        glRotatef(30, 1, 0, 0);
        glScalef(2.5, 2.5, 2.5);
        glColor3f(1,1,1);
        myGltfModel2->draw();
    glPopMatrix();






    // ---- Shader and new Model Loader stuff ----

    GLboolean lightingEnabled = glIsEnabled(GL_LIGHTING);
    GLboolean depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
    GLboolean blendEnabled = glIsEnabled(GL_BLEND);
    GLboolean tex2DEnabled = glIsEnabled(GL_TEXTURE_2D);
    GLboolean cullFaceEnabled = glIsEnabled(GL_CULL_FACE);
    GLint currentProgram; glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
    GLint boundVAO = 0; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &boundVAO);
    GLint boundTex = 0; glGetIntegerv(GL_TEXTURE_BINDING_2D, &boundTex);
    GLboolean depthMask; glGetBooleanv(GL_DEPTH_WRITEMASK, &depthMask);
    GLint depthFunc; glGetIntegerv(GL_DEPTH_FUNC, &depthFunc);
    GLint vp[4]; glGetIntegerv(GL_VIEWPORT, vp);


    spinPlatform.update(myTime->deltaTime);
    movePlatform.update(myTime->deltaTime);
    orb.update(myTime->deltaTime);

    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0,-9,-440));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(7));
    glm::mat4 viewMatrix = myCam->getViewMatrix();
    glm::mat4 projMatrix = myCam->getProjectionMatrix((float)width / height);


    gltfShader->use();
    glUniformMatrix4fv(gltfShader->getUniform("uModel"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glUniformMatrix4fv(gltfShader->getUniform("uView"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(gltfShader->getUniform("uProj"), 1, GL_FALSE, glm::value_ptr(projMatrix));

    jointAngle += deltaTime * glm::radians(45.0f); // 45 degrees per second

    if (!levelComplete) {
        myNewModel.draw();
        levelPlatforms.draw();
        spinPlatform.draw();
        movePlatform.draw();
    }

    glm::mat4 orbModelMatrix = glm::mat4(1.0f);

    orbModelMatrix = glm::translate(orbModelMatrix, glm::vec3(0, -12, -440));
    orbModelMatrix = glm::scale(orbModelMatrix, glm::vec3(7.0f));
    glUniformMatrix4fv(gltfShader->getUniform("uModel"), 1, GL_FALSE, glm::value_ptr(orbModelMatrix));

    if (!levelComplete)
        orb.draw();


    glUseProgram(currentProgram);
    if (lightingEnabled) glEnable(GL_LIGHTING); else glDisable(GL_LIGHTING);
    if (depthTestEnabled) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
    if (blendEnabled) glEnable(GL_BLEND); else glDisable(GL_BLEND);
    if (tex2DEnabled) glEnable(GL_TEXTURE_2D); else glDisable(GL_TEXTURE_2D);
    if (cullFaceEnabled) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
    glBindVertexArray(boundVAO);
    glBindTexture(GL_TEXTURE_2D, boundTex);
    glDepthMask(depthMask);
    glDepthFunc(depthFunc);
    glViewport(vp[0], vp[1], vp[2], vp[3]);




    // ---- Video Loader ----
    if (fabs(myCam->eye.z + 440.0f) < 2 && fabs(myCam->eye.x + 0) < 2 && fabs(myCam->eye.y + 0) < 4) {
        levelComplete = true;
        //cout << "Level Complete!" << endl;
    }


    // ---- Outro Cutscene ----
    if (levelComplete)
    {
        cutScene1->update(myTime->deltaTime);

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, windowWidth, 0, windowHeight, -1, 1);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, cutScene1->getCurrentTexture());
        glColor4f(1, 1, 1, 1);

        glBegin(GL_QUADS);

            glTexCoord2f(0, 1);   // was (0,0)
            glVertex2f(0, 0);

            glTexCoord2f(1, 1);   // was (1,0)
            glVertex2f(windowWidth, 0);

            glTexCoord2f(1, 0);   // was (1,1)
            glVertex2f(windowWidth, windowHeight);

            glTexCoord2f(0, 0);   // was (0,1)
            glVertex2f(0, windowHeight);

        glEnd();


        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();

        glColor4f(1,1,1,1);
    }


    // ---- hitboxes ----
    myHitboxes->drawHitboxes();
    if (showHitBoxes)
        myHitboxes->debugBoxes();



    if (deathOverlayActive)
    {
        glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0, width, 0, height, -1, 1);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        //
        // ===========================
        //   BACKGROUND OVERLAY
        // ===========================
        //
        glBindTexture(GL_TEXTURE_2D, deathOverlayBG);

        float w = windowWidth;
        float h = windowHeight;
        float x = (width - w) * 0.5f;
        float y = (height - h) * 0.5f;

        glBegin(GL_QUADS);
            glTexCoord2f(0,1); glVertex2f(x,     y);
            glTexCoord2f(1,1); glVertex2f(x+w,   y);
            glTexCoord2f(1,0); glVertex2f(x+w,   y+h);
            glTexCoord2f(0,0); glVertex2f(x,     y+h);
        glEnd();


        float t = deathTimer / 1.0f;
        float scale = 1.1f + (1.2f - 1.1f) * t;

        glBindTexture(GL_TEXTURE_2D, deathOverlayText);

        float baseW = 1024.0f;
        float baseH = 512.0f;

        float textW = baseW * scale;
        float textH = baseH * scale;

        float textX = (width  - textW) * 0.5f;
        float textY = (height - textH) * 0.5f;

        glBegin(GL_QUADS);
            glTexCoord2f(0,1); glVertex2f(textX,       textY);
            glTexCoord2f(1,1); glVertex2f(textX+textW, textY);
            glTexCoord2f(1,0); glVertex2f(textX+textW, textY+textH);
            glTexCoord2f(0,0); glVertex2f(textX,       textY+textH);
        glEnd();


        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
    }

}

int _Scene2::winMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_KEYDOWN:
        myInput->wParam = wParam;
        if (!deathOverlayActive)
            myInput->keys[wParam] = true;
        break;

    case WM_KEYUP:
        myInput->wParam = wParam;
        myInput->keys[wParam] = false;
        break;

    case WM_MOUSEMOVE:
        if (!deathOverlayActive)
            myInput->mouseMove(myCam, LOWORD(lParam), HIWORD(lParam));
        break;
    }
    return 0;
}
