#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Camera.h"
#include "PointLight.h"
#include "Renderer.h"
#include "SceneLoader.h"

class App
{
public:
    App();
    virtual ~App();

private:
    void InitGL();
    void LoadResources();
    void UnloadResources();

public:
    void MainLoop();

private:
    static void FrameBufferResizeCallback(GLFWwindow* window, int width, int height);
    void Resize(int width, int height);

    GLFWwindow* mWindow;
 
    PointLight* mPointLight;
    Renderer* mRenderer;
    Camera* mCamera;

    // Resources
    SceneLoader* sl_;
    TexturedMesh* mesh_;
    Node* node_;
};