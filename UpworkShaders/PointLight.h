#pragma once

#include <glm/glm.hpp>

class PointLight
{
public:
    PointLight(const glm::vec3& position, const glm::vec3& color);
    PointLight(const glm::vec3& position);

    glm::vec3 GetPosition() const;
    glm::vec3 GetColor() const;

    void SetPosition(const glm::vec3& position);
    void SetColor(const glm::vec3& color);

private:
    glm::vec3 mPosition, mColor;
};