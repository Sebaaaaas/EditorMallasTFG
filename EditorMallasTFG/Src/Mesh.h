#pragma once

#include <vector>
#include <string>

class Shader;


struct Vertex
{
    float Position[3];
    float Normal[3];
    //float TexCoords[2];
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

    int selectedVertex = -1;
};

