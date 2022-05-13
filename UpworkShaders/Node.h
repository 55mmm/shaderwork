#pragma once

#include <glm/glm.hpp>

#include <string>

#include "Mesh.h"
#include "Texture.h"
#include "TexturedMesh.h"

class Node
{
public:
    Node(Mesh* mesh, Texture* texture, Node* parent);

    Node(Mesh* mesh, Texture* texture,
        float scale,
        float rotX,
        float rotY,
        float rotZ,
        const glm::vec3& position,
        Node* parent
    );

    Node(Mesh* mesh, Texture* texture,
        float scaleX,
        float scaleY,
        float scaleZ,
        float rotX,
        float rotY,
        float rotZ,
        const glm::vec3& position,
        Node* parent
    );

    virtual ~Node();

private:

    Node* mParent = nullptr;

    TexturedMesh* mTexturedMesh = nullptr;

    float mScaleX, mScaleY, mScaleZ;
    float mRotX, mRotY, mRotZ;
    glm::vec3 mPosition;

    glm::mat4x4 mLocalModel;
    glm::mat4x4 mGlobalModel;

    bool mLocalModelMatrixUpToDate = false;

    void BuildLocalModelMatrix();
    void BuildGlobalModelMatrix();

    float mReflectivity = 0.325f;
    float mShineDamper = 6.f;

    bool mCastShadow = true; // set this to false if object should not cast shadow

public:

    void SetParent(Node* parent);
    Node* GetParent() const;

    void SetMesh(Mesh* mesh);
    void SetTexture(Texture* texture);
    void SetScale(float scale);
    void SetScale(float x, float y, float z);
    void SetRotation(float rotX, float rotY, float rotZ);
    void SetPosition(const glm::vec3& position);

    void SetReflectivity(float reflectivity);
    void SetShineDamper(float shineDamper);
    void SetSpecularVariables(float reflectivity, float shineDamper);

    void SetCastShadow(bool b);

    Mesh* GetMesh() const;
    Texture* GetTexture() const;
    TexturedMesh* GetTexturedMesh() const;

    float GetScaleX() const;
    float GetScaleY() const;
    float GetScaleZ() const;
    float GetRotationX() const;
    float GetRotationY() const;
    float GetRotationZ() const;
    glm::vec3 GetPosition() const;

    float GetReflectivity() const;
    float GetShineDamper() const;

    bool CastsShadow() const;

    const glm::mat4x4* GetLocalModelMatrix();
    const glm::mat4x4* GetGlobalModelMatrix();
};