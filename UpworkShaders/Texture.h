#pragma once

#include <GL/glew.h>

#include <string>

#include "Settings.h"

class Texture
{
public:
    Texture(const std::string& file, bool bilinearFiltering=true, bool flipUVs=true);
    ~Texture();

    GLuint GetID() const;
    bool HasAlphaChannel() const;
private:
    GLuint mTextureID;
    int mNumChannels = 0;

    const std::string mFilePrefix = Settings::resourcesPathPrefix + "textures/";
};
