#include "_hitboxes.h"
#include <GL/glut.h>

_hitboxes::_hitboxes() {}
_hitboxes::~_hitboxes() {}

void _hitboxes::populateHitboxes()
{
    boxes.clear();

    // ---- Spawn Platform ----
    boxes.push_back({ {0.0f, -9.5f, 17.0f},  {20.0f, 2.75f, 20.0f} });
    boxes.push_back({ {25.0f, -9.5f, 17.0f}, {8.0f, 2.75f, 14.0f} });
    boxes.push_back({ {-25.0f, -9.5f, 17.0f}, {8.0f, 2.75f, 14.0f} });
    boxes.push_back({ {0.0f, -9.5f, -8.0f},  {14.0f, 2.75f, 8.0f} });
    boxes.push_back({ {0.0f, -9.5f, 43.0f},  {14.0f, 2.75f, 8.0f} });

    // ---- Level platforms ----
    boxes.push_back({ {0.0f, -9.5f, -59.5f}, {3.0f, 0.75f, 10.0f} });
    boxes.push_back({ {0.0f, -9.5f, -106.5f}, {3.0f, 0.75f, 10.0f} });
    boxes.push_back({ {0.0f, -9.5f, -151.5f}, {9.0f, 0.75f, 10.0f} });

    boxes.push_back({ {0.0f, -9.5f, -195.5f}, {4.5f, 0.75f, 6.0f} });
    boxes.push_back({ {0.0f, -9.5f, -239.5f}, {4.7f, 0.75f, 20.0f} });

    boxes.push_back({ {0.0f, -9.5f, -294.0f}, {9.0f, 0.75f, 10.0f} });
    boxes.push_back({ {0.0f, -6.5f, -335.5f}, {9.0f, 0.75f, 10.0f} });
    boxes.push_back({ {0.0f, -3.5f, -376.0f}, {9.0f, 0.75f, 10.0f} });

    boxes.push_back({ {20.0f, -4.5f, -426.0f}, {8.5f, 0.75f, 18.0f} });
    boxes.push_back({ {-20.0f, -4.5f, -426.0f}, {8.5f, 0.75f, 18.0f} });

    boxes.push_back({ {11.0f, -7.0f, -485.0f}, {2.0f, 2.0f, 20.0f} });
    boxes.push_back({ {-11.0f, -7.0f, -485.0f}, {2.0f, 2.0f, 20.0f} });
    boxes.push_back({ {0.0f, -6.0f, -534.0f}, {3.0f, 2.0f, 10.0f} });

    // ---- End Platform ----
    boxes.push_back({ {0.0f,  -9.5f, -595.0f}, {20.0f, 2.75f, 20.0f} });
    boxes.push_back({ {25.0f, -9.5f, -595.0f}, { 8.0f, 2.75f, 14.0f} });
    boxes.push_back({ {-25.0f,-9.5f, -595.0f}, { 8.0f, 2.75f, 14.0f} });
    boxes.push_back({ {0.0f,  -9.5f, -620.0f}, {14.0f, 2.75f,  8.0f} });
    boxes.push_back({ {0.0f,  -9.5f, -570.0f}, {14.0f, 2.75f,  8.0f} });

}

void _hitboxes::drawHitboxes()
{
    glPushAttrib(GL_CURRENT_BIT);
    for (auto& box : boxes) {
        glPushMatrix();
        glColor3f(1.0f, 1.0f, 1.0f);
        glTranslatef(box.center.x, box.center.y, box.center.z);
        glScalef(box.halfSize.x * 2, box.halfSize.y * 2, box.halfSize.z * 2);
        glutWireCube(1.0f);
        glPopMatrix();
    }
    glPopAttrib();
}




float _hitboxes::raycastY(const vec3& origin)
{
    float closestY = -10000.0f;
    for (auto& box : boxes) {
        if (origin.x >= box.center.x - box.halfSize.x && origin.x <= box.center.x + box.halfSize.x &&
            origin.z >= box.center.z - box.halfSize.z && origin.z <= box.center.z + box.halfSize.z)
        {
            float topY = box.center.y + box.halfSize.y;
            if (topY <= origin.y && topY > closestY) {
                closestY = topY;
            }
        }
    }
    return closestY;
}

