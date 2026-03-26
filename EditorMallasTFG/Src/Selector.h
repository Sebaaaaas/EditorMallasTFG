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

    int pickVertex(const Mesh& mesh, float mouseX, float mouseY, int width, int height, const Camera& camera);

private:
    // Minimum distance from vertex to be selected
    float minSelectDistance;

    Ray* ray;
    DebugRenderer* dbRenderer;
};