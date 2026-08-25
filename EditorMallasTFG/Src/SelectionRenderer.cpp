#include "SelectionRenderer.h"

#include "Mesh.h"

SelectionRenderer::SelectionRenderer() {

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Reservamos espacio para una cara (3 vertices, 9 floats -> posicion)
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 9, nullptr, GL_DYNAMIC_DRAW);

    // layout(location = 0) -> vec3 position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); // !! hace falta?
    glBindVertexArray(0);
}

SelectionRenderer::~SelectionRenderer() {
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

void SelectionRenderer::drawPoints(const std::vector<glm::vec3>& vertices) {

    if (vertices.empty())
        return;

    glPointSize(5.0f);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_DYNAMIC_DRAW);

    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(vertices.size()));
}

void SelectionRenderer::drawLines(const std::vector<glm::vec3>& vertices) {

    if (vertices.empty())
        return;

    glLineWidth(5.0f);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_DYNAMIC_DRAW);

    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(vertices.size()));
}

void SelectionRenderer::drawTriangles(const std::vector<glm::vec3>& vertices) {

    if (vertices.empty())
        return;

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_DYNAMIC_DRAW);

    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()));    
}