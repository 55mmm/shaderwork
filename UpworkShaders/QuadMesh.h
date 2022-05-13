#pragma once

#include <GL/glew.h>

class QuadMesh
{
public:
    QuadMesh();
    virtual ~QuadMesh();

    unsigned int GetNumVertices() const;
    GLuint GetVaoID() const;


private:



    /*

    (-1,1)-----------------(1,1)
      |                     |
      |                     |
      |                     |
      |                     |
      |                     |
    (-1,-1)-----------------(1,-1)

    */


    const float mVertices[8] =
    {
        -1.f, 1.f, // top left
        -1.f, -1.f, // bottom left
        1.f, 1.f, // top right
        1.f, -1.f // bottom right
    };


    const unsigned int mNumVertices = 4;

    GLuint mVBO = 0;
    GLuint mVAO = 0;
};