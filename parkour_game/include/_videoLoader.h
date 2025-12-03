#ifndef _VIDEO_LOADER_H
#define _VIDEO_LOADER_H

#include <vector>
#include <string>
#include <SOIL2.h>
#include <gl/glew.h>

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
