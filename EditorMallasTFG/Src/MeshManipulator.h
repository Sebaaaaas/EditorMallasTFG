#pragma once

#include <glm/glm.hpp>
#include <unordered_set>

#include "Mesh.h"
#include "EditorTypes.h"


class Ray;
class Mesh;
class Camera;
class Selector;

// Dada una malla e input de raton, permite editar la malla
class MeshManipulator : public QObject{

    Q_OBJECT

public:

    MeshManipulator(Selector* selector_);
    ~MeshManipulator();

    void setEditingMesh(Mesh* mesh);

    void beginTransform(const Camera& camera, float mouseX, float mouseY, int w, int h);
    void updateTransform(float mouseX, float mouseY, int w, int h, const Camera& camera);
    void endTransform();

    bool isDragging() const;

    void setTransformMode(TransformMode mode);
    void setTransformAxis(TransformAxis axis);

    void extrudeSelection(float distance);

    void deleteSelection();

    void setSelectedXPosition(double value);
    void setSelectedYPosition(double value);    
    void setSelectedZPosition(double value);

signals:

    // Cuando cambia la posicion, actualizamos el los spinbox
    void selectedPositionChanged(double x, double y, double z);


private:
    // Indica si el usuario esta arrastrando con el raton
    bool dragging = false;

    // Usado para rotacion y escala
    glm::vec2 transformStartMouse;

    glm::vec3 dragStartPoint;
    glm::vec3 dragPlaneNormal;

    Ray* ray;
    Selector* selector;

    Mesh* currentMesh;

    TransformMode transformMode;
    TransformAxis transformAxis;

    glm::vec3 transformPivot;

    // Guarda indices originales de cada vertice que se transforme en un frame
    std::unordered_map<unsigned int, glm::vec3> originalPositions;

    glm::vec3 selectionCenter() const;

    // Calcula transformPivot nuevo y escoge nuevos vertices, ademas de guardar posiciones originales. Llamar antes de realizar una operacion de transform
    void refreshSelectionSnapshot();

    void updateTranslation(float mouseX, float mouseY, int w, int h, const Camera& camera);
    void updateRotation(float mouseX, float mouseY);
    void updateScale(float mouseX, float mouseY);

    // Aplica una transformacion a la seleccion
    void transformSelection(const glm::mat4& transform);

    void translateSelection(const glm::vec3& delta);
    void rotateSelection(float angle, glm::vec3 axis);
    void scaleSelection(glm::vec3 scale);

};