#pragma once

#include <glm.hpp>
#include <glad/gl.h>

class DebugRenderer
{
private:
    GLuint vao, vbo;

public:
    void drawPoint(const glm::vec3& pos);
    void drawLine(const glm::vec3& a, const glm::vec3& b);
};