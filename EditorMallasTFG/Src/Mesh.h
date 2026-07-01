#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#include <glm.hpp>

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

struct Edge {
    unsigned int v0;
    unsigned int v1;
};

struct Face {
    unsigned int v0;
    unsigned int v1;
    unsigned int v2;
};

class Mesh
{
private:

    // De utilidad: https://www.opengl-tutorial.org/intermediate-tutorials/tutorial-9-vbo-indexing/
    unsigned int VBO, EBO;      // Vertex Buffer Objects - podemos guardar un gran numero de vertices en la memoria de la GPU con toda su info(pos, colores, normales, etc)
                                // En lugar de mandarlo continuamente desde la CPU, lo cargamos una vez en la memoria de GPU, para que lo pueda usar mas rapido
                                // Element Buffer Objects (o Index Buffer) - Si quieres reutilizar info en lugar de repetirte, guarda indices que hacen referencia a vertices en el VBO (deduplicacion)

    void setupMesh();

    // Despues de cargar la malla, usamos esta funcion para crear todos los pares de vertices que conforman lados
    void generateEdges(); // !! mejorable

    void generateFaces();

public:
    std::vector<Vertex> vertices; // Vertices de renderizado, no total de vertices(en un cubo deben salir 24, ya que al no repetir, quedamos con 4 por cara, que conservan normales)
    std::vector<unsigned int> indices;
    //std::vector<Texture> textures;

    // Mapeado para mover vertices en misma posicion
    std::vector<std::vector<unsigned int>> vertexGroups; // Grupos de vertices con la misma posicion
    std::vector<unsigned int> vertexToGroup;             // Mapeado de vertice a su grupo

    unsigned int VAO; // Vertex Array Objects - Ayuda a la GPU a interpretar los valores que tiene el buffer VBO, como si fuera un manual de instrucciones para la GPU

    std::vector<Edge> edges;
    
    std::vector<Face> faces;


    Mesh(std::string path);
    ~Mesh();

    void draw();

    // Cargamos con tiniobjloader  la malla en formato obj con el nombre "path", que debe encontrarse en la carpeta Bin/Assets
    void loadOBJ(const std::string& path, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);

    void saveOBJ(const std::string& path);
    
    // Actualiza las normales de TODA la malla >> !! posible mejora es que solo recalcule normales que cambien
    void recalculateNormals();

    // NO USAR: SI SE EDITA UNA MALLA GRANDE, EN VEZ DE MUCHAS LLAMADAS A ESTA FUNCION(MUCHAS LLAMADAS A GPU) LLAMAR UNA SOLA VEZ A "updateAllVertices"
    // Manda informacion a la GPU para que actualice la posicion de un vertice.
    void updateVertex(int index);

    // Manda a la GPU la informacion de toda la malla para que se vean cambios en pantalla
    void updateAllVertices();
};

