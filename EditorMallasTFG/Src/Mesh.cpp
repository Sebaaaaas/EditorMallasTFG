#include "Mesh.h"

#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <set>

#include <glad/gl.h>

#pragma warning(push)
#pragma warning(disable: 26495) // Deshabilitamos warnings de este archivo especificamente, ya que no es nuestro
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#pragma warning(pop)


Mesh::Mesh(const std::string& path) {
    
    EBO = 0;
    VBO = 0;
    VAO = 0;

    loadOBJ(path);

    generateIndices();
    generateEdges();

    // Se llama despues de generateIndices ya que envia el contenido de indices a la GPU
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
    glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
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

                // Asignacion de vertices a un mismo grupo si comparten posicion
                auto it = uniqueVertices.find(vertex);

                if (it == uniqueVertices.end()) { // Si no existe otro vertice con la misma posicion y normal(usa el hash para comparar)
                        
                    vertexIndex = (unsigned int)vertices.size();
                    uniqueVertices[vertex] = vertexIndex;

                    vertices.push_back(vertex);

                    unsigned int groupIndex;

                    // Asignamos un grupo por cada posicion unica para vertices
                    auto it2 = positionIndexToGroup.find(index.vertex_index);

                    if (it2 == positionIndexToGroup.end()) { // No encontrado antes, creamos nuevo grupo posicional

                        groupIndex = (unsigned int)vertexGroups.size();

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

            indexOffset += faceVertices;
        }

    }

    std::cout << "Vertices: " << vertices.size() << std::endl;
    std::cout << "VertexGroups: " << vertexGroups.size() << std::endl;

}

void Mesh::saveOBJ(const std::string& path) {

    std::ofstream file(path);

    if (!file.is_open())
        return;

    // Guardamos vertices, solo uno por cada vertexGroup, para que no pongamos de mas
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

    if (vertices.empty())
        return;

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

void Mesh::generateEdges() {

    // Por si acaso quedan aristas, pero habitualmente deberia de haberse borrado la malla antigua y generado una nueva
    edges.clear();

    std::set<std::pair<unsigned int, unsigned int>> uniqueEdges;

    for (const Polygon& polygon : polygons) {

        size_t vertexCount = polygon.vertices.size();

        // Si hubiera un poligono que fuera unicamente un vertice
        if (vertexCount < 2)
            continue;

        // Recorremos los vertices del poligono, creando arista
        for (int i = 0; i < vertexCount; ++i) {

            unsigned int a = polygon.vertices[i];
            unsigned int b = polygon.vertices[(i + 1) % vertexCount]; // Modulo permite evitar tener que poner un caso especial de if para unir el vertice vertexCount-1 al 0

            // Deduplicamos por grupo (posicion), no por indice de renderizado, ya que dos caras distintas pueden compartir arista con vertices
            // de renderizado distintos (normales distintas)
            unsigned int groupA = vertexToGroup[a];
            unsigned int groupB = vertexToGroup[b];

            // Devuelve los dos valores de forma ordenada
            std::pair groupPair = std::minmax(groupA, groupB);

            if (uniqueEdges.insert(groupPair).second) { // Si ya existia en el conjunto, devolvera false y no lo introducira
                // Seguimos guardando los indices de renderizado reales para poder dibujar la arista
                edges.push_back({ a, b });
            }
        }
    }

    std::cout << "Edges: " << edges.size() << std::endl;
}

void Mesh::recalculateNormals() {

    // Reseteamos normales
    for (auto& v : vertices)
        v.Normal = glm::vec3(0.0f);

    // Acumulamos normales de cada cara (de renderizado)
    for (size_t i = 0; i < indices.size(); i += 3) {

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

    // Normalizamos
    for (auto& v : vertices)
        v.Normal = glm::normalize(v.Normal);
}

void Mesh::updateAllVertices() {

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size() * sizeof(Vertex),
        vertices.data(),
        GL_DYNAMIC_DRAW
    );
}

unsigned int Mesh::addVertex(const Vertex& vertex) {

    unsigned int idx = (unsigned int)vertices.size();
    vertices.push_back(vertex);

    // Como al crear un nuevo vertice, asumimos que se quiere colocar en una posicion nueva en el espacio, le asignamos un nuevo grupo
    unsigned int groupIndex = (unsigned int)vertexGroups.size();
    vertexGroups.push_back({idx});
    vertexToGroup.push_back(groupIndex);

    return idx;
}

unsigned int Mesh::addPolygon(const Polygon& polygon) {
    
    unsigned int idx = (unsigned int)polygons.size();

    polygons.push_back(polygon);

    return idx;
}

glm::vec3 Mesh::polygonNormal(unsigned int polygonIndex) {

    const Polygon& polygon = polygons[polygonIndex];

    if (polygon.vertices.size() < 3)
        return glm::vec3(0.0f);

    // Nos basta con tres vertices del poligono para conocer la normal
    const glm::vec3& a = vertices[polygon.vertices[0]].Position;
    const glm::vec3& b = vertices[polygon.vertices[1]].Position;
    const glm::vec3& c = vertices[polygon.vertices[2]].Position;

    glm::vec3 ab = b - a;
    glm::vec3 ac = c - a;

    // Obtenemos el vector perpendicular al poligono
    glm::vec3 normal = glm::cross(ab, ac);

    float length = glm::length(normal);

    if (length < 0.00001f)
        return glm::vec3(0.0f);

    return glm::normalize(normal);
}

void Mesh::generateIndices() {

    indices.clear();

    for (const Polygon& polygon : polygons) {

        if (polygon.vertices.size() < 3)
            continue;

        unsigned int first = polygon.vertices[0];

        for (size_t i = 1; i + 1 < polygon.vertices.size(); ++i) {

            indices.push_back(first);
            indices.push_back(polygon.vertices[i]);
            indices.push_back(polygon.vertices[i + 1]);
        }
    }

    std::cout << "Indices: " << indices.size() << std::endl;
}

void Mesh::updateIndices() {

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(unsigned int),
        indices.data(),
        GL_DYNAMIC_DRAW
    );
}

void Mesh::rebuildTopology() {
    
    generateEdges();

    generateIndices();
    
    recalculateNormals();

    updateAllVertices();
    updateIndices();
}

void Mesh::removeLooseVertices() {

    // Asumimos inicialmente que todos los vertices estan sueltos
    std::vector<bool> referencedVertices(vertices.size(), false);

    int numUnreferencedVertices = (int)vertices.size();

    // Recorremos cada poligono. Todos los vertices dentro de un poligono no estan sueltos, asi que los marcamos como referenciados
    for (const Polygon& polygon : polygons)
        for (unsigned int vertex : polygon.vertices) {
            referencedVertices[vertex] = true;
            numUnreferencedVertices--;
        }

    // Si no habia vertices sueltos, podemos volver
    if (numUnreferencedVertices == 0)
        return;
    
    // Determinamos como resulta el nuevo vector Vertices tras la eliminacion de los sueltos. Remap almacena donde quedan los vertices tras la eliminacion
    // de los anteriores (donde antes iba el 3, ahora va el 4...)
    std::vector<int> remap(vertices.size(), -1);
    std::vector<Vertex> postDeletionVerticesVector;
    postDeletionVerticesVector.reserve(vertices.size());

    for (size_t i = 0; i < vertices.size(); ++i) {
        if (referencedVertices[i]) {
            remap[i] = (unsigned int)postDeletionVerticesVector.size();
            postDeletionVerticesVector.push_back(vertices[i]);
        }
    }

    // Copiamos los nuevos vertices post-eliminacion
    vertices = std::move(postDeletionVerticesVector);

    // Usamos el remapeado para ajustar los nuevos vertices de cada poligono
    for (Polygon& polygon : polygons)
        for (unsigned int& vertex : polygon.vertices)
            vertex = (unsigned int)remap[vertex];

    // Reconstruimos vertexGroups y vertexToGroups, ya que han cambiado, e incluso podria quedar eliminado un grupo completo de vertices
    std::vector<std::vector<unsigned int>> postDeletionVertexGroups;
    std::vector<unsigned int> newVertexToGroup(vertices.size());

    // Recorremos cada vertezGroup (grupo geometrico de vertices)
    for (const auto& oldGroup : vertexGroups) {

        // Buscamos aquellos casos donde queden vertices de renderizado restantes dentro de cada grupo
        std::vector<unsigned int> survivors;

        for (unsigned int oldIdx : oldGroup)
            if (remap[oldIdx] != -1)
                survivors.push_back((unsigned int)remap[oldIdx]);

        // Si no ha habido ninguno restante, no tenemos que reasignar ningun indice, simplemente no queda ninguno del grupo
        if (survivors.empty())
            continue;

        // Asignamos nuevo indice de grupo a los supertvivientes
        unsigned int newGroupIndex = (unsigned int)postDeletionVertexGroups.size();
        for (unsigned int newIdx : survivors)
            newVertexToGroup[newIdx] = newGroupIndex;

        postDeletionVertexGroups.push_back(std::move(survivors));
    }

    vertexGroups = std::move(postDeletionVertexGroups);
    vertexToGroup = std::move(newVertexToGroup);
}

void Mesh::deletePolygons(const std::unordered_set<unsigned int>& polygonIndices) {

    if (polygonIndices.empty())
        return;

    // Tenemos que eliminar de polygons aquellos encontrados, sin dejar huecos. Para ello creamos una copia donde almacenaremos los poligonos que
    // si permaneceran tras la destruccion, que copiaremos al final

    std::vector<Polygon> postDeletionPolygonVector;
    postDeletionPolygonVector.reserve(polygons.size());

    for (size_t i = 0; i < polygons.size(); ++i) {
        
        if (polygonIndices.count(i) == 0) // Si no esta en la lista de indices a borrar, lo volveremos a introducir
            postDeletionPolygonVector.push_back(polygons[i]);
    }

    polygons = std::move(postDeletionPolygonVector);

    // Debemos asegurar que no queden vertices sueltos
    removeLooseVertices();

    rebuildTopology();
}

void Mesh::deleteVertexGroups(const std::unordered_set<unsigned int>& groups) {

    if (groups.empty())
        return;

    // Borramos los poligonos que hagan uso del vertice geometrico seleccionado
    std::unordered_set<unsigned int> polygonsToDelete;

    for (size_t i = 0; i < polygons.size(); ++i) {

        for (unsigned int vertex : polygons[i].vertices) {

            if (groups.count(vertexToGroup[vertex]) > 0) {
                polygonsToDelete.insert((unsigned int)i);
                break;
            }
        }
    }

    deletePolygons(polygonsToDelete);
}
