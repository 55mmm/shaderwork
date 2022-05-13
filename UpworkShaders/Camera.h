#pragma once

#include <glm/glm.hpp>

class Camera
{
public:

    Camera(const glm::vec3& eye, const glm::vec3& center);

    void SetEye(const glm::vec3& eye);
    void SetCenter(const glm::vec3& center);

    const glm::vec3& GetEye() const;
    const glm::vec3& GetCenter() const;

    const glm::mat4x4& GetViewMatrix();
    
private:
    void BuildViewMatrix();

    glm::vec3 mEye, mCenter;
    const glm::vec3 mUp;

    bool mViewMatrixUpToDate = false;
    glm::mat4x4 mView;
    
};