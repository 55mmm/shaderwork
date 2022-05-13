#include "QuadMesh.h"

#include <iostream>

QuadMesh::QuadMesh()
{
    // [Create VAO]
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    // [VBO]
    glGenBuffers(1, &mVBO);
    
    // Positions
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, 2 * mNumVertices * sizeof(float), mVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    // [Unbind]
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

QuadMesh::~QuadMesh()
{
    glDeleteVertexArrays(1, &mVAO);
    glDeleteBuffers(1, &mVBO);
}

unsigned int QuadMesh::GetNumVertices() const
{
    return mNumVertices;
}

GLuint QuadMesh::GetVaoID() const
{
    return mVAO;
}