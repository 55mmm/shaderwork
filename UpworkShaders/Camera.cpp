#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(const glm::vec3& eye, const glm::vec3& center) :
mEye(eye),
mCenter(center),
mUp(0.f, 1.f, 0.f)
{
    BuildViewMatrix();
}

void Camera::SetEye(const glm::vec3& eye)
{
    mEye = eye;
    mViewMatrixUpToDate = false;
}

void Camera::SetCenter(const glm::vec3& center)
{
    mCenter = center;
    mViewMatrixUpToDate = false;
}

const glm::vec3& Camera::GetEye() const
{
    return mEye;
}

const glm::vec3& Camera::GetCenter() const
{
    return mCenter;
}

const glm::mat4x4& Camera::GetViewMatrix()
{
    if (!mViewMatrixUpToDate)
        BuildViewMatrix();
    return mView;
}

void Camera::BuildViewMatrix()
{
    mView = glm::lookAt(
        mEye,
        mCenter,
        mUp
    );
    mViewMatrixUpToDate = true;
}