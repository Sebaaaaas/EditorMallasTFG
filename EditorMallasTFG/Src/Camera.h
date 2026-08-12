#pragma once

#include <gtc/matrix_transform.hpp>

#include "EditorTypes.h"

class Camera
{
public:
    Camera(float width, float height);

    glm::vec3 getPosition() const;

    // El viewMatrix transforma coordenadas 3D del mundo a la vista de la camara(tecnicamente, camara no gira, el mundo entero gira alrededor de la camara) (world to view)
    glm::mat4 getViewMatrix() const;

    // ProjectionMatrix transforma el mundo 3D y lo pasa a 2D para poder renderizarlo en la pantalla(view to projection)
    glm::mat4 getProjectionMatrix() const;

    void manageInput();

    void setAspectRatio(float width, float height);

    void setPosition(const glm::vec3& pos);
    void setTarget(const glm::vec3& newTarget);
    glm::vec3 getTarget() const;

    // Resetea la camara a su posicion inicial
    void reset();
    void orbit(float xoffset, float yoffset);
    void pan(float xoffset, float yoffset);
    void zoom(float amount);

    void setProjectionMode(ProjectionMode mode);

private:
    glm::vec3 position; // Posicion de la camara
    glm::vec3 target; // Posicion a la que estamos mirando
    glm::vec3 up; // Direccion hacia arriba del mundo, por lo general siempre sera (0, 1, 0)

    // Variables para la rotacion de la camara
    float yaw, pitch;
    float orbitSensitivity, panSensitivity, zoomSensitivity;
    // Distancia hasta el objeto de focus de la camara
    float distance; 

    // Mas info sobre variables en https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/gluPerspective.xml
    float fov; // Angulo del field of view, en la direccion y
    float aspectRatio; // Relacion de x(ancho) respecto a y(alto) de la camara
    float nearPlane; // Siempre debe ser positivo
    float farPlane; // Siempre debe ser positivo

    ProjectionMode projectionMode;
    float orthogonalZoom;
};