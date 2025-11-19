#ifndef _CAMERA_H
#define _CAMERA_H

#include <_common.h>
#include <_collisionCheck.h>
#include <_gltfLoader.h>
#include <gltfModel.h>

class gltfModel;
class _collisionCheck;

class _camera
{
    public:
        _camera();
        virtual ~_camera();

        vec3 eye;                   //eye position
        vec3 des;                   //where you are looking at
        vec3 up;                    //camera head orientation

        float step;                 //camera speed
        vec3 rotAngle;              //rotation of camera
        float distance;             //eye to source

        enum {FORWARD, BACKWARD, LEFT, RIGHT};

        void camInit();             //initialize camera
        void camReset();            //reset camera
        void update(float deltaTime, _collisionCheck* colCheck, gltfModel* groundMesh);

        void rotateXY();            //rotate XY direction
        void rotateUp();            //rotate up & down

        void camMoveFdBd(float dir);  //move camera forward & backward
        void camMoveLtRt(float dir);  //move camera left & right

        void setUpCamera();         //set camera using glulookat



        // In _camera.h
        void jump();
        void updateVertical(float);
        float lerp(float, float, float);

        float verticalVel;   // vertical velocity for jump
        bool isJumping;      // true while in the air
        float gravity;       // e.g., -9.8 units/sec^2
        float groundY;       // the Y position of the floor

        float targetY;
        vec3 lookDir;
        // store camera position at jump start so we can restore after landing
        vec3 startEye;
        vec3 startDes;
        // landing interpolation state
        bool isLanding = false;
        float landingTimer = 0.0f;
        float landingDuration = 0.0f;




    protected:

    private:
};

#endif // _CAMERA_H
