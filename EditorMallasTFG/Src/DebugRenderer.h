#pragma once

#include <glm.hpp>

class DebugRenderer
{
public:
    void drawPoint(const glm::vec3& pos);
    void drawLine(const glm::vec3& a, const glm::vec3& b);
};