void _hitboxes::debugBoxes() {

        // ---- Spawn Platform ----
        glPushAttrib(GL_CURRENT_BIT);
            glPushMatrix();
                glColor3f(1.0f, 1.0f, 1.0f);
                glTranslatef(0.0f, -9.5f, 17.0f);
                glScalef(20.0f, 2.75f, 20.0f);
                glutWireCube(2.0);
            glPopMatrix();
            glPushMatrix();
                glColor3f(1.0f, 1.0f, 1.0f);
                glTranslatef(25.0f, -9.5f, 17.0f);
                glScalef(8.0f, 2.75f, 14.0f);
                glutWireCube(2.0);
            glPopMatrix();
            glPushMatrix();
                glColor3f(1.0f, 1.0f, 1.0f);
                glTranslatef(-25.0f, -9.5f, 17.0f);
                glScalef(8.0f, 2.75f, 14.0f);
                glutWireCube(2.0);
            glPopMatrix();
            glPushMatrix();
                glColor3f(1.0f, 1.0f, 1.0f);
                glTranslatef(0.0f, -9.5f, -8.0f);
                glScalef(14.0f, 2.75f, 8.0f);
                glutWireCube(2.0);
            glPopMatrix();
            glPushMatrix();
                glColor3f(1.0f, 1.0f, 1.0f);
                glTranslatef(0.0f, -9.5f, 43.0f);
                glScalef(14.0f, 2.75f, 8.0f);
                glutWireCube(2.0);
            glPopMatrix();
        glPopAttrib();


        // ---- Level Platforms ----
        glPushAttrib(GL_CURRENT_BIT);
            glPushMatrix();
                glColor3f(0.0f, 1.0f, 0.0f);
                glTranslatef(0.0f, -9.5f, -59.5f);
                glScalef(3.0f, 0.75f, 10.0f);
                glutWireCube(2.0);
            glPopMatrix();
        glPopAttrib();

        glPushAttrib(GL_CURRENT_BIT);
            glPushMatrix();
                glColor3f(0.0f, 1.0f, 0.0f);
                glTranslatef(0.0f, -9.5f, -106.5f);
                glScalef(3.0f, 0.75f, 10.0f);
                glutWireCube(2.0);
            glPopMatrix();
        glPopAttrib();

        glPushAttrib(GL_CURRENT_BIT);
            glPushMatrix();
                glColor3f(0.0f, 1.0f, 0.0f);
                glTranslatef(0.0f, -9.5f, -151.5f);
                glScalef(9.0f, 0.75f, 10.0f);
                glutWireCube(2.0);
            glPopMatrix();
        glPopAttrib();

        glPushAttrib(GL_CURRENT_BIT);
            glPushMatrix();
                glColor3f(0.0f, 1.0f, 0.0f);
                glTranslatef(0.0f, -9.5f, -195.5f);
                glScalef(4.5f, 0.75f, 6.0f);
                glutWireCube(2.0);
            glPopMatrix();
        glPopAttrib();

        glPushAttrib(GL_CURRENT_BIT);
            glPushMatrix();
                glColor3f(0.0f, 1.0f, 0.0f);
                glTranslatef(0.0f, -9.5f, -239.5f);
                glScalef(4.7f, 0.75f, 20.0f);
                glutWireCube(2.0);
            glPopMatrix();
        glPopAttrib();


        // ---- Normal Platforms
        glPushAttrib(GL_CURRENT_BIT);
            glPushMatrix();
                glColor3f(0.0f, 1.0f, 0.0f);
                glTranslatef(0.0f, -9.5f, -294.0f);
                glScalef(9.0f, 0.75f, 10.0f);
                glutWireCube(2.0);
            glPopMatrix();
        glPopAttrib();

        glPushAttrib(GL_CURRENT_BIT);
            glPushMatrix();
                glColor3f(0.0f, 1.0f, 0.0f);
                glTranslatef(0.0f, -6.5f, -335.5f);
                glScalef(9.0f, 0.75f, 10.0f);
                glutWireCube(2.0);
            glPopMatrix();
        glPopAttrib();

        glPushAttrib(GL_CURRENT_BIT);
            glPushMatrix();
                glColor3f(0.0f, 1.0f, 0.0f);
                glTranslatef(0.0f, -3.5f, -376.0f);
                glScalef(9.0f, 0.75f, 10.0f);
                glutWireCube(2.0);
            glPopMatrix();
        glPopAttrib();


        // ---- Rotated Platforms ----
        glPushAttrib(GL_CURRENT_BIT);
            glPushMatrix();
                glColor3f(1.0f, 1.0f, 0.0f);
                glTranslatef(20.0f, -3.5f, -426.0f);
                glRotatef(35, 0, 0, 1);
                glScalef(8.5f, 0.75f, 18.0f);
                glutWireCube(2.0);
            glPopMatrix();
        glPopAttrib();
        glPushAttrib(GL_CURRENT_BIT);
            glPushMatrix();
                glColor3f(1.0f, 1.0f, 0.0f);
                glTranslatef(-20.0f, -3.5f, -426.0f);
                glRotatef(-35, 0, 0, 1);
                glScalef(8.5f, 0.75f, 18.0f);
                glutWireCube(2.0);
            glPopMatrix();
        glPopAttrib();


        // ---- Thin Platforms
        glPushAttrib(GL_CURRENT_BIT);
            glPushMatrix();
                glColor3f(0.0f, 1.0f, 0.0f);
                glTranslatef(11.0f, -7.0f, -485.0f);
                glScalef(1.0f, 2.0f, 15.0f);
                glutWireCube(2.0);
            glPopMatrix();
        glPopAttrib();
        glPushAttrib(GL_CURRENT_BIT);
            glPushMatrix();
                glColor3f(0.0f, 1.0f, 0.0f);
                glTranslatef(-11.0f, -7.0f, -485.0f);
                glScalef(1.0f, 2.0f, 15.0f);
                glutWireCube(2.0);
            glPopMatrix();
        glPopAttrib();

        glPushAttrib(GL_CURRENT_BIT);
            glPushMatrix();
                glColor3f(0.0f, 1.0f, 0.0f);
                glTranslatef(0.0f, -6.0f, -534.0f);
                glScalef(3.0f, 2.0f, 10.0f);
                glutWireCube(2.0);
            glPopMatrix();
        glPopAttrib();


        // ---- End Platform ----
        glPushAttrib(GL_CURRENT_BIT);
            glPushMatrix();
                glColor3f(1.0f, 1.0f, 1.0f);
                glTranslatef(0.0f, -9.5f, -595.0f);
                glScalef(20.0f, 2.75f, 20.0f);
                glutWireCube(2.0);
            glPopMatrix();
            glPushMatrix();
                glColor3f(1.0f, 1.0f, 1.0f);
                glTranslatef(25.0f, -9.5f, -595.0f);
                glScalef(8.0f, 2.75f, 14.0f);
                glutWireCube(2.0);
            glPopMatrix();
            glPushMatrix();
                glColor3f(1.0f, 1.0f, 1.0f);
                glTranslatef(-25.0f, -9.5f, -595.0f);
                glScalef(8.0f, 2.75f, 14.0f);
                glutWireCube(2.0);
            glPopMatrix();
            glPushMatrix();
                glColor3f(1.0f, 1.0f, 1.0f);
                glTranslatef(0.0f, -9.5f, -620.0f);
                glScalef(14.0f, 2.75f, 8.0f);
                glutWireCube(2.0);
            glPopMatrix();
            glPushMatrix();
                glColor3f(1.0f, 1.0f, 1.0f);
                glTranslatef(0.0f, -9.5f, -570.0f);
                glScalef(14.0f, 2.75f, 8.0f);
                glutWireCube(2.0);
            glPopMatrix();
        glPopAttrib();
}
