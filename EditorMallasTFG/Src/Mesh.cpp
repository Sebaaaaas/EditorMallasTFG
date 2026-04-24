#include "Mesh.h"

#include <iostream>
#include <glad/gl.h>

#include "Shader.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"


Mesh::Mesh(std::string path)
{
    loadOBJ(path, vertices, indices);
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
    //unsigned int diffuseNr = 1;
    //unsigned int specularNr = 1;
    //for (unsigned int i = 0; i < textures.size(); i++)
    //{
    //    glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
    //    // retrieve texture number (the N in diffuse_textureN)
    //    std::string number;
    //    std::string name = textures[i].type;
    //    if (name == "texture_diffuse")
    //        number = std::to_string(diffuseNr++);
    //    else if (name == "texture_specular")
    //        number = std::to_string(specularNr++);

    //    shader.setInt(("material." + name + number).c_str(), i);
    //    glBindTexture(GL_TEXTURE_2D, textures[i].id);
    //}
    //glActiveTexture(GL_TEXTURE0);

    shader.use();

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

//void Mesh::loadOBJ(const std::string& path, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices)
//{
//    tinyobj::attrib_t attrib;
//    std::vector<tinyobj::shape_t> shapes;
//    std::vector<tinyobj::material_t> materials;
//    std::string warn, err;
//    
//    bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str());
//    
//    if (!warn.empty()) std::cout << warn << std::endl;
//    if (!err.empty()) std::cerr << err << std::endl;
//    if (!success) throw std::runtime_error("Fallo en la carga de OBJ");
// 
//    std::unordered_map<Vertex, unsigned int> uniqueVertices;
//
//    std::cout << shapes.size() << std::endl;
//    for (const tinyobj::shape_t shape : shapes)
//    {
//        std::cout << shape.mesh.indices.size() << std::endl;
//        for (const auto& index : shape.mesh.indices)
//        {
//            Vertex vertex{};
//
//            vertex.Position = glm::vec3(
//                attrib.vertices[3 * index.vertex_index + 0],
//                attrib.vertices[3 * index.vertex_index + 1],
//                attrib.vertices[3 * index.vertex_index + 2]
//            );
//
//            if (!attrib.normals.empty() && index.normal_index >= 0)
//            {
//                vertex.Normal = glm::vec3(
//                    attrib.normals[3 * index.normal_index + 0],
//                    attrib.normals[3 * index.normal_index + 1],
//                    attrib.normals[3 * index.normal_index + 2]
//                );
//            }
//
//            if (uniqueVertices.count(vertex) == 0)
//            {
//                uniqueVertices[vertex] = static_cast<unsigned int>(vertices.size());
//                vertices.push_back(vertex);
//            }
//
//            indices.push_back(uniqueVertices[vertex]);
//
//        }
//    }
//    std::cout << uniqueVertices.size() << std::endl;
//
//}

void Mesh::loadOBJ(const std::string& path, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str());

    if (!warn.empty()) std::cout << warn << std::endl;
    if (!err.empty()) std::cerr << err << std::endl;
    if (!success) throw std::runtime_error("Fallo en la carga de OBJ");


    std::unordered_map<int, unsigned int> positionIndexToGroup;

    for (const auto& shape : shapes)
    {
        for (const auto& index : shape.mesh.indices)
        {
            Vertex vertex{};

            // Position
            vertex.Position = glm::vec3(
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            );

            // Normal
            if (!attrib.normals.empty() && index.normal_index >= 0)
            {
                vertex.Normal = glm::vec3(
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2]
                );
            }
                        
            unsigned int vertexIndex = vertices.size(); // Siguiente espacio disponible
            vertices.push_back(vertex);
            indices.push_back(vertexIndex);

            // Asignamos al grupo que comparta posicion del vertice
            unsigned int groupIndex;

            if (positionIndexToGroup.count(index.vertex_index) == 0) // Si no habia ningun vertice previo compartiendo posicion
            {
                groupIndex = vertexGroups.size();
                positionIndexToGroup[index.vertex_index] = groupIndex;
                vertexGroups.push_back({});
            }
            else
            {
                groupIndex = positionIndexToGroup[index.vertex_index];
            }

            vertexGroups[groupIndex].push_back(vertexIndex);
            vertexToGroup.push_back(groupIndex);
        }
    }
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

// https://learnopengl.com/Model-Loading/Mesh
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
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

    // Coordenadas de textura
    /*glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));*/


    glBindVertexArray(0);
}

void Mesh::updateVertex(int index)
{
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferSubData(GL_ARRAY_BUFFER, index * sizeof(Vertex), sizeof(Vertex), &vertices[index]);

}