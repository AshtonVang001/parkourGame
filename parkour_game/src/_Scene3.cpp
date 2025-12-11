#include "_Scene3.h"
#include "gltfModel.h"
#include "_gltfLoader.h"
#include <iostream>
#include <vector>
#include <cfloat>

_Scene3::_Scene3()
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

_Scene3::~_Scene3()
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

void _Scene3::reSizeScene(int width, int height)
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

void _Scene3::initGL()
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
    mySkyBox->tex[0] = mySkyBox->textures->loadTexture("images/back3.png");
    mySkyBox->tex[1] = mySkyBox->textures->loadTexture("images/front3.png");
    mySkyBox->tex[2] = mySkyBox->textures->loadTexture("images/top3.png");
    mySkyBox->tex[3] = mySkyBox->textures->loadTexture("images/bottom3.png");
    mySkyBox->tex[4] = mySkyBox->textures->loadTexture("images/right3.png");
    mySkyBox->tex[5] = mySkyBox->textures->loadTexture("images/left3.png");

    // ---- UI textures ----
    deathOverlayBG = myDeathOverlay->loadTexture("images/DeathBG.png");
    deathOverlayText = myDeathOverlayText->loadTexture("images/DeathText.png");


    // ---- Camera ----
    myCam->camInit();


    // ---- Sounds ----
    snds->initSounds();
    //snds->playSound("sounds/untitled.mp3");


    // ---- Load GLTF Model ----
    myGltfModel2 = loader.loadModel("models/catSkull.glb");
    myGltfModel  = loader.loadModel("models/catSkull.glb");


    // ---- Load Model Texture ----
    GLuint texID3 = testTexture->loadTexture("images/bone2.jpg");
    GLuint texID2 = testTexture->loadTexture("images/dark.png");


    // ---- NEW MODEL LOADER ----
    myNewModel.load("models/endPlatform3.glb");
    myNewModel.setActiveAnimation(0);

    orb.load("models/Orb.glb");
    orb.setActiveAnimation(0);

    levelPlatforms.load("models/newLevelplatforms3.glb");
    levelPlatforms.setActiveAnimation(0);

    spikes.load("models/spikes.glb");
    spikes.setActiveAnimation(0);



    gltfShader = new Shader("shaders/gltf_skin.vert", "shaders/gltf_skin.frag");


    // ---- Bind Model Texture ----
    myGltfModel2->textureID = texID3;   //skull
    myGltfModel->textureID  = texID2;   //skull


    // ---- Video Loader ----
    cutScene1->load("videos/cutscene3", "cutscene", 1, 175, 24.0f, VideoMode::STREAM);
    cutScene1->playOnce();

    introCutscene->load("videos/finalIntro", "finalIntro", 1, 100, 24.0f, VideoMode::STREAM);
    introCutscene->playOnce();

    winTexture = myTexture->loadTexture("images/winScreen.png");

}

void _Scene3::updateScene()
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



    // ---- Lasers ----

    float px = myCam->eye.x;
    float py = myCam->eye.y;
    float pz = myCam->eye.z;

    laserTimer += myTime->deltaTime;

    if (introCutscene->finished && !myCam->died) {
        if (laserTimer >= laserCooldown)
        {
            shootLaser();
            laserTimer = 0.0f;   // reset
        }
    }

    for (auto& L : lasers)
    {
        if (!L.alive) continue;

        // Move the laser forward
        L.x += L.dx * L.speed * myTime->deltaTime;
        L.y += L.dy * L.speed * myTime->deltaTime;
        L.z += L.dz * L.speed * myTime->deltaTime;

        float pull = -L.x * L.curveStrength * myTime->deltaTime;
        L.x += pull;

        // --- Simple Collision Check ---
        float dx = L.x - px;
        float dy = L.y - py;
        float dz = L.z - pz;
        float dist2 = dx*dx + dy*dy + dz*dz;

        float hitRange = 1.5f;
        if (dist2 < hitRange * hitRange)
        {
            L.alive = false;
            myCam->died = true;
            break;
        }
    }


}



