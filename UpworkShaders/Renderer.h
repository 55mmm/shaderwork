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

    void DrawCartoon();
    void DrawMirror(const std::vector<Node*>& regular, const std::vector<Node*>& inverted, Node* floor);

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
    Shader *mCelShader, *mNormalsShader, *mCartoonShader, *mMirrorShader;

    // [Framebuffer objects & Quad mesh]
    QuadMesh* mQuadMesh;

    void CreateFrameBuffers();
    void DestroyFrameBuffers();

    GLuint mNormalsAndDepthFBO, mCelShadingFBO; // First one is attached to normals and depth texture, second one to cel shading
    GLuint mNormalsMap;
    GLuint mDepthMap;
    GLuint mCelTexture;

    
};