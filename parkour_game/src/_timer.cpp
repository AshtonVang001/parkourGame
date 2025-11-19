#include "_timer.h"

_timer::_timer()
{
    startTime = clock();
    lastFrameTime = startTime; // initialize lastFrameTime
    deltaTime = 0.0f;
}

_timer::~_timer() {}

clock_t _timer::getTicks()
{
    return clock() - startTime;
}

void _timer::reset()
{
    startTime = clock();
    lastFrameTime = startTime;
    deltaTime = 0.0f;
}

void _timer::pauseTime()
{

}

void _timer::resumeTime()
{

}

void _timer::updateDeltaTime()
{
    clock_t current = clock();
    deltaTime = (float)(current - lastFrameTime) / CLOCKS_PER_SEC;
    lastFrameTime = current;

    if (deltaTime < 0.00001f) deltaTime = 0.00001f;
}
