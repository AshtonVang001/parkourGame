#include "_camera.h"

_camera::_camera()
{
    //ctor
}

_camera::~_camera()
{
    //dtor
}

void _camera::camInit()
{
    eye.x = eye.y = 0;
    eye.z = 10;

    des.x = des.y = des.z = 0;

    up.x = up.z = 0;
    up.y = 1;

    step = 0.5;

    distance = sqrt(pow((eye.x-des.x),2) + pow((eye.y-des.y),2) + pow((eye.z-des.z),2));

    rotAngle.x = rotAngle.y = 0;

    verticalVel = 0.0f;
    isJumping = false;
    gravity = -40.0f;
    groundY = eye.y;
    startEye = eye;
    startDes = des;


}

void _camera::camReset()
{
    eye.x = eye.y = 0;
    eye.z = 10;

    des.x = des.y = des.z = 0;

    up.x = up.z = 0;
    up.y = 1;

    step = 0.5;

    distance = sqrt(pow((eye.x-des.x),2) + pow((eye.y-des.y),2) + pow((eye.z-des.z),2));

    rotAngle.x = rotAngle.y = 0;
}

void _camera::rotateXY()
{
    // ---- Clamp pitch ----
    if (rotAngle.y > 89.0f) rotAngle.y = 89.0f;
    if (rotAngle.y < -89.0f) rotAngle.y = -89.0f;

    float yaw   = rotAngle.x * PI / 180.0f;
    float pitch = rotAngle.y * PI / 180.0f;

    lookDir.x = cos(pitch) * sin(yaw);
    lookDir.y = sin(pitch);
    lookDir.z = cos(pitch) * cos(yaw);

    des = eye + lookDir;
}


void _camera::rotateUp()
{

}

void _camera::camMoveFdBd(float dir)
{
    // ---- Forward vector ----
    vec3 forward = des - eye;
    forward.y = 0;
    float len = sqrt(forward.x*forward.x + forward.z*forward.z);
    if (len != 0)
    {
        forward.x /= len;
        forward.z /= len;
    }

    // ---- Move W = +dir ----
    eye.x += forward.x * dir;
    eye.z += forward.z * dir;
    des.x += forward.x * dir;
    des.z += forward.z * dir;

    // Update standing pose if not jumping
    if (!isJumping) startEye = eye;
    if (!isJumping) startDes = des;
}

void _camera::camMoveLtRt(float dir)
{
    // ---- Forward vector ----
    vec3 forward = des - eye;
    forward.y = 0;
    float len = sqrt(forward.x*forward.x + forward.z*forward.z);
    if (len != 0)
    {
        forward.x /= len;
        forward.z /= len;
    }

    // ---- Right vector ----
    vec3 right;
    right.x = forward.z;
    right.y = 0;
    right.z = -forward.x;

    // ---- Move D = +dir, A = -dir ----
    eye.x += right.x * dir;
    eye.z += right.z * dir;
    des.x += right.x * dir;
    des.z += right.z * dir;

    // Update standing pose if not jumping
    if (!isJumping) startEye = eye;
    if (!isJumping) startDes = des;
}



void _camera::setUpCamera()
{
    gluLookAt(eye.x, eye.y, eye.z, des.x, des.y, des.z, up.x, up.y, up.z);
}

void _camera::jump()
{
    if (!isJumping) {
        verticalVel = 15.0f;   // jump strength
        isJumping = true;
        // store starting camera position to restore after landing
        startEye = eye;
        startDes = des;
    }
}


void _camera::updateVertical(float deltaTime)
{
    if (isJumping)
    {
        // Apply gravity
        verticalVel += gravity * deltaTime;
        float nextY = eye.y + verticalVel * deltaTime;

        // Check for landing
        if (nextY <= groundY)
        {
            nextY = groundY;
            verticalVel = 0.0f;
            isJumping = false;

            // Record landing target for smooth interpolation
            landingTargetEye.x = eye.x;
            landingTargetEye.y = nextY;
            landingTargetEye.z = eye.z;

            landingTargetDes.x = des.x;
            landingTargetDes.y = des.y;
            landingTargetDes.z = des.z;

            landingTimer = 0.0f;
            isLanding = true;
        }

        // Apply vertical movement
        eye.y = nextY;
        des = eye + lookDir;
    }
    else if (isLanding)
    {
        // Smoothly interpolate camera to landing target
        landingTimer += deltaTime;
        float t = landingTimer / landingDuration;
        if (t >= 1.0f)
        {
            eye = landingTargetEye;
            des = landingTargetDes;
            isLanding = false;
        }
        else
        {
            eye.x = lerp(eye.x, landingTargetEye.x, t);
            eye.y = lerp(eye.y, landingTargetEye.y, t);
            eye.z = lerp(eye.z, landingTargetEye.z, t);

            des.x = lerp(des.x, landingTargetDes.x, t);
            des.y = lerp(des.y, landingTargetDes.y, t);
            des.z = lerp(des.z, landingTargetDes.z, t);
        }

        // Ensure lookDir matches des - eye
        lookDir = des - eye;
    }
}

float _camera::lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}

glm::mat4 _camera::getViewMatrix() {
    return glm::lookAt(
        glm::vec3(eye.x, eye.y, eye.z),
        glm::vec3(des.x, des.y, des.z),
        glm::vec3(up.x,  up.y,  up.z)
    );
}


glm::mat4 _camera::getProjectionMatrix(float aspect) {
    return glm::perspective(glm::radians(fov), aspect, 0.1f, 1000.0f);
}

void _camera::update(float dt)
{
    //
    // 1. Apply horizontal movement first (already done in input)
    //

    //
    // 2. Apply vertical physics
    //

    // predict Y before collision
    float predictedY = eye.y;

    if (isJumping) {
        verticalVel += gravity * dt;
        predictedY += verticalVel * dt;
    }

    //
    // 3. Clamp to ground using new collision groundY
    //

    if (predictedY <= groundY) {
        predictedY = groundY;
        verticalVel = 0;
        isJumping = false;
    }

    //
    // 4. Apply the corrected Y
    //
    eye.y = predictedY;
    des = eye + lookDir;
}
