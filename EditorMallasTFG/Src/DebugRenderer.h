#pragma once

#include <glm/glm.hpp>
#include <glad/gl.h>

class Mesh;
struct Edge;
struct Polygon;

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
    void drawEdge(const Mesh& mesh, const Edge& edge);
    void drawPolygon(const Mesh& mesh, const Polygon& polygon);
};