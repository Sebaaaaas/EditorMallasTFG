#include "DebugRenderer.h"

//#include <glad/gl.h>

void DebugRenderer::drawPoint(const glm::vec3& pos)
{
    glPointSize(25.0f);

    float v[3] = { pos.x, pos.y, pos.z };

    //GLuint vao, vbo;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    glDrawArrays(GL_POINTS, 0, 1);

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

void DebugRenderer::drawLine(const glm::vec3& a, const glm::vec3& b)
{
    float vertices[6] = {
        a.x, a.y, a.z,
        b.x, b.y, b.z
    };

    //GLuint vao, vbo;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glDrawArrays(GL_LINES, 0, 2);

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}
