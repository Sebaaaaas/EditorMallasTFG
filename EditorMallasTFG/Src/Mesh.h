#pragma once

#include <vector>

#include <string>

using namespace std;

class Shader;


struct Vertex
{
    float Position[3];
    float Normal[3];
    float TexCoords[2];
};

class Mesh
{
private:
    //  render data
    unsigned int VAO, VBO, EBO;

    void setupMesh();

public:
    // mesh data
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    //vector<Texture>      textures;

    Mesh(vector<Vertex> vertices, vector<unsigned int> indices/*, vector<Texture> textures*/);
    ~Mesh();
    void Draw(Shader& shader);

    static Mesh LoadOBJ(const std::string& path);
    //void setupMesh();
};

