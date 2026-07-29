#pragma once

#include <glm.hpp>
#include <unordered_set>

#include <QObject>

class Mesh;
class Camera;
class Ray;

enum class TransformMode
{
    Translate,
    Rotate,
    Scale
};

enum class TransformAxis 
{
    X, 
    Y,
    Z,
    All
};

// Dada una malla e input de raton, permite editar la malla
class MeshManipulator : public QObject {

    Q_OBJECT // usar "public:" despues de esto para declarar las variables publicas, esto es necesario para detectar que es un QObject

public:

    MeshManipulator();
    ~MeshManipulator();

    void setEditingMesh(Mesh* mesh);

    void beginTransform(const Camera& camera, float mouseX, float mouseY);
    void updateTransform(float mouseX, float mouseY, int w, int h, const Camera& camera);
    void endTransform();

    void updateTranslation(float mouseX, float mouseY, int w, int h, const Camera& camera);
    void updateRotation(float mouseX, float mouseY);
    void updateScale(float mouseX, float mouseY);

    bool isDragging() const { return dragging; }

    void selectVertex(int vertexIndex, bool additive = false);
    void selectEdge(unsigned int edgeIndex, bool additive = false);
    void selectPolygon(unsigned int polygonIndex, bool additive = false);

    void clearSelection(); // !! esto seguramente deberia ir en el selector
    bool hasSelection() const; // !! esto seguramente deberia ir en el selector

    const std::unordered_set<unsigned int>& getSelectedGroups() const;
    const std::unordered_set<unsigned int>& getSelectedEdges() const;
    const std::unordered_set<unsigned int>& getSelectedPolygons() const;

    // Devuelve un conjunto con los indices de los vertices que han sido seleccionados
    std::unordered_set<unsigned int> getSelectedGroups();

    void setTransformMode(TransformMode mode);
    void setTransformAxis(TransformAxis axis);


signals:
    // Cuando cambia la posicion, actualizamos el los spinbox
    void selectedPositionChanged(double x, double y, double z);

public slots: // Permite recibir señales de QWidgets cuando cambian sus valores https://doc.qt.io/qt-6/signalsandslots.html
    void setSelectedXPosition(double value);
    void setSelectedYPosition(double value);
    void setSelectedZPosition(double value);

private:
    // Indica si el usuario esta arrastrando con el raton
    bool dragging = false;

    std::unordered_set<unsigned int> selectedGroups;
    std::unordered_set<unsigned int> selectedEdges;
    std::unordered_set<unsigned int> selectedPolygons;

    // Usado para rotacion y escala
    glm::vec2 transformStartMouse;

    glm::vec3 dragStartPoint;
    glm::vec3 dragPlaneNormal;

    Ray* ray;

    Mesh* currentMesh;

    TransformMode transformMode;
    TransformAxis transformAxis;

    glm::vec3 transformPivot;

    std::vector<unsigned int> selectedVertices;
    std::vector<glm::vec3> originalPositions;

    glm::vec3 selectionCenter() const;

    // Aplica una transformacion a la seleccion
    void transformSelection(const glm::mat4& transform);

    void translateSelection(const glm::vec3& delta);

    void rotateSelection(float angle, glm::vec3 axis);

    void scaleSelection(glm::vec3 scale);

};