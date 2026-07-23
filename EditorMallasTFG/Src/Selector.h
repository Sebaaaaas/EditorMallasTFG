#pragma once

#include <glm.hpp>

class Mesh;
class Camera;

enum class SelectionMode {
    Vertex,
    Edge,
    Face
};

// https://learnopengl.com/Getting-started/Transformations
class Selector
{
public:
    Selector();
    ~Selector();

    // Antes de llamar a pickXX debemos proyectar los vertices a la pantalla llamando a esta funcion
    void projectVerticesToScreen(const Mesh& mesh, int width, int height, const glm::mat4& view, const glm::mat4& projection);

    int pick(const Mesh& mesh, float mouseX, float mouseY, int width, int height, Camera* camera);
    
    void setSelectionMode(SelectionMode newMode);

    SelectionMode getSelectionMode() const;

private:

    SelectionMode currentSelectionMode;

    // Distancia minima para la seleccion de un vertice en pixeles !! convertir de float a int tanto esto como pos de vertices 2d?
    float minSelectDistancePixels;
    float minEdgeDistancePixels;

    std::vector<glm::vec2> verticesProjectedToScreen; // Almacenamos vertices de la malla proyectados en la pantalla

    // !! convertir a booleano si no esta dentro de la zona de clip en lugar de devolver valor muy lejano?
    glm::vec2 worldToScreen(const glm::vec3& p, int width, int height, const glm::mat4& view, const glm::mat4& projection);

    // Dada la malla mesh, devolvemos el vertice del vector Mesh::vertices mas proximo al raton(vertices de renderizado), 
    // convirtiendolos a coordenadas en pantalla para medir la distancia al raton
    int pickVertex(const Mesh& mesh, float mouseX, float mouseY, int width, int height, Camera* camera);

    int pickEdge(const Mesh& mesh, float mouseX, float mouseY, int width, int height, Camera* camera);

    int pickFace(const Mesh& mesh, float mouseX, float mouseY, int width, int height, Camera* camera);

    // Determina si un punto esta dentro de un triangulo usando coordenadas baricentricas
    bool pointInTriangle(const glm::vec2& p, const glm::vec2& a, const glm::vec2& b, const glm::vec2& c);

};