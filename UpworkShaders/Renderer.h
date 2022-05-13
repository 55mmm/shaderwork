#pragma once

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Mesh.h"
#include "Node.h"
#include "PointLight.h"
#include "QuadMesh.h"
#include "Texture.h"
#include "TexturedMesh.h"


#include <vector>
#include <unordered_map>

class Renderer
{
public:
    Renderer(int width, int height, Camera* camera, PointLight* light);
    virtual ~Renderer();

    void Prepare();
    void Add(Node* node);
    void Add(const std::vector<Node*>& nodes);

    void Draw();

    void Resize(int width, int height);

    GLuint GetDepthMap() const;

private:
    void BuildAndLoadProjectionMatrix();
    glm::mat4x4 mProjection;

    std::unordered_map<Mesh*, std::unordered_map<Texture*, std::vector<Node*>>> mNodes;

    int mWidth, mHeight;
    float mAspectRatio;

    Camera* mCamera;
    PointLight* mPointLight;

    const float mFOV = 40.f; // 45
    const float mNearPlane = 1.f;
    const float mFarPlane = 100.f;

    // [Shaders]
    Shader *mCartoonShader, *mNormalsShader, *mDepthShader;

    // [Framebuffer objects & Quad mesh]
    QuadMesh* mQuadMesh;

    void CreateFrameBuffer();
    void DestroyFrameBuffer();

    GLuint mFrameBufferObject;
    GLuint mNormalsMap;
    GLuint mDepthMap;

    
};