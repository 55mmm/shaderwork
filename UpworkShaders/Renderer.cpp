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
mCelShader(new Shader("cel")),
mNormalsShader(new Shader("normals")),
mCartoonShader(new Shader("cartoon")),
mMirrorShader(new Shader("mirror")),
mQuadMesh(new QuadMesh)
{
    CreateFrameBuffers();

    // Initialize shaders
    mCelShader->Initialize();
    mNormalsShader->Initialize();
    mCartoonShader->Initialize();
    mMirrorShader->Initialize();


    // Projection matrix
    BuildAndLoadProjectionMatrix();

    // Bind texture slot locations
    mCartoonShader->Start();
    mCartoonShader->LoadUniformInt("normalMap", 0);
    mCartoonShader->LoadUniformInt("depthMap", 1);
    mCartoonShader->LoadUniformInt("celTexture", 2);
    mCartoonShader->Stop();

    // Set number of cel steps
    mCelShader->Start();
    mCelShader->LoadUniformFloat("celSteps", 5.f);
    mCelShader->Stop();

}

Renderer::~Renderer()
{
    delete mCelShader;
    delete mNormalsShader;
    delete mCartoonShader;
    delete mMirrorShader;

    delete mQuadMesh;

    DestroyFrameBuffers();
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

void Renderer::DrawCartoon()
{
    // General
    glViewport(0, 0, mWidth, mHeight);
    // Disable face culling for our models
    glDisable(GL_CULL_FACE);
    // glCullFace(GL_BACK);


    // [Normals and depth framebuffer]
    glBindFramebuffer(GL_FRAMEBUFFER, mNormalsAndDepthFBO);
    glClearColor(0.f, 0.f, 0.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render normals map
    mNormalsShader->Start();
    mNormalsShader->LoadUniformMat4x4("view", mCamera->GetViewMatrix());
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

    // [Cel shading FBO]
    glBindFramebuffer(GL_FRAMEBUFFER, mCelShadingFBO);
    glClearColor(1.f, 1.f, 1.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mCelShader->Start();
    mCelShader->LoadUniformMat4x4("view", mCamera->GetViewMatrix());
    mCelShader->LoadUniformVec3("lightPosition", mPointLight->GetPosition());
    mCelShader->LoadUniformVec3("lightColor", mPointLight->GetColor());
    for (auto pair1: mNodes)
    {
        auto mesh = pair1.first;
        auto textureMap = pair1.second;
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, mDepthMap);
        glBindVertexArray(mesh->GetVaoID());
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        for (auto pair2 : textureMap)
        {
            auto texture = pair2.first;
            auto nodes = pair2.second;
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture->GetID());
            for (auto node : nodes)
            {
                mCelShader->LoadUniformMat4x4("model", *node->GetGlobalModelMatrix());
                mCelShader->LoadUniformFloat("reflectivity", node->GetReflectivity());
                mCelShader->LoadUniformFloat("shineDamper", node->GetShineDamper());

                glDrawElements(GL_TRIANGLES, mesh->GetNumIndices(), GL_UNSIGNED_INT, 0);

            }

        }
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);

    }

    mCelShader->Stop();

    // [Final rendering step]

    // Unbind fbo
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    mCartoonShader->Start();
    mCartoonShader->LoadUniformMat4x4("view", glm::mat4(1.f));
    glClearColor(.937f, .941f, .945f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // [Normals and depth maps]
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mNormalsMap);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mDepthMap);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, mCelTexture);

    glBindVertexArray(mQuadMesh->GetVaoID());
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, mQuadMesh->GetNumVertices());
    glDisableVertexAttribArray(0);

    mCartoonShader->Stop();
}

void Renderer::DrawMirror(const std::vector<Node*>& nodes, Node* floor)
{
    glViewport(0, 0, mWidth, mHeight);
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Shader parameters

    mMirrorShader->Start();
    mMirrorShader->LoadUniformMat4x4("view", mCamera->GetViewMatrix());
    mMirrorShader->LoadUniformVec3("lightPosition", mPointLight->GetPosition());
    mMirrorShader->LoadUniformVec3("lightColor", mPointLight->GetColor());
    mMirrorShader->LoadUniformInt("flipY", 0);

    // Render regular nodes

    for (auto node : nodes)
    {
        glBindVertexArray(node->GetMesh()->GetVaoID());
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, node->GetTexture()->GetID());
        
        mMirrorShader->LoadUniformMat4x4("model", *node->GetGlobalModelMatrix());
        mMirrorShader->LoadUniformFloat("reflectivity", node->GetReflectivity());
        mMirrorShader->LoadUniformFloat("shineDamper", node->GetShineDamper());

        glDrawElements(GL_TRIANGLES, node->GetMesh()->GetNumIndices(), GL_UNSIGNED_INT, 0);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
    }

    // Enable stencil testing and set test function and operations to write ones to all selected stencils.

    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 1, 0xFF); // Set any stencil to 1
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilMask(0xFF); // Write to stencil buffer
    glDepthMask(GL_FALSE); // Don't write to depth buffer
    glClear(GL_STENCIL_BUFFER_BIT); // Clear stencil buffer (0 by default)

    // Render floor

    glBindVertexArray(floor->GetMesh()->GetVaoID());
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, floor->GetTexture()->GetID());
    
    mMirrorShader->LoadUniformVec3("lightColor", glm::vec3(0.25f));
    mMirrorShader->LoadUniformMat4x4("model", *floor->GetGlobalModelMatrix());
    mMirrorShader->LoadUniformFloat("reflectivity", floor->GetReflectivity());
    mMirrorShader->LoadUniformFloat("shineDamper", floor->GetShineDamper());

    glDrawElements(GL_TRIANGLES, floor->GetMesh()->GetNumIndices(), GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    // Set stencil function to pass if stencil value equals 1

    glStencilFunc(GL_EQUAL, 1, 0xFF); // Pass test if stencil value is 1
    glStencilMask(0x00); // Don't write anything to stencil buffer
    glDepthMask(GL_TRUE); // Write to depth buffer

    // Render inverted nodes
    mMirrorShader->LoadUniformVec3("lightColor", glm::vec3(0.25f));
    mMirrorShader->LoadUniformInt("flipY", 1);

    for (auto node : nodes)
    {
        glBindVertexArray(node->GetMesh()->GetVaoID());
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, node->GetTexture()->GetID());
        
        mMirrorShader->LoadUniformMat4x4("model", *node->GetGlobalModelMatrix());
        mMirrorShader->LoadUniformFloat("reflectivity", node->GetReflectivity());
        mMirrorShader->LoadUniformFloat("shineDamper", node->GetShineDamper());

        glDrawElements(GL_TRIANGLES, node->GetMesh()->GetNumIndices(), GL_UNSIGNED_INT, 0);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
    }

    // Disable stencil testing
    glDisable(GL_STENCIL_TEST);
}

void Renderer::Resize(int width, int height)
{
    mWidth = width;
    mHeight = height;
    mAspectRatio = (float) width / height;
    BuildAndLoadProjectionMatrix();

    // Create framebuffers of new size
    DestroyFrameBuffers();
    CreateFrameBuffers();
}

void Renderer::BuildAndLoadProjectionMatrix()
{
    mProjection = glm::perspective(
        glm::radians(mFOV),
        mAspectRatio,
        mNearPlane,
        mFarPlane
    );

    mCelShader->Start();
    mCelShader->LoadUniformMat4x4("projection", mProjection);
    mCelShader->Stop();

    mNormalsShader->Start();
    mNormalsShader->LoadUniformMat4x4("projection", mProjection);
    mNormalsShader->Stop();

    mCartoonShader->Start();
    mCartoonShader->LoadUniformVec2("framebufferSize", glm::vec2(mWidth, mHeight));
    mCartoonShader->Stop();

    mMirrorShader->Start();
    mMirrorShader->LoadUniformMat4x4("projection", mProjection);
    mMirrorShader->Stop();

}

void Renderer::CreateFrameBuffers()
{
    // Generate and bind normals & depth attachment FBO
    glGenFramebuffers(1, &mNormalsAndDepthFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, mNormalsAndDepthFBO);

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


    // Create and bind cel shading FBO
    glGenFramebuffers(1, &mCelShadingFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, mCelShadingFBO);
    glGenTextures(1, &mCelTexture);
    glBindTexture(GL_TEXTURE_2D, mCelTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mWidth, mHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Attach texture to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mCelTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDepthMap, 0);

    // Unbind FBO
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Renderer::DestroyFrameBuffers()
{
    glDeleteFramebuffers(1, &mNormalsAndDepthFBO);
    glDeleteFramebuffers(1, &mCelShadingFBO);

    glDeleteTextures(1, &mNormalsMap);
    glDeleteTextures(1, &mDepthMap);
    glDeleteTextures(1, &mCelTexture);
}

