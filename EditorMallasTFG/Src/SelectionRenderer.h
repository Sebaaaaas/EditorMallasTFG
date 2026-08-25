#pragma once

#include <glm/glm.hpp>
#include <glad/gl.h>

class Mesh;
struct Edge;
struct Polygon;

class SelectionRenderer
{
private:
    GLuint vao, vbo;

public:

    SelectionRenderer();
    ~SelectionRenderer();

    void drawPoints(const std::vector<glm::vec3>& vertices);
    void drawLines(const std::vector<glm::vec3>& vertices);
    void drawTriangles(const std::vector<glm::vec3>& vertices);
};