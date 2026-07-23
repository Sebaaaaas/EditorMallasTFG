#include "Selector.h"

#include "Mesh.h"
#include "Camera.h"

Selector::Selector() {

    currentSelectionMode = SelectionMode::Vertex;

    minSelectDistancePixels = 10.0f; // !! esto es feo?
    minEdgeDistancePixels = 10.0f;
}

Selector::~Selector() {
}

void Selector::projectVerticesToScreen(const Mesh& mesh, int width, int height, const glm::mat4& view, const glm::mat4& projection) {

    verticesProjectedToScreen.resize(mesh.vertices.size());

    for (int i = 0; i < verticesProjectedToScreen.size(); ++i) {

        verticesProjectedToScreen[i] = worldToScreen(mesh.vertices[i].Position, width, height, view, projection);
    }
}

int Selector::pick(const Mesh& mesh, float mouseX, float mouseY, int width, int height, Camera* camera)
{
    switch (currentSelectionMode)
    {
    case SelectionMode::Vertex:
        return pickVertex(mesh, mouseX, mouseY, width, height, camera);

    case SelectionMode::Edge:
        return pickEdge(mesh, mouseX, mouseY, width, height, camera);

    case SelectionMode::Face:
        return pickFace(mesh, mouseX, mouseY, width, height, camera);
    }

    return -1;
}

void Selector::setSelectionMode(SelectionMode newMode) {
    currentSelectionMode = newMode;
}

SelectionMode Selector::getSelectionMode() const {
    return currentSelectionMode;
}

glm::vec2 Selector::worldToScreen(const glm::vec3& p, int width, int height, const glm::mat4& view, const glm::mat4& projection) {

    // !! No multiplicamos por model porque actualmente nuestra matriz model es la matriz identidad(mat4(1.0f)), si queremos moverlo en el mundo habra
    // que cambiar esto y añadir la matriz model a la operacion
    glm::vec4 clipSpace = projection * view * glm::vec4(p, 1.0f); // Pasamos p a vector4 para poder operar con las matrices

    // Si w <= 0, se encuentra detras de la camara, y no sera seleccionable, por lo que devolvemos una posicion muy lejana(!!feo y puede fallar en casos extremos)
    if (clipSpace.w <= 0.0f)
        return glm::vec2(-99999.0f);

    // Proceso de viewport transform (ndc: normalized device coordinates), nos deja las coordenadas en el espacio [-1,1]
    glm::vec3 ndc = glm::vec3(clipSpace) / clipSpace.w;

    // Convertimos el espacio normalizado a espacio en la pantalla
    float screenX = (ndc.x + 1.0f) * 0.5f * width;
    float screenY = (1.0f - ndc.y) * 0.5f * height; // Aqui restamos para darle la vuelta, los sistemas de coordenadas estan al reves si no

    return glm::vec2(screenX, screenY);
}

int Selector::pickVertex(const Mesh& mesh, float mouseX, float mouseY, int width, int height, Camera* camera) {

    int selectedVertex = -1;
    float minDist = minSelectDistancePixels;

    // Recorremos todos los vertices de la malla
    for (int i = 0; i < mesh.vertices.size(); i++) {

        glm::vec3 vertexPos = mesh.vertices[i].Position;

        // Calculamos la posicion del vertice en pantalla

        float dist = glm::distance(verticesProjectedToScreen[i], glm::vec2(mouseX, mouseY));

        if (dist < minDist) {
            minDist = dist;
            selectedVertex = i;
        }
    }

    return selectedVertex;
}

int Selector::pickEdge(const Mesh& mesh, float mouseX, float mouseY, int width, int height, Camera* camera) { // !! REVISAR ESTO

    int selectedEdge = -1;
    float minDist = minEdgeDistancePixels;

    glm::vec2 mouse(mouseX, mouseY);

    for (int i = 0; i < mesh.edges.size(); i++)
    {
        const Edge& edge = mesh.edges[i];

        glm::vec3 a = mesh.vertices[edge.v0].Position;
        glm::vec3 b = mesh.vertices[edge.v1].Position;

        // Prroyectamos los vertices a la pantalla
        glm::vec2 a2 = verticesProjectedToScreen[edge.v0];

        glm::vec2 b2 = verticesProjectedToScreen[edge.v1];

        glm::vec2 ab = b2 - a2;

        // Evitamos segmentos degenerados
        float len2 = glm::dot(ab, ab);
        if (len2 < 0.00001f)
            continue;

        // Proyeccion del raton sobre el segmento
        float t = glm::dot(mouse - a2, ab) / len2;
        t = glm::clamp(t, 0.0f, 1.0f);

        glm::vec2 closest = a2 + t * ab;

        float dist = glm::distance(mouse, closest);

        if (dist < minDist) {
            minDist = dist;
            selectedEdge = i;
        }
    }

    return selectedEdge;
}

int Selector::pickFace(const Mesh& mesh, float mouseX, float mouseY, int width, int height, Camera* camera) { // !! revisar porque algo falla

    for (int i = 0; i < mesh.faces.size(); ++i) {

        const Face& face = mesh.faces[i];

        glm::vec2 a = verticesProjectedToScreen[face.v0];
        glm::vec2 b = verticesProjectedToScreen[face.v1];
        glm::vec2 c = verticesProjectedToScreen[face.v2];

        if (pointInTriangle(glm::vec2(mouseX, mouseY), a, b, c))
            return i;
    }

    return -1;
}

bool Selector::pointInTriangle(const glm::vec2& p, const glm::vec2& a, const glm::vec2& b, const glm::vec2& c) {

    glm::vec2 v0 = c - a;
    glm::vec2 v1 = b - a;
    glm::vec2 v2 = p - a;

    float dot00 = glm::dot(v0, v0);
    float dot01 = glm::dot(v0, v1);
    float dot02 = glm::dot(v0, v2);
    float dot11 = glm::dot(v1, v1);
    float dot12 = glm::dot(v1, v2);

    float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);

    float u = (dot11 * dot02 - dot01 * dot12) * invDenom;

    float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

    return (u >= 0.0f) && (v >= 0.0f) && (u + v <= 1.0f);

}


