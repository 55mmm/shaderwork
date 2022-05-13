#include "Mesh.h"

#include <iostream>

Mesh::Mesh(
    unsigned int numVertices,
    unsigned int numIndices,
    const float* positions,
    const float* normals,
    const float* textureCoords,
    const unsigned int* indices
) : 
mNumVertices(numVertices),
mNumIndices(numIndices)
{
    LoadToVAO(positions, normals, textureCoords, indices);
}

Mesh::Mesh(const aiScene* scene) :
mNumVertices(scene->mMeshes[0]->mNumVertices),
mNumIndices(scene->mMeshes[0]->mNumFaces * 3)
{
    const aiMesh* importedMesh = scene->mMeshes[0];
    const bool hasTexCoords = importedMesh->HasTextureCoords(0);
    
    float* positions = new float[3 * mNumVertices];
    float* normals = new float[3 * mNumVertices];
    float* textureCoords = new float[2 * mNumVertices];

    unsigned int* indices = new unsigned int[mNumIndices];

    // Positions, normals and texture coords
    for (unsigned int i = 0; i < mNumVertices; ++i)
    {
        positions[3 * i] = importedMesh->mVertices[i].x;
        positions[3 * i + 1] = importedMesh->mVertices[i].y;
        positions[3 * i + 2] = importedMesh->mVertices[i].z;

        normals[3 * i] = importedMesh->mNormals[i].x;
        normals[3 * i + 1] = importedMesh->mNormals[i].y;
        normals[3 * i + 2] = importedMesh->mNormals[i].z;

        if (hasTexCoords)
        {
            const aiVector3D* importedTextureCoords = importedMesh->mTextureCoords[0];
            textureCoords[2 * i] = importedTextureCoords[i].x;
            textureCoords[2 * i + 1] = importedTextureCoords[i].y;
        }
        else
        {
            textureCoords[2 * i] = 0.f;
            textureCoords[2 * i + 1] = 0.f;
        }
    }

    // Indices
    for (unsigned int i = 0; i < importedMesh->mNumFaces; ++i)
        for (unsigned int j = 0; j < 3; ++j)
            indices[3 * i + j] = importedMesh->mFaces[i].mIndices[j];
    
    // Loading
    LoadToVAO(positions, normals, textureCoords, indices);

    // Clean up memory
    delete[] positions;
    delete[] normals;
    delete[] textureCoords;
    delete[] indices;
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &mVAO);
    glDeleteBuffers(1, &mIndexBuffer);
    glDeleteBuffers(3, mVBOs);
}

unsigned int Mesh::GetNumVertices() const
{
    return mNumVertices;
}

unsigned int Mesh::GetNumIndices() const
{
    return mNumIndices;
}

GLuint Mesh::GetVaoID() const
{
    return mVAO;
}

void Mesh::LoadToVAO(
    const float* positions,
    const float* normals,
    const float* textureCoords,
    const unsigned int* indices
)
{
    // [Create VAO]
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    // [Index buffer]
    glGenBuffers(1, &mIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mNumIndices * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    // [VBOs]
    glGenBuffers(3, mVBOs);
    
    // Positions
    glBindBuffer(GL_ARRAY_BUFFER, mVBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, 3 * mNumVertices * sizeof(float), positions, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    // Normals
    glBindBuffer(GL_ARRAY_BUFFER, mVBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, 3 * mNumVertices * sizeof(float), normals, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    // Texture coordinates
    glBindBuffer(GL_ARRAY_BUFFER, mVBOs[2]);
    glBufferData(GL_ARRAY_BUFFER, 2 * mNumVertices * sizeof(float), textureCoords, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    // [Unbind]
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}