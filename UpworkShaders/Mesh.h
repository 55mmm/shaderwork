#pragma once

#include <GL/glew.h>
#include <assimp/scene.h>

#include <string>

class Mesh
{
public:
    Mesh(
        unsigned int numVertices,
        unsigned int numIndices,
        const float* positions,
        const float* normals,
        const float* textureCoords,
        const unsigned int* indices
    );
    Mesh(const aiScene* scene);

    virtual ~Mesh();

    unsigned int GetNumVertices() const;
    unsigned int GetNumIndices() const;
    GLuint GetVaoID() const;

private:
    void LoadToVAO(
        const float* positions,
        const float* normals,
        const float* textureCoords,
        const unsigned int* indices
    );
    
    const unsigned int mNumVertices;
    const unsigned int mNumIndices;

    GLuint mVBOs[3] = { 0, 0, 0 };
    GLuint mVAO = 0;

    GLuint mIndexBuffer = 0;

};
