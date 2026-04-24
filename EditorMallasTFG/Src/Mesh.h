#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#include <glm.hpp>

class Shader;

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    //glm::vec2 TexCoords;

    /*bool operator==(const Vertex& other) const
    {
        return Position == other.Position && Normal == other.Normal;
    }*/
};

//struct Texture {
//    unsigned int id;
//    std::string type;
//};

// Con esto mapeamos cada Vertex a un rango determinado - https://en.cppreference.com/w/cpp/utility/hash.html
//                                                      - https://en.wikipedia.org/wiki/Hash_function
//template<>
//struct std::hash<Vertex>
//{
//    size_t operator()(const Vertex& v) const noexcept
//    {
//        size_t h1 = hash<float>()(v.Position.x);
//        size_t h2 = hash<float>()(v.Position.y);
//        size_t h3 = hash<float>()(v.Position.z);
//
//        size_t h4 = hash<float>()(v.Normal.x);
//        size_t h5 = hash<float>()(v.Normal.y);
//        size_t h6 = hash<float>()(v.Normal.z);
//
//        // Combinamos los hashes
//        return (((((h1 ^ (h2 << 1)) ^ (h3 << 1)) ^ (h4 << 1)) ^ (h5 << 1)) ^ (h6 << 1));
//    }
//};

class Mesh
{
private:

    // De utilidad: https://www.opengl-tutorial.org/intermediate-tutorials/tutorial-9-vbo-indexing/
    unsigned int VBO, EBO;      // Vertex Buffer Objects - podemos guardar un gran numero de vertices en la memoria de la GPU con toda su info(pos, colores, normales, etc)
                                // En lugar de mandarlo continuamente desde la CPU, lo cargamos una vez en la memoria de GPU, para que lo pueda usar mas rapido
                                // Element Buffer Objects (o Index Buffer) - Si quieres reutilizar info en lugar de repetirte, guarda indices que hacen referencia a vertices en el VBO (deduplicacion)

    void setupMesh();

public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    //std::vector<Texture> textures;

    // Mapeado para mover vertices en misma posicion
    std::vector<std::vector<unsigned int>> vertexGroups; // Grupos de vertices con la misma posicion
    std::vector<unsigned int> vertexToGroup;             // vertex -> group

    unsigned int VAO; // Vertex Array Objects - Ayuda a la GPU a interpretar los valores que tiene el buffer VBO, como si fuera un manual de instrucciones para la GPU

    Mesh(std::string path);
    ~Mesh();

    void draw(Shader& shader);

    // Cargamos con tiniobjloader  la malla en formato obj con el nombre "path", que debe encontrarse en la carpeta Bin/Assets
    void loadOBJ(const std::string& path, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);

    void saveOBJ(const std::string& path);

    void updateVertex(int index);
};

