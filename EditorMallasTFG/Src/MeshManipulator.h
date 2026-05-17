#pragma once

#include <glm.hpp>
#include <unordered_set>

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

    void selectVertex(const Mesh* mesh, int vertexIndex, bool additive = false);
    void clearSelection();
    bool hasSelection() const;

    // Devuelve un conjunto con los indices de los vertices que han sido seleccionados
    std::unordered_set<unsigned int> getSelectedGroups();

private:
    bool dragging = false;

    std::unordered_set<unsigned int> selectedGroups;

    glm::vec3 dragStartPoint;
    glm::vec3 dragPlaneNormal;

    Ray* ray;

};