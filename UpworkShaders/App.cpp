#include "App.h"

#include <chrono>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>

#include "SceneLoader.h"
#include "Settings.h"

App::App()
{
    // mCamera = new Camera(
    //     glm::vec3(0.f, 7.0f, 7.f),
    //     glm::vec3(0.f, 2.75f, 0.f)
    // );
    mCamera = new Camera(
        glm::vec3(0.f, 6.0f, 9.f),
        glm::vec3(0.f, 1.5f, 0.f)
    );
    InitGL();
    LoadResources();
}

App::~App()
{
    UnloadResources();

    delete mRenderer;
    delete mPointLight;
    delete mCamera;

    // Destroy window and terminate
    glfwDestroyWindow(mWindow);
    glfwTerminate();
}

void App::InitGL()
{
    if (!glfwInit())
        throw std::runtime_error("Failed to init GLFW.");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //if (Settings::windowMaximized)
        //glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE); // this causes a bug under Windows

    glfwWindowHint(GLFW_SAMPLES, Settings::samples);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* vmode = glfwGetVideoMode(monitor);
    const int initialWidth = 3 * vmode->width / 4;
    const int initialHeight = 3 * vmode->height / 4;
    mWindow = glfwCreateWindow(
        initialWidth,
        initialHeight,
        "Shaders",
        NULL,
        NULL
    );

    if (mWindow == NULL)
        throw std::runtime_error("Failed to create window.");

    glfwSetWindowSizeLimits(mWindow, 640, 480, GLFW_DONT_CARE, GLFW_DONT_CARE);
    glfwMakeContextCurrent(mWindow);

    // Setup resize callbacks
    glfwSetWindowUserPointer(mWindow, this);
    glfwSetFramebufferSizeCallback(mWindow, FrameBufferResizeCallback);

    // Start GLEW
    glewExperimental = GL_TRUE;
    glewInit();


    const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
    const GLubyte* version = glGetString(GL_VERSION); // version as a string

    std::cout << "Renderer: " << renderer << std::endl;
    std::cout << "OpenGL version: " << version << std::endl;

    glViewport(0, 0, initialWidth, initialHeight);
    mPointLight = new PointLight(glm::vec3(25.0, 35.0, 25.0));
    mRenderer = new Renderer(initialWidth, initialHeight, mCamera, mPointLight);
}

void App::LoadResources()
{
    sl_ = new SceneLoader;
    mesh1_ = new TexturedMesh(new Mesh(sl_->LoadScene("teapot.obj")), new Texture("red.png"));
    mesh2_ = new TexturedMesh(new Mesh(sl_->LoadScene("suzanne.obj")), new Texture("blue.png", true, false));
    mesh3_ = new TexturedMesh(new Mesh(sl_->LoadScene("plane.obj")), new Texture("gray.png"));
    node1_ = new Node(mesh1_->GetMesh(), mesh1_->GetTexture(), 0.75f, 0.f, 0.f, 0.f, glm::vec3(2.f, .5f, 0.f), nullptr);
    node2_ = new Node(mesh2_->GetMesh(), mesh2_->GetTexture(), 1.25f, 0.f, 0.f, 0.f, glm::vec3(-3.f, .0f, -1.f), nullptr);
    node3_ = new Node(mesh3_->GetMesh(), mesh3_->GetTexture(), 15.f, 0.f, 30.f, 0.f, glm::vec3(), nullptr);
}

void App::UnloadResources()
{
    delete sl_;
    delete mesh1_;
    delete mesh2_;
    delete mesh3_;
    delete node1_;
    delete node2_;
    delete node3_;
}

void App::MainLoop()
{
    while(!glfwWindowShouldClose(mWindow))
    {
        // [Input Handling]
        glfwPollEvents();
        
        // Cursor
        glm::dvec2 cursorPosition{};
        glfwGetCursorPos(mWindow, &cursorPosition.x, &cursorPosition.y);

        // Mouse
        const std::array<bool, 2> currentMouseState = {
            glfwGetMouseButton(mWindow, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS,
            glfwGetMouseButton(mWindow, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS
        };

        // [Time]
        static auto startTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        // [Update node]
        node1_->SetRotation(0.0, 25.f * time, 0.0);
        node2_->SetRotation(0.0, -30.f * time, 0.f);
        float y = 1.f + std::sin(time / 1.f);
        node1_->SetPosition(glm::vec3(node1_->GetPosition().x, y, node1_->GetPosition().z));
        node2_->SetPosition(glm::vec3(node2_->GetPosition().x, 0.5f + y, node2_->GetPosition().z));

        // [Drawing]
        mRenderer->Prepare();
        mRenderer->Add(node1_);
        mRenderer->Add(node2_);
        std::vector<Node*> mirroredNodes{node1_, node2_};
        mRenderer->DrawMirror(mirroredNodes, node3_);
        // mRenderer->DrawCartoon();

        // [Refreshing]
        glfwSwapInterval(1);
        glfwSwapBuffers(mWindow);
    }

}

void App::FrameBufferResizeCallback(GLFWwindow* window, int width, int height)
{
    auto app = reinterpret_cast<App*>(glfwGetWindowUserPointer(window));
    app->Resize(width, height);
}

void App::Resize(int width, int height)
{
    std::cout << "Resizing to " << width << "x" << height << std::endl;
    glViewport(0, 0, width, height);

    // Update projection matrices here
    mRenderer->Resize(width, height);
}
