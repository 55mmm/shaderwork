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

    (0,1)-----------------(1,1)
      |                     |
      |                     |
      |                     |
      |                     |
      |                     |
    (0,0)-----------------(1,0)

    */


    const float mVertices[8] =
    {
        0.f, 1.f, // top left
        0.f, 0.f, // bottom left
        1.f, 1.f, // top right
        1.f, 0.f // bottom right
    };


    const unsigned int mNumVertices = 4;

    GLuint mVBO = 0;
    GLuint mVAO = 0;
};