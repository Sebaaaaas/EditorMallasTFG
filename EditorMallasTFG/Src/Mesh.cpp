#include "Mesh.h"

#include <iostream>
#include <glad/gl.h>

#define TINYOBJLOADER_IMPLEMENTATION // tener esto en mas de un archivo causara errores de linker segun entiendo
#include "tiny_obj_loader.h"

#include "Shader.h"

Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices)
{
    this->vertices = vertices;
    this->indices = indices;

    setupMesh();
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Mesh::Draw(Shader& shader)
{
    shader.use();

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES,
        static_cast<GLsizei>(indices.size()),
        GL_UNSIGNED_INT,
        0);
    glBindVertexArray(0);
}

Mesh Mesh::LoadOBJ(const std::string& path)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    bool success = tinyobj::LoadObj(&attrib, &shapes, &materials,
        &warn, &err, path.c_str());

    if (!warn.empty())
        std::cout << warn << std::endl;

    if (!err.empty())
        std::cerr << err << std::endl;

    if (!success)
        throw std::runtime_error("Failed to load OBJ");

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    for (const auto& shape : shapes)
    {
        for (const auto& index : shape.mesh.indices)
        {
            Vertex vertex{};

            vertex.Position[0] = attrib.vertices[3 * index.vertex_index + 0];
            vertex.Position[1] = attrib.vertices[3 * index.vertex_index + 1];
            vertex.Position[2] = attrib.vertices[3 * index.vertex_index + 2];

            if (!attrib.normals.empty())
            {
                vertex.Normal[0] = attrib.normals[3 * index.normal_index + 0];
                vertex.Normal[1] = attrib.normals[3 * index.normal_index + 1];
                vertex.Normal[2] = attrib.normals[3 * index.normal_index + 2];
            }

            if (!attrib.texcoords.empty())
            {
                vertex.TexCoords[0] = attrib.texcoords[2 * index.texcoord_index + 0];
                vertex.TexCoords[1] = attrib.texcoords[2 * index.texcoord_index + 1];
            }

            vertices.push_back(vertex);
            indices.push_back(indices.size());
        }
    }

    return Mesh(vertices, indices/*, {}*/);
}

void Mesh::setupMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
        vertices.size() * sizeof(Vertex),
        &vertices[0],
        GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(unsigned int),
        &indices[0],
        GL_STATIC_DRAW);

    // Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex),
        (void*)0);

    // Normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, Normal));

    // TexCoords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);
}