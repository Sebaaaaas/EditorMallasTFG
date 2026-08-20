#pragma once

#include <glm/glm.hpp>

class Mesh;
class Camera;

enum class SelectionMode {
    Vertex,
    Edge,
    Face
};

// Guardamos la posicion en pantalla de los vertices y su profundidad para poder seleccionarlos facilmente. Visible sirve para saber si esta dentro del
// area visual de la camara, no para si esta delante o detras de otro elemento
struct ProjectedVertex {
    glm::vec2 screenPosition;
    float depth;
    bool visible;
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

    // Distancia minima para la seleccion de un vertice en pixeles
    float minSelectDistancePixels;
    float minEdgeDistancePixels;

    // Variable que influye en que priorizamos, distancia al raton del vertice seleccionado o profundidad
    const float distanceEpsilon = 2.0f;

    std::vector<ProjectedVertex> projectedVertices; // Almacenamos vertices de la malla proyectados en la pantalla

    ProjectedVertex worldToScreen(const glm::vec3& p, int width, int height, const glm::mat4& viewProjection);

    // Dada la malla mesh, devolvemos el vertice del vector Mesh::vertices mas proximo al raton(vertices de renderizado), 
    // convirtiendolos a coordenadas en pantalla para medir la distancia al raton, pero tambien teniendo en cuenta la distancia a la pantalla
    int pickVertex(const Mesh& mesh, float mouseX, float mouseY, int width, int height, Camera* camera);

    int pickEdge(const Mesh& mesh, float mouseX, float mouseY, int width, int height, Camera* camera);

    // Selecciona el poligono al que apunta el raton de la malla mesh
    int pickFace(const Mesh& mesh, float mouseX, float mouseY, int width, int height, Camera* camera);

    // Funcion auxiliar para determinar si un vertice con distancia al raton distance y profundidad respecto a pantalla depth es mejor como candidato seleccionado
    // frente a otro con distancia y profundidad distintos
    bool isBetterCandidate(float distance, float depth, float& bestDistance, float& bestDepth) const;

    // Determina si un punto esta dentro de un triangulo usando el algoritmo de coordenadas baricentricas, que ademas devolvemos en u y v para poder usar
    bool pointInTriangle(const glm::vec2& p, const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, float& u, float& v);

};