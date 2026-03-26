#include "Mesh.h"

#include <iostream>
#include <glad/gl.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "Shader.h"

using namespace std;


    //GLuint vao = 0;
    //glGenVertexArrays(1, &vao);
    //glBindVertexArray(vao);
    //glEnableVertexAttribArray(0);
    //glBindBuffer(GL_ARRAY_BUFFER, vbo);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL); // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);, 0 es el "stride", o espacio entre valores, que como
    //                                                          // tenemos "tightly-packed", ponerlo a 0 se puede hacer y OpenGL sabe como separarlos, en caso contrario indicar con, por ejemplo,
    //                                                          // 3 * sizeof(float) (que tambien se podria poner en este caso)


Mesh::Mesh(vector<Vertex> vertices_, vector<unsigned int> indices_)
{
    vertices = vertices_;
    indices = indices_;

    setupMesh();
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Mesh::draw(Shader& shader)
{
    shader.use();

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES,
        static_cast<GLsizei>(indices.size()),
        GL_UNSIGNED_INT,
        0);
    glBindVertexArray(0);
}

Mesh Mesh::loadOBJ(const std::string& path)
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
        throw std::runtime_error("Fallo en la carga de OBJ");

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

            /*if (!attrib.texcoords.empty())
            {
                vertex.TexCoords[0] = attrib.texcoords[2 * index.texcoord_index + 0];
                vertex.TexCoords[1] = attrib.texcoords[2 * index.texcoord_index + 1];
            }*/

            vertices.push_back(vertex);
            indices.push_back(indices.size());
        }
    }

    return Mesh(vertices, indices/*, {}*/);
}

void Mesh::saveOBJ(const std::string& path)
{
    std::ofstream file(path);

    if (!file.is_open())
        return;

    // Write vertices
    for (const auto& v : vertices)
    {
        file << "v "
            << v.Position[0] << " "
            << v.Position[1] << " "
            << v.Position[2] << "\n";
    }

    // Write faces (indices)
    for (size_t i = 0; i < indices.size(); i += 3)
    {
        file << "f "
            << indices[i] + 1 << " "
            << indices[i + 1] + 1 << " "
            << indices[i + 2] + 1 << "\n";
    }

    file.close();
}

void Mesh::setupMesh()
{
    // Creacion de buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);


    // Asignacion de buffers
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_DYNAMIC_DRAW); // Copia los vertices a la memoria del buffer, el ultimo parametro: 
                                                                                                     // GL_STREAM_DRAW: the data is set only once and used by the GPU at most a few times. 
                                                                                                     // GL_STATIC_DRAW: the data is set only once and used many times. 
                                                                                                     // GL_DYNAMIC_DRAW : the data is changed a lot and used many times.

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_DYNAMIC_DRAW);

    // Indicamos cómo se deben leer los VBO a base de "rellenar" la info del VAO
    // Posiciones
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, // indice de atributo (igual que en el vertex shader - layout (location = 0) in), numero de componentes (x, y, z), que tipo es, deberia OpenGL normalizar los valores?
        sizeof(Vertex),                             // stride: tamaño total de un vertice
        (void*)0);                                  // offset: donde comienza este atributo
    
    // Normales         
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, Normal));

    // Coordenadas de textura - no usado actualmente
    /*glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, TexCoords));*/

    glBindVertexArray(0);
}

void Mesh::updateVertex(int index)
{
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferSubData(
        GL_ARRAY_BUFFER,
        index * sizeof(Vertex),
        sizeof(Vertex),
        &vertices[index]
    );

}