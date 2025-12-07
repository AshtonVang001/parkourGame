#include "_videoLoader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstring> // std::memcpy

VideoLoader::VideoLoader()
{
    streamTexture = 0;
    streamWidth = 0;
    streamHeight = 0;
    loop = true;
    playing = true;
    timeAccumulator = 0.0f;
    currentFrame = 0;
}

VideoLoader::~VideoLoader()
{
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
    folder = folderPath;
    namePrefix = prefix;
    firstFrame = startFrame;
    lastFrame = endFrame;
    frameDuration = 1.0f / fps;
    this->mode = mode;
    timeAccumulator = 0.0f;
    currentFrame = startFrame;
    finished = false;

    if (mode == VideoMode::PRELOAD)
    {
        for (int i = startFrame; i <= endFrame; ++i)
        {
            std::string path = buildFramePath(i);

            std::vector<unsigned char> bmpData;
            int w, h;
            if (!loadBMP(path, bmpData, w, h))
            {
                std::cout << "[VideoLoader] Failed to load frame: " << path << std::endl;
                return false;
            }

            GLuint tex;
            glGenTextures(1, &tex);
            glBindTexture(GL_TEXTURE_2D, tex);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // Correct color order for BMP
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR, GL_UNSIGNED_BYTE, bmpData.data());
            frames.push_back(tex);
        }
    }
    else if (mode == VideoMode::STREAM)
    {
        // Load first frame immediately
        std::vector<unsigned char> bmpData;
        if (!loadBMP(buildFramePath(startFrame), bmpData, streamWidth, streamHeight))
        {
            std::cout << "[VideoLoader] Failed to load first frame for stream: "
                      << buildFramePath(startFrame) << std::endl;
            return false;
        }

        // Create the OpenGL texture if it doesn't exist
        if (!streamTexture)
            glGenTextures(1, &streamTexture);

        glBindTexture(GL_TEXTURE_2D, streamTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Upload the first frame
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, streamWidth, streamHeight, 0,
                     GL_BGR, GL_UNSIGNED_BYTE, bmpData.data());

        // Ensure the first frame is rendered immediately
        currentFrame = startFrame;
        finished = false;
        playing = true;
    }

    return true;
}

void VideoLoader::update(float deltaTime)
{
    if (!playing) return;

    timeAccumulator += deltaTime;

    while (timeAccumulator >= frameDuration)
    {
        timeAccumulator -= frameDuration;
        currentFrame++;

        if (currentFrame > lastFrame)
        {
            if (loop)
                currentFrame = firstFrame; // loop again
            else
            {
                currentFrame = lastFrame;
                if (!finished)
                {
                    finished = true;
                    playing = false;
                }
            }
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
    if (!playing)
        return 0; // don't display any frame until play is called

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
    timeAccumulator = 0.0f;
    finished = false;

    if (mode == VideoMode::STREAM)
        loadFrameStream(currentFrame);
}

void VideoLoader::loadFrameStream(int frameIndex)
{
    std::vector<unsigned char> bmpData;
    int w, h;
    if (!loadBMP(buildFramePath(frameIndex), bmpData, w, h))
    {
        std::cout << "[VideoLoader] Failed to load frame: " << buildFramePath(frameIndex) << std::endl;
        return;
    }

    if (w != streamWidth || h != streamHeight)
    {
        std::cout << "[VideoLoader] Frame size mismatch!" << std::endl;
        return;
    }

    glBindTexture(GL_TEXTURE_2D, streamTexture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, streamWidth, streamHeight, GL_BGR, GL_UNSIGNED_BYTE, bmpData.data());
}

bool VideoLoader::loadBMP(const std::string& filename,
                          std::vector<unsigned char>& outData,
                          int& width,
                          int& height)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) return false;

    unsigned char header[54];
    file.read(reinterpret_cast<char*>(header), 54);
    if (header[0] != 'B' || header[1] != 'M') return false;

    width  = *reinterpret_cast<int*>(&header[18]);
    height = *reinterpret_cast<int*>(&header[22]);
    int dataOffset = *reinterpret_cast<int*>(&header[10]);

    int rowSize = (width * 3 + 3) & (~3); // padded to 4 bytes
    outData.resize(width * height * 3);
    std::vector<unsigned char> rowData(rowSize);

    file.seekg(dataOffset, std::ios::beg);
    for (int y = 0; y < height; ++y)
    {
        file.read(reinterpret_cast<char*>(rowData.data()), rowSize);
        std::memcpy(&outData[(height - 1 - y) * width * 3], rowData.data(), width * 3);
    }

    return true;
}

std::string VideoLoader::buildFramePath(int frameNumber) const
{
    std::ostringstream ss;
    ss << folder << "/" << namePrefix << std::setw(4)
       << std::setfill('0') << frameNumber << ".bmp";
    return ss.str();
}

void VideoLoader::playOnce()
{
    loop = false;
    currentFrame = firstFrame;
    timeAccumulator = 0.0f;
    playing = true;
    finished = false;

    // Force the first frame to be loaded and ready
    if (mode == VideoMode::STREAM)
        loadFrameStream(currentFrame);

    // For PRELOAD, nothing special needed — frames already loaded
}













/*
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
                if (!finished)
                {
                    finished = true;
                    playing = false;

                    if (onFinished)
                        onFinished();
                }
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
*/
