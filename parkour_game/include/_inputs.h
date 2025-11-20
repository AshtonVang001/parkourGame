#ifndef _INPUTS_H
#define _INPUTS_H
#include <_common.h>
#include <_model.h>
#include <_parallax.h>
#include <_skyBox.h>
#include <_sprite.h>
#include <_camera.h>
#include <_3DModelLoader.h>
#include <_sceneSwitcher.h>

class _inputs
{
    public:
        _inputs();
        virtual ~_inputs();

        bool keys[256] = {false};

        void keyPressed(_model *);                          //key pressed
        void keyUp();                                       //key released              (blank because not in use)

        void keyPressed(_parallax *);
        void keyPressed(_skyBox *);
        void keyPressed(_sprite *);
        void keyPressed(_camera *, float);
        void keyPressed(_3DModelLoader *,  _3DModelLoader *);

        void keyUp(_sprite *);

        void mouseEventDown(_model *, double, double);      //mouse button pressed

        void mouseEventUp();                                //mouse button released     (blank because not in use)

        void mouseWheel(_model *, double);                  //mouse wheel

        void mouseMove(_model *, double, double);           //mouse movement
        void mouseMove(_camera *, double, double);

        double prev_MouseX;                                 //keep track of mouse X-position
        double prev_MouseY;                                 //keep track of mouse Y-position

        bool isRotationActive;                              //performing rotation
        bool isTranslationActive;                           //performing translation

        WPARAM wParam;
        LPARAM lParam;

        float newFov = 60.0f;

    protected:

    private:
};

#endif // _INPUTS_H
