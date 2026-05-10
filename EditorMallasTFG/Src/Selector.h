#pragma once

#include <glm.hpp>

class Mesh;
class Camera;
class Ray;
class DebugRenderer;

class Selector
{
public:
    Selector();
    ~Selector();

    // Dada la malla mesh, devolvemos el vertice del vector Mesh::vertices mas proximo al raton
    int pickVertex(const Mesh& mesh, float mouseX, float mouseY, int width, int height, Camera* camera);

private:
    // Distancia minima para la seleccion de un vertice
    float minSelectDistance;

    Ray* ray;
    DebugRenderer* dbRenderer;
};