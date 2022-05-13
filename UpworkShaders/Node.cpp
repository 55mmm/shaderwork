#include "Node.h"

#include <glm/gtc/matrix_transform.hpp>

Node::Node(Mesh* mesh, Texture* texture, Node* parent) :
mParent(parent),
mTexturedMesh(new TexturedMesh(mesh, texture)),
mScaleX(1.f),
mScaleY(1.f),
mScaleZ(1.f),
mRotX(0.f),
mRotY(0.f),
mRotZ(0.f),
mPosition(glm::vec3(0.f, 0.f, 0.f))
{
    BuildLocalModelMatrix();
}

Node::Node(Mesh* mesh, Texture* texture,
    float scale,
    float rotX,
    float rotY,
    float rotZ,
    const glm::vec3& position,
    Node* parent
) :
mParent(parent),
mTexturedMesh(new TexturedMesh(mesh, texture)),
mScaleX(scale),
mScaleY(scale),
mScaleZ(scale),
mRotX(rotX),
mRotY(rotY),
mRotZ(rotZ),
mPosition(position)
{
    BuildLocalModelMatrix();
}

Node::Node(Mesh* mesh, Texture* texture,
    float scaleX,
    float scaleY,
    float scaleZ,
    float rotX,
    float rotY,
    float rotZ,
    const glm::vec3& position,
    Node* parent
) :
mParent(parent),
mTexturedMesh(new TexturedMesh(mesh, texture)),
mScaleX(scaleX),
mScaleY(scaleY),
mScaleZ(scaleZ),
mRotX(rotX),
mRotY(rotY),
mRotZ(rotZ),
mPosition(position)
{
    BuildLocalModelMatrix();
}

Node::~Node()
{
    delete mTexturedMesh;
}

void Node::BuildLocalModelMatrix()
{
    mLocalModel = glm::mat4x4(1.f);
    mLocalModel = glm::translate(mLocalModel, mPosition);
    mLocalModel = glm::rotate(mLocalModel, glm::radians(mRotX), glm::vec3(1.f, 0.f, 0.f));
    mLocalModel = glm::rotate(mLocalModel, glm::radians(mRotY), glm::vec3(0.f, 1.f, 0.f));
    mLocalModel = glm::rotate(mLocalModel, glm::radians(mRotZ), glm::vec3(0.f, 0.f, 1.f));
    mLocalModel = glm::scale(mLocalModel, glm::vec3(mScaleX, mScaleY, mScaleZ));
    mLocalModelMatrixUpToDate = true;
}

void Node::BuildGlobalModelMatrix()
{
    if (mParent == nullptr)
        mGlobalModel = *GetLocalModelMatrix();
    else
        mGlobalModel = *(mParent->GetGlobalModelMatrix()) * *GetLocalModelMatrix();
}

void Node::SetParent(Node* parent)
{
    mParent = parent;
}

Node* Node::GetParent() const
{
    return mParent;
}

void Node::SetMesh(Mesh* mesh)
{
    mTexturedMesh->SetMesh(mesh);
}

void Node::SetTexture(Texture* texture)
{
    mTexturedMesh->SetTexture(texture);
}

void Node::SetScale(float scale)
{
    mScaleX = mScaleY = mScaleZ = scale;
    mLocalModelMatrixUpToDate = false;
}

void Node::SetScale(float x, float y, float z)
{
    mScaleX = x;
    mScaleY = y;
    mScaleZ = z;
}

void Node::SetRotation(float rotX, float rotY, float rotZ)
{
    mRotX = rotX;
    mRotY = rotY;
    mRotZ = rotZ;
    mLocalModelMatrixUpToDate = false;
}

void Node::SetPosition(const glm::vec3& position)
{
    mPosition = position;
    mLocalModelMatrixUpToDate = false;
}

void Node::SetReflectivity(float reflectivity)
{
    mReflectivity = reflectivity;
}

void Node::SetShineDamper(float shineDamper)
{
    mShineDamper = shineDamper;
}

void Node::SetSpecularVariables(float reflectivity, float shineDamper)
{
    mReflectivity = reflectivity;
    mShineDamper = shineDamper;
}

void Node::SetCastShadow(bool b)
{
    mCastShadow = b;
}

Mesh* Node::GetMesh() const
{
    return mTexturedMesh->GetMesh();
}

Texture* Node::GetTexture() const
{
    return mTexturedMesh->GetTexture();
}

TexturedMesh* Node::GetTexturedMesh() const
{
    return mTexturedMesh;
}

float Node::GetScaleX() const
{
    return mScaleX;
}

float Node::GetScaleY() const
{
    return mScaleY;
}

float Node::GetScaleZ() const
{
    return mScaleZ;
}

float Node::GetRotationX() const
{
    return mRotX;
}

float Node::GetRotationY() const
{
    return mRotY;
}

float Node::GetRotationZ() const
{
    return mRotZ;
}

glm::vec3 Node::GetPosition() const
{
    return mPosition;
}

float Node::GetReflectivity() const
{
    return mReflectivity;
}

float Node::GetShineDamper() const
{
    return mShineDamper;
}

bool Node::CastsShadow() const
{
    return mCastShadow;
}

const glm::mat4x4* Node::GetLocalModelMatrix()
{
    if (!mLocalModelMatrixUpToDate)
        BuildLocalModelMatrix();
    return &mLocalModel;
}

const glm::mat4x4* Node::GetGlobalModelMatrix()
{
    BuildGlobalModelMatrix();
    return &mGlobalModel;
}