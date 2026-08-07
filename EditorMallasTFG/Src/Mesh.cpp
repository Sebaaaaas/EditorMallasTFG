#include "Mesh.h"

#include <unordered_map>
#include <iostream>
#include <set>

#include <glad/gl.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include <algorithm>


Mesh::Mesh(const std::string& path) {
    
    EBO = 0;
    VBO = 0;
    VAO = 0;

    loadOBJ(path);

    generateEdges();

    setupMesh();
}

Mesh::~Mesh() {

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Mesh::draw() {

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

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

// https://en.wikipedia.org/wiki/Wavefront_.obj_file
void Mesh::loadOBJ(const std::string& path) {

    // Cargamos malla con tinyobjloader
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;
    
    bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str(), NULL, false);
    
    if (!warn.empty())
        std::cout << warn << std::endl;
    if (!err.empty())
        std::cerr << err << std::endl;
    if (!success)
        throw std::runtime_error("Fallo en la carga de OBJ");
 

    // Almacenamos la informacion relevante en nuestra clase Mesh
    
    // Utilizamos uniqueVertices para deduplicacion
    std::unordered_map<Vertex, unsigned int> uniqueVertices;
    std::unordered_map<int, unsigned int> positionIndexToGroup;

    // Por cada malla separada(por ejemplo, dos cubos que no comparten topologia)
    for (const tinyobj::shape_t shape : shapes) {
        
        int indexOffset = 0;

        // Iteramos sobre las caras de cada malla
        for (int face = 0; face < shape.mesh.num_face_vertices.size(); ++face) {

            int faceVertices = shape.mesh.num_face_vertices[face];

            // Construimos y almacenamos nuestros poligonos
            Polygon polygon;

            for (int v = 0; v < faceVertices; ++v) {

                const tinyobj::index_t& index = shape.mesh.indices[indexOffset + v];

                Vertex vertex{};

                vertex.Position = glm::vec3(attrib.vertices[3 * index.vertex_index + 0], 
                                            attrib.vertices[3 * index.vertex_index + 1], 
                                            attrib.vertices[3 * index.vertex_index + 2]);

                if (!attrib.normals.empty() && index.normal_index >= 0) {

                    vertex.Normal = glm::vec3(
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2]);
                }

                // Asignamos indices
                unsigned int vertexIndex;

                // Deduplicacion de vertices
                auto it = uniqueVertices.find(vertex);

                if (it == uniqueVertices.end()) { // Si no existe otro vértice con la misma posicion y normal(usa el hash para comparar)
                        
                    vertexIndex = vertices.size();
                    uniqueVertices[vertex] = vertexIndex;

                    vertices.push_back(vertex);

                    unsigned int groupIndex;

                    // Asignamos un grupo por cada posicion unica para vertices
                    auto it2 = positionIndexToGroup.find(index.vertex_index);
                    if (it2 == positionIndexToGroup.end()) { // No encontrado antes, creamos nuevo grupo

                        groupIndex = vertexGroups.size();

                        positionIndexToGroup[index.vertex_index] = groupIndex;

                        // Creamos un grupo nuevo vacio
                        vertexGroups.push_back({});
                    }
                    else {
                        groupIndex = it2->second;
                    }

                    vertexGroups[groupIndex].push_back(vertexIndex);
                    vertexToGroup.push_back(groupIndex);
                }
                else { // Ya existia el vertice previamente, no lo duplicamos sino que usamos el ya existente
                    vertexIndex = it->second;
                }

                polygon.vertices.push_back(vertexIndex);
            }

            polygons.push_back(polygon);

            // El renderizado sigue usando triangulos, asi que los guardamos como tal
            for (int i = 1; i < faceVertices - 1; ++i) {

                indices.push_back(polygon.vertices[0]);
                indices.push_back(polygon.vertices[i]);
                indices.push_back(polygon.vertices[i + 1]);
            }

            indexOffset += faceVertices;
        }

    }

    std::cout << "Indices: " << indices.size() << std::endl;
    std::cout << "Vertices: " << vertices.size() << std::endl;
    std::cout << "VertexGroups: " << vertexGroups.size() << std::endl;

}

void Mesh::saveOBJ(const std::string& path) {

    std::ofstream file(path);

    if (!file.is_open())
        return;

    // Guardamos vertices, solo uno por cada vertexGroup, para que no pongamos de más
    std::vector<unsigned int> groupToObjIndex(vertexGroups.size());

    unsigned int objIndex = 1;

    for (size_t group = 0; group < vertexGroups.size(); ++group) {

        unsigned int renderVertex = vertexGroups[group][0];

        const glm::vec3& p = vertices[renderVertex].Position;

        file << "v "
            << p.x << " "
            << p.y << " "
            << p.z << "\n";

        groupToObjIndex[group] = objIndex++;
    }

    // Guardamos normales de vertices
    for (const auto& v : vertices) {

        file << "vn "
            << v.Normal.x << " "
            << v.Normal.y << " "
            << v.Normal.z << "\n";
    }

    // Guardamos indices
    for (size_t i = 0; i < indices.size(); i += 3) {

        file << "f ";

        for (int j = 0; j < 3; ++j)
        {
            unsigned int render = indices[i + j];

            unsigned int group = vertexToGroup[render];

            file << groupToObjIndex[group]
                << "//"
                << render + 1
                << " ";
        }

        file << "\n";
    }

    file.close();
}

// https://learnopengl.com/Model-Loading/Mesh
void Mesh::setupMesh() {

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

    // Indicamos como se deben leer los VBO a base de "rellenar" la info del VAO
    // Posiciones
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, // indice de atributo (igual que en el vertex shader - layout (location = 0) in), numero de componentes (x, y, z), que tipo es, deberia OpenGL normalizar los valores?
        sizeof(Vertex),                             // stride: tamanio total de un vertice
        (void*)0);                                  // offset: donde comienza este atributo
    
    // Normales         
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

    // Coordenadas de textura
    /*glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));*/


    glBindVertexArray(0);
}

void Mesh::recalculateNormals() {

    // Reset normals
    for (auto& v : vertices)
        v.Normal = glm::vec3(0.0f);

    // Accumulate face normals
    for (size_t i = 0; i < indices.size(); i += 3)
    {
        Vertex& v0 = vertices[indices[i]];
        Vertex& v1 = vertices[indices[i + 1]];
        Vertex& v2 = vertices[indices[i + 2]];

        glm::vec3 edge1 = v1.Position - v0.Position;
        glm::vec3 edge2 = v2.Position - v0.Position;

        glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

        v0.Normal += normal;
        v1.Normal += normal;
        v2.Normal += normal;
    }

    // Normalize
    for (auto& v : vertices)
        v.Normal = glm::normalize(v.Normal);
}

void Mesh::updateAllVertices() {

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());
}

void Mesh::generateEdges() {

    // Por si acaso quedan aristas, pero habitualmente deberia de haberse borrado la malla antigua y generado una nueva
    edges.clear();

    std::set<std::pair<unsigned int, unsigned int>> uniqueEdges;

    for (const Polygon& polygon : polygons) {

        size_t vertexCount = polygon.vertices.size();

        // Si hubiera un poligono que fuera únicamente un vértice
        if (vertexCount < 2)
            continue;

        // Recorremos los vertices del poligono, creando aristas
        for (int i = 0; i < vertexCount; ++i) {

            unsigned int a = polygon.vertices[i];
            unsigned int b = polygon.vertices[(i + 1) % vertexCount]; // Modulo permite evitar tener que poner un caso especial de if para unir el vertice vertexCount-1 al 0

            // Devuelve los dos valores de forma ordenada
            std::pair edgePair = std::minmax(a, b);

            if (uniqueEdges.insert(edgePair).second) { // Si ya existia en el conjunto, devolvera false
                edges.push_back({edgePair.first, edgePair.second});
            }
        }
    }

    std::cout << "Edges: " << edges.size() << std::endl;

}
