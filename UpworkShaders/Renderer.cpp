#include "Renderer.h"

#include <ctime>
#include <cstdlib>
#include <iostream>

#include "Settings.h"

Renderer::Renderer(int width, int height, Camera* camera, PointLight* light) :
mWidth(width),
mHeight(height),
mAspectRatio((float) width / height),
mCamera(camera),
mPointLight(light),
mCartoonShader(new Shader("ui")),
mNormalsShader(new Shader("normals")),
mDepthShader(new Shader("depth")),
mQuadMesh(new QuadMesh)
{
    CreateFrameBuffer();

    // Initialize shaders
    mCartoonShader->Initialize();
    mNormalsShader->Initialize();
    mDepthShader->Initialize();


    // Projection matrix
    BuildAndLoadProjectionMatrix();

    // Bind texture slot locations
    mCartoonShader->Start();
    mCartoonShader->LoadUniformInt("normalMap", 0);
    mCartoonShader->LoadUniformInt("depthMap", 1);
    mCartoonShader->Stop();

}

Renderer::~Renderer()
{
    delete mCartoonShader;
    delete mNormalsShader;
    delete mDepthShader;

    delete mQuadMesh;

    DestroyFrameBuffer();
}

void Renderer::Prepare()
{
    mNodes.clear();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}

void Renderer::Add(Node* node)
{
    // By textured mesh

    auto meshKey = node->GetMesh();
    auto textureKey = node->GetTexture();

    if (mNodes.contains(meshKey))
    {
        if (mNodes[meshKey].contains(textureKey))
            mNodes[meshKey][textureKey].push_back(node);
        else
        {
            std::vector<Node*> v{ node };
            mNodes[meshKey].insert(std::make_pair(textureKey, v));
        }
    }
    else
    {
        std::vector<Node*> v{ node };
        std::unordered_map<Texture*, std::vector<Node*>> m;
        m.insert(std::make_pair(textureKey, v));
        mNodes.insert(std::make_pair(meshKey, m));
    }
}

void Renderer::Add(const std::vector<Node*>& nodes)
{
    for (const auto& n : nodes)
        Add(n);
}

void Renderer::Draw()
{
    // Render normals map as usual

    glViewport(0, 0, mWidth, mHeight);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glBindFramebuffer(GL_FRAMEBUFFER, mFrameBufferObject);
    glClearColor(0.f, 0.f, 0.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mNormalsShader->Start();
    mNormalsShader->LoadUniformMat4x4("view", mCamera->GetViewMatrix());

    glDisable(GL_CULL_FACE);
    // glCullFace(GL_BACK);

    for (auto pair : mNodes)
    {
        auto mesh = pair.first;
        auto textureMap = pair.second;

        glBindVertexArray(mesh->GetVaoID());
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        for (auto pair : textureMap)
        {
            for (auto node : pair.second)
            {
                if (!node->CastsShadow())
                    continue;
                mNormalsShader->LoadUniformMat4x4("model", *node->GetGlobalModelMatrix());
                glDrawElements(GL_TRIANGLES, mesh->GetNumIndices(), GL_UNSIGNED_INT, 0);
            }
        }

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
    }

    mNormalsShader->Stop();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // [Final rendering step]


    // Enable alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    mCartoonShader->Start();
    mCartoonShader->LoadUniformMat4x4("view", glm::mat4(1.f));
    glClearColor(.937f, .941f, .945f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mNormalsMap);

    glBindVertexArray(mQuadMesh->GetVaoID());
    glEnableVertexAttribArray(0);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, mQuadMesh->GetNumVertices());


    glDisableVertexAttribArray(0);
    mCartoonShader->Stop();

    // Disable alpha blending
    glDisable(GL_BLEND);

}

void Renderer::Resize(int width, int height)
{
    mWidth = width;
    mHeight = height;
    mAspectRatio = (float) width / height;
    BuildAndLoadProjectionMatrix();

    // Create framebuffers of new size
    DestroyFrameBuffer();
    CreateFrameBuffer();
}

void Renderer::BuildAndLoadProjectionMatrix()
{
    mProjection = glm::perspective(
        glm::radians(mFOV),
        mAspectRatio,
        mNearPlane,
        mFarPlane
    );

    // mCartoonShader->Start();
    // mCartoonShader->LoadUniformMat4x4("projection", mProjection);
    // mCartoonShader->Stop();

    mNormalsShader->Start();
    mNormalsShader->LoadUniformMat4x4("projection", mProjection);
    mNormalsShader->Stop();

    mDepthShader->Start();
    mDepthShader->LoadUniformMat4x4("projection", mProjection);
    mDepthShader->Stop();

}

void Renderer::CreateFrameBuffer()
{
    // Generate and bind FBO
    glGenFramebuffers(1, &mFrameBufferObject);
    glBindFramebuffer(GL_FRAMEBUFFER, mFrameBufferObject);

    // Normals map
    glGenTextures(1, &mNormalsMap);
    glBindTexture(GL_TEXTURE_2D, mNormalsMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mWidth, mHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Depth map
    glGenTextures(1, &mDepthMap);
    glBindTexture(GL_TEXTURE_2D, mDepthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, mWidth, mHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // Attach textures to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mNormalsMap, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDepthMap, 0);

    // Unbind FBO
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Renderer::DestroyFrameBuffer()
{
    glDeleteFramebuffers(1, &mFrameBufferObject);

    glDeleteTextures(1, &mNormalsMap);
    glDeleteTextures(1, &mDepthMap);
}

