#include "PointLight.h"

PointLight::PointLight(const glm::vec3& position, const glm::vec3& color) :
mPosition(position), mColor(color)
{}

PointLight::PointLight(const glm::vec3& position) :
mPosition(position), mColor(glm::vec3(1.f, 1.f, 1.f))
{}

glm::vec3 PointLight::GetPosition() const
{
    return mPosition;
}

glm::vec3 PointLight::GetColor() const
{
    return mColor;
}

void PointLight::SetPosition(const glm::vec3& position)
{
    mPosition = position;
}

void PointLight::SetColor(const glm::vec3& color)
{
    mColor = color;
}