#ifndef _SCENESWITCHER_H
#define _SCENESWITCHER_H

#include <_common.h>


enum SceneID {
    SCENE_MENU,
    SCENE_GAME,
    SCENE_LEVEL_TWO,
    SCENE_LEVEL_THREE,
    PAUSE_MENU
};


class _sceneSwitcher
{
    public:
        _sceneSwitcher();
        virtual ~_sceneSwitcher();

        SceneID currentScene = SCENE_MENU;

    protected:

    private:
};

#endif // _SCENESWITCHER_H
