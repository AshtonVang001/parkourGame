#include "_videoLoader.h"
#include <iostream>
#include <sstream>
#include <iomanip>

VideoLoader::VideoLoader()
{
    streamTexture = 0;
    loop = true;
    playing = true;
    timeAccumulator = 0.0f;
    currentFrame = 0;
}

VideoLoader::~VideoLoader()
{
    // Cleanup
    for (auto tex : frames)
        glDeleteTextures(1, &tex);

    if (streamTexture)
        glDeleteTextures(1, &streamTexture);
}

bool VideoLoader::load(const std::string& folderPath,
                       const std::string& prefix,
                       int startFrame,
                       int endFrame,
                       float fps,
                       VideoMode mode)
{
    this->folder      = folderPath;
    this->namePrefix  = prefix;
    this->firstFrame  = startFrame;
    this->lastFrame   = endFrame;
    this->frameDuration = 1.0f / fps;
    this->mode        = mode;
    this->timeAccumulator = 0.0f;
    this->currentFrame = startFrame;

    if (mode == VideoMode::PRELOAD)
    {
        // Preload frames into VRAM
        for (int i = startFrame; i <= endFrame; ++i)
        {
            std::string path = buildFramePath(i);

            GLuint tex = SOIL_load_OGL_texture(
                path.c_str(),
                SOIL_LOAD_AUTO,
                SOIL_CREATE_NEW_ID,
                SOIL_FLAG_MIPMAPS
            );

            if (!tex)
            {
                std::cout << "[VideoLoader] Failed to load frame: " << path << std::endl;
                return false;
            }

            frames.push_back(tex);
        }
    }
    else if (mode == VideoMode::STREAM)
    {
        // create one texture
        glGenTextures(1, &streamTexture);
        glBindTexture(GL_TEXTURE_2D, streamTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // load first frame
        loadFrameStream(startFrame);
    }

    return true;
}

void VideoLoader::update(float deltaTime)
{
    if (!playing) return;

    timeAccumulator += deltaTime;   // accumulate time

    // Advance the video as many frames as needed (handles slow frames)
    while (timeAccumulator >= frameDuration)
    {
        timeAccumulator -= frameDuration;
        currentFrame++;

        if (currentFrame > lastFrame)
        {
            if (loop)
                currentFrame = firstFrame;
            else
                currentFrame = lastFrame;
        }

        if (mode == VideoMode::STREAM)
            loadFrameStream(currentFrame);
    }
}

void VideoLoader::render()
{
    glBindTexture(GL_TEXTURE_2D, getCurrentTexture());
}

GLuint VideoLoader::getCurrentTexture() const
{
    if (mode == VideoMode::PRELOAD)
    {
        int index = currentFrame - firstFrame;
        if (index < 0 || index >= (int)frames.size()) return 0;
        return frames[index];
    }
    else
    {
        return streamTexture;
    }
}

void VideoLoader::reset()
{
    currentFrame = firstFrame;
    timeAccumulator = 0;

    if (mode == VideoMode::STREAM)
        loadFrameStream(currentFrame);
}

void VideoLoader::loadFrameStream(int frameIndex)
{
    std::string path = buildFramePath(frameIndex);

    int w, h, ch;
    unsigned char* img = SOIL_load_image(path.c_str(), &w, &h, &ch, SOIL_LOAD_RGBA);

    if (!img)
    {
        std::cout << "[VideoLoader] Stream failed to load: " << path << std::endl;
        return;
    }

    glBindTexture(GL_TEXTURE_2D, streamTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);

    SOIL_free_image_data(img);
}

std::string VideoLoader::buildFramePath(int frameNumber) const
{
    std::ostringstream ss;
    ss << folder << "/" << namePrefix << std::setw(4)
       << std::setfill('0') << frameNumber << ".png";
    return ss.str();
}
