#pragma once

#include <vector>
#include <string>

#include <glm.hpp>

class Shader;


struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    //glm::vec3 TexCoords;

    bool operator==(const Vertex& other) const
    {
        return Position == other.Position && Normal == other.Normal;
    }
};

// Con esto mapeamos la informacion de cada Vertex a un rango determinado - https://en.cppreference.com/w/cpp/utility/hash.html
//                                                                        - https://en.wikipedia.org/wiki/Hash_function
template<>
struct std::hash<Vertex>
{
    size_t operator()(const Vertex& v) const
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

class Mesh
{
private:

    // Mas info en https://medium.com/@deyan.sirakov2006/the-definitive-guide-to-opengl-vbos-vaos-and-ebos-6193ab13ccc5
    unsigned int VAO, VBO, EBO; // Vertex Array Objects - Ayuda a la GPU a interpretar los valores que tiene el buffer VBO, como si fuera un manual de instrucciones para la GPU
                                // Vertex Buffer Objects - podemos guardar un gran numero de vertices en la memoria de la GPU con toda su info(pos, colores, normales, etc)
                                // En lugar de mandarlo continuamente desde la CPU, lo cargamos una vez en la memoria de GPU, para que lo pueda usar mas rapido
                                // Element Buffer Objects (o Index Buffer) - Si quieres reutilizar info en lugar de repetirte, guarda indices que hacen referencia a vertices en el VBO

    void setupMesh();

public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    //std::vector<Texture> textures;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices/*, std::vector<Texture> textures*/);
    ~Mesh();
    void draw(Shader& shader);

    static Mesh loadOBJ(const std::string& path);

    void saveOBJ(const std::string& path);

    void updateVertex(int index);
};