void _Scene3::drawScene()
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
        glRotatef(angleY1, 0, 1, 0);
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


    if (!levelComplete) {
        glPushMatrix();
            glTranslatef(0, 9 - yOffset * 2, -595);
            glRotatef(0, 0, 1, 0);
            glRotatef(30, 1, 0, 0);
            glScalef(7, 7, 7);
            glColor3f(1,1,1);
            myGltfModel->draw();
        glPopMatrix();
    }




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


    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0,-9,-595));
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
        spikes.draw();
    }


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



    // ---- Lasers ----
    for (auto& L : lasers)
    {
        if (!L.alive && !levelComplete) continue;
        glPushAttrib(GL_CURRENT_BIT);
        glPushMatrix();
        glTranslatef(L.x, L.y, L.z);

        float yaw   = atan2(L.dx, L.dz) * 180.0f / M_PI;
        float pitch = -atan2(L.dy, sqrt(L.dx*L.dx + L.dz*L.dz)) * 180.0f / M_PI;

        glRotatef(yaw,   0, 1, 0);
        glRotatef(pitch, 1, 0, 0);

        glScalef(0.2f, 0.2f, 2.0f);
        glColor3f(1, 1, 1);
        glutSolidCube(2);  // placeholder for a laser
        glPopMatrix();
        glPopAttrib();
    }


    if (deathOverlayActive && introCutscene->finished)
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





    // ---- Video Loader ----
    if (!myCam->died && fabs(myCam->eye.z + 595.0f) < 10 && fabs(myCam->eye.x + 0) < 10 && fabs(myCam->eye.y + 0) < 10) {
        levelComplete = true;
        //cout << "Level Complete!" << endl;
    }


    // ---- Intro Cutscene ----
    if (startLevel && !introCutscene->finished)
    {
        introCutscene->update(myTime->deltaTime);

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0, windowWidth, 0, windowHeight, -1, 1);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, introCutscene->getCurrentTexture());
        glColor4f(1,1,1,1);

        glBegin(GL_QUADS);
            glTexCoord2f(0, 1); glVertex2f(0, 0);
            glTexCoord2f(1, 1); glVertex2f(windowWidth, 0);
            glTexCoord2f(1, 0); glVertex2f(windowWidth, windowHeight);
            glTexCoord2f(0, 0); glVertex2f(0, windowHeight);
        glEnd();

        glDisable(GL_TEXTURE_2D);

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();

        glColor4f(1,1,1,1);
    }


    // ---- Outro Cutscene ----
    if (levelComplete && !cutScene1->finished)
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


    // ---- Show Win Screen ----
    if (cutScene1->finished && !winShown)
    {
        glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0, windowWidth, 0, windowHeight, -1, 1);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, winTexture);
        glColor4f(1, 1, 1, 1);

        glBegin(GL_QUADS);
            glTexCoord2f(0, 1); glVertex2f(0, 0);
            glTexCoord2f(1, 1); glVertex2f(windowWidth, 0);
            glTexCoord2f(1, 0); glVertex2f(windowWidth, windowHeight);
            glTexCoord2f(0, 0); glVertex2f(0, windowHeight);
        glEnd();

        glDisable(GL_TEXTURE_2D);

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
    }

}

int _Scene3::winMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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


void _Scene3::shootLaser() {
    spawnLaserOffset(+3.0f, -2.5f);  // Right laser
    spawnLaserOffset(-3.0f, -2.5f);  // Left laser
}

void _Scene3::spawnLaserOffset(float xOff, float yOff)
{
    Laser L;

    L.x = xOff;
    L.y = (9 - yOffset * 2) + yOff;
    L.z = -595;

    // Target: the player
    float px = myCam->eye.x;
    float py = myCam->eye.y;
    float pz = myCam->eye.z;

    // --- Compute direction normally ---
    L.dx = px - L.x;
    L.dy = py - L.y;
    L.dz = pz - L.z;

    float len = sqrt(L.dx*L.dx + L.dy*L.dy + L.dz*L.dz);
    L.dx /= len;
    L.dy /= len;
    L.dz /= len;

    L.speed = 350.0f;
    L.alive = true;

    L.curveStrength = 0.5f; // NEW (see below)

    lasers.push_back(L);
}

