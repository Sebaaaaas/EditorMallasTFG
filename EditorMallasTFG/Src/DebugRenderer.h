#pragma once

#include <glm.hpp>
#include <glad/gl.h>

class DebugRenderer
{
private:
    GLuint vao, vbo;

public:

    DebugRenderer();
    ~DebugRenderer();

    void drawPoint(const glm::vec3& pos);
    void drawLine(const glm::vec3& a, const glm::vec3& b);
    void drawTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);
};