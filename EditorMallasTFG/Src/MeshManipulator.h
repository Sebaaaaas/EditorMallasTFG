#pragma once

#include <glm.hpp>

class Mesh;
class Camera;
class Ray;

// Dada una malla e input de raton, permite editar la malla
class MeshManipulator
{
public:

    MeshManipulator();
    ~MeshManipulator();

    void beginDrag(const Mesh* mesh, int vertexIndex, const Camera& camera);
    void updateDrag(Mesh* mesh, float mouseX, float mouseY, int w, int h, const Camera& camera);
    void endDrag();

    bool isDragging() const { return dragging; }

private:
    bool dragging = false;
    int selectedVertex = -1;

    glm::vec3 dragStartPoint;
    glm::vec3 dragPlaneNormal;

    Ray* ray;

};