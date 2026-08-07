#include "DebugRenderer.h"

#include "Mesh.h"

DebugRenderer::DebugRenderer()
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Reservamos espacio para una linea (2 puntos, 6 floats -> posicion y normales) !! ahora es una cara, ha subido de 6 a 9 floats
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 9, nullptr, GL_DYNAMIC_DRAW);

    // layout(location = 0) -> vec3 position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); // !! hace falta?
    glBindVertexArray(0);
}

DebugRenderer::~DebugRenderer()
{
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

void DebugRenderer::drawPoint(const glm::vec3& pos)
{
    float vertex[3] = {
        pos.x, pos.y, pos.z
    };

    glPointSize(5.0f);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Actualizamos solo la posicion
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertex), vertex);

    glDrawArrays(GL_POINTS, 0, 1);

    // Volvemos a estado base
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void DebugRenderer::drawLine(const glm::vec3& a, const glm::vec3& b)
{
    float vertices[6] = {
        a.x, a.y, a.z,
        b.x, b.y, b.z
    };

    glLineWidth(5.0f);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Actualizamos con los nuevos valores
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    glDrawArrays(GL_LINES, 0, 2);

    // Volvemos a estado base
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void DebugRenderer::drawTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {

    float vertices[9] =
    {
        a.x,a.y,a.z,
        b.x,b.y,b.z,
        c.x,c.y,c.z
    };

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        sizeof(vertices),
        vertices);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void DebugRenderer::drawEdge(const Mesh& mesh, const Edge& edge) {
    glm::vec3 a = mesh.vertices[edge.v0].Position;
    glm::vec3 b = mesh.vertices[edge.v1].Position;

    drawLine(a, b);
}

void DebugRenderer::drawPolygon(const Mesh& mesh, const Polygon& polygon) {
    
    for (size_t i = 1; i + 1 < polygon.vertices.size(); ++i) {

        glm::vec3 a = mesh.vertices[polygon.vertices[0]].Position;
        glm::vec3 b = mesh.vertices[polygon.vertices[i]].Position;
        glm::vec3 c = mesh.vertices[polygon.vertices[i + 1]].Position;

        drawTriangle(a, b, c);
    }
}
