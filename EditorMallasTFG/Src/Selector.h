#pragma once

#include <glm.hpp>

class Mesh;
class Camera;
class Ray;
class DebugRenderer;

#include <vector>

class Selector
{
public:
    Selector();
    ~Selector();

    // Seleccionamos un vertice ada la posicion del raton, lanzando un rayo desde la camara y encontrando el vertice mas proximo
    int pickVertex(const Mesh& mesh, float mouseX, float mouseY, int width, int height, Camera* camera);

private:
    // Distancia minima para la seleccion de un vertice
    float minSelectDistance;

    Ray* ray;
    DebugRenderer* dbRenderer;
};