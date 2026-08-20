#pragma once

#include <unordered_set>
#include <string>
#include <vector>

#include <glm/glm.hpp>

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    //glm::vec2 TexCoords;

    bool operator==(const Vertex& other) const
    {
        return Position == other.Position && Normal == other.Normal;
    }
};

//struct Texture {
//    unsigned int id;
//    std::string type;
//};

// Con esto mapeamos cada Vertex a un rango determinado - https://en.cppreference.com/w/cpp/utility/hash.html
//                                                      - https://en.wikipedia.org/wiki/Hash_function
template<>
struct std::hash<Vertex>
{
    size_t operator()(const Vertex& v) const noexcept
    {
        size_t h1 = hash<float>()(v.Position.x);
        size_t h2 = hash<float>()(v.Position.y);
        size_t h3 = hash<float>()(v.Position.z);

        size_t h4 = hash<float>()(v.Normal.x);
        size_t h5 = hash<float>()(v.Normal.y);
        size_t h6 = hash<float>()(v.Normal.z);

        // Combinamos los hashes
        return (((((h1 ^ (h2 << 1)) ^ (h3 << 1)) ^ (h4 << 1)) ^ (h5 << 1)) ^ (h6 << 1));
    }
};

struct Edge { // !! Nota: para extrusion probablemente necesario incorporar a que dos poligonos pertenece cada arista
    unsigned int v0;
    unsigned int v1;
};

// Estructura que almacena caras en lugar de triangulos
struct Polygon {
    std::vector<unsigned int> vertices;
};

class Mesh
{
private:

    // De utilidad: https://www.opengl-tutorial.org/intermediate-tutorials/tutorial-9-vbo-indexing/
    unsigned int VBO, EBO;      // Vertex Buffer Objects - podemos guardar un gran numero de vertices en la memoria de la GPU con toda su info(pos, colores, normales, etc)
                                // En lugar de mandarlo continuamente desde la CPU, lo cargamos una vez en la memoria de GPU, para que lo pueda usar mas rapido
                                // Element Buffer Objects (o Index Buffer) - Si quieres reutilizar info en lugar de repetirte, guarda indices que hacen referencia a vertices en el VBO (deduplicacion)

    unsigned int VAO; // Vertex Array Objects - Ayuda a la GPU a interpretar los valores que tiene el buffer VBO, como si fuera un manual de instrucciones para la GPU

    void setupMesh();

    // Despues de cargar la malla, usamos esta funcion para crear todos los pares de vertices que conforman lados
    void generateEdges();

public:

    std::vector<Vertex> vertices; // Almacena todos los vertices de la malla. Son vertices de renderizado(en un cubo deben salir 24, a 4 por cara)
    std::vector<unsigned int> indices; // Aqui se almacenan los vertices triangulados que se enviaran a la GPU, debe ser actualizado al crear y destruir vertices o no se pintara
                                       // correctamente
    //std::vector<Texture> textures;

    // Mapeado para mover vertices en misma posicion
    std::vector<std::vector<unsigned int>> vertexGroups; // Grupos de vertices con la misma posicion
    std::vector<unsigned int> vertexToGroup;             // Mapeado de vertice a su grupo


    std::vector<Edge> edges; // !! igual no deberian ser publicos estos
    std::vector<Polygon> polygons;

    Mesh(const std::string& path);
    ~Mesh();

    void draw();

    // Cargamos con tiniobjloader  la malla en formato obj con el nombre "path"
    void loadOBJ(const std::string& path);

    // !! arreglar, lo guardamos triangulado
    void saveOBJ(const std::string& path);
    
    // Actualiza las normales de TODA la malla >> !! posible mejora es que solo recalcule normales que cambien
    void recalculateNormals();

    // Manda a la GPU la informacion de toda la malla para que se vean cambios en pantalla
    void updateAllVertices();        

    unsigned int addVertex(const Vertex& vertex);

    unsigned int addPolygon(const Polygon& polygon);

    glm::vec3 polygonNormal(unsigned int polygonIndex);

    // Genera indices a partir de los poligonos que conforman la malla
    void generateIndices();
    void updateIndices();

    // Cuando hay creacion o destruccion de elementos de la malla, debemos actualizar todos los vectores a los que afecte el cambio
    void rebuildTopology();

    // Encuentra y elimina vertices que se quedan sueltos tras borrar geometria
    void removeLooseVertices();
    
    void deletePolygons(const std::unordered_set<unsigned int>& polygonIndices);
    // Para borrar grupos geometricos de vertices
    void deleteVertexGroups(const std::unordered_set<unsigned int>& groups);
};

