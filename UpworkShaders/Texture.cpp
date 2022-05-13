#include "Texture.h"

#include <iostream>
#include <stdexcept>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture(const std::string& file, bool bilinearFiltering, bool flipUVs)
{
    glGenTextures(1, &mTextureID);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mTextureID);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    if (bilinearFiltering)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    const std::string path = mFilePrefix + file;
    int width, height;
    stbi_set_flip_vertically_on_load(flipUVs);
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &mNumChannels, 0);
    if (!data)
    {
        const std::string error = std::string("Failed to load \"") + path + "\"";
        throw std::runtime_error(error);
    }

    GLenum format = 0;
    if (mNumChannels == 3)
        format = GL_RGB;
    else if (mNumChannels == 4)
        format = GL_RGBA;
    else
        throw std::runtime_error(std::string("Unknown pixel format for \"") + path + "\"");
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);    
}

Texture::~Texture()
{
    glDeleteTextures(1, &mTextureID);
}

GLuint Texture::GetID() const
{
    return mTextureID;
}

bool Texture::HasAlphaChannel() const
{
    return mNumChannels == 4;
}