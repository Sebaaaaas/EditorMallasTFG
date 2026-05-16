#pragma once

#include <glm.hpp>

class Mesh;
class Camera;
class Ray;

class Selector
{
public:
    Selector();
    ~Selector();

    // Dada la malla mesh, devolvemos el vertice del vector Mesh::vertices mas proximo al raton(vertices de renderizado)
    int pickVertex(const Mesh& mesh, float mouseX, float mouseY, int width, int height, Camera* camera);

private:
    // Distancia minima para la seleccion de un vertice
    float minSelectDistance;

    Ray* ray;

};