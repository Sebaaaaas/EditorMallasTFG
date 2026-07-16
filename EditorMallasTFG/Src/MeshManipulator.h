#pragma once

#include <glm.hpp>
#include <unordered_set>

#include <QObject>

class Mesh;
class Camera;
class Ray;

// Dada una malla e input de raton, permite editar la malla
class MeshManipulator : public QObject {

    Q_OBJECT // usar public: despues de esto para declarar las variables publicas, necesario para detectar que es un QObject

public:

    MeshManipulator();
    ~MeshManipulator();

    void setEditingMesh(Mesh* mesh);

    void beginDrag(const Mesh* mesh, const std::vector<unsigned int>& vertexIndex, const Camera& camera);
    void updateDrag(Mesh* mesh, float mouseX, float mouseY, int w, int h, const Camera& camera);
    void endDrag();

    bool isDragging() const { return dragging; }

    void selectVertex(const Mesh* mesh, int vertexIndex, bool additive = false);
    void clearSelection();
    bool hasSelection() const;

    // Devuelve un conjunto con los indices de los vertices que han sido seleccionados
    std::unordered_set<unsigned int> getSelectedGroups();

signals:
    // Cuando cambia la posicion, actualizamos el los spinbox
    void selectedPositionChanged(double x, double y, double z);

public slots: // Permite recibir señales de QWidgets cuando cambian sus valores https://doc.qt.io/qt-6/signalsandslots.html
    void setSelectedXPosition(double value);
    void setSelectedYPosition(double value);
    void setSelectedZPosition(double value);

private:
    bool dragging = false;

    std::unordered_set<unsigned int> selectedGroups;

    glm::vec3 dragStartPoint;
    glm::vec3 dragPlaneNormal;

    Ray* ray;

    Mesh* currentMesh;

    glm::vec3 selectionCenter() const;

    // Para que los slots setSelectedPosition lo llamen y muevan elementos seleccionados
    void translateSelection(const glm::vec3& delta);

};