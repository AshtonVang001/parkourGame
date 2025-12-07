#ifndef _VIDEO_LOADER_H
#define _VIDEO_LOADER_H

#include <vector>
#include <string>
#include <gl/glew.h>
#include <functional>
#include <iostream>

enum class VideoMode
{
    PRELOAD,   // load all frames at startup
    STREAM     // load only 1 frame at a time
};

class VideoLoader
{
public:
    VideoLoader();
    ~VideoLoader();

    bool load(const std::string& folderPath,
              const std::string& prefix,
              int startFrame,
              int endFrame,
              float fps,
              VideoMode mode);

    void update(float deltaTime);
    void render();

    void setLoop(bool state) { loop = state; }
    void reset();
    void play() { playing = true; }
    void pause() { playing = false; }

    std::function<void()> onFinished = nullptr;
    bool finished = false;
    void playOnce();// { loop = false; play(); finished = false; }

    GLuint getCurrentTexture() const;

    bool loop;
    bool playing;
    float timeAccumulator;
    int currentFrame;

private:
    // General
    std::string folder;
    std::string namePrefix;
    int firstFrame;
    int lastFrame;
    float frameDuration;
    VideoMode mode;

    // PRELOAD MODE
    std::vector<GLuint> frames;

    // STREAM MODE
    GLuint streamTexture;
    int streamWidth;
    int streamHeight;
    void loadFrameStream(int frameIndex);

    // Helpers
    std::string buildFramePath(int frameNumber) const;
    bool loadBMP(const std::string& filename,
                 std::vector<unsigned char>& outData,
                 int& width,
                 int& height);
};

#endif














/*
#ifndef _VIDEO_LOADER_H
#define _VIDEO_LOADER_H

#include <vector>
#include <string>
#include <SOIL2.h>
#include <gl/glew.h>
#include <functional>

enum class VideoMode
{
    PRELOAD,   // load all frames at startup
    STREAM     // load only 1 frame at a time
};

class VideoLoader
{
public:
    VideoLoader();
    ~VideoLoader();

    bool load(const std::string& folderPath,
              const std::string& prefix,
              int startFrame,
              int endFrame,
              float fps,
              VideoMode mode);

    void update(float deltaTime);
    void render();

    // control functions
    void setLoop(bool state) { loop = state; }
    void reset();
    void play() { playing = true; }
    void pause() { playing = false; }

    std::function<void()> onFinished = nullptr;
    bool finished = false;

    GLuint getCurrentTexture() const;

private:
    // general
    std::string folder;
    std::string namePrefix;
    int firstFrame;
    int lastFrame;
    float frameDuration;
    float timeAccumulator;
    int currentFrame;
    bool loop;
    bool playing;
    VideoMode mode;

    // PRELOAD MODE
    std::vector<GLuint> frames;

    // STREAM MODE
    GLuint streamTexture;
    void loadFrameStream(int frameIndex);

    // helper
    std::string buildFramePath(int frameNumber) const;
};

#endif
*/
