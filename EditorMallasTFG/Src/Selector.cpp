#include "Selector.h"

#include "Mesh.h"
#include "Camera.h"
#include "Ray.h"

Selector::Selector() {

    currentSelectionMode = SelectionMode::Vertex;

    minSelectDistancePixels = 10.0f; // !! esto es feo?
    minEdgeDistancePixels = 10.0f;
}

Selector::~Selector() {
}

void Selector::projectVerticesToScreen(const Mesh& mesh, int width, int height, const glm::mat4& view, const glm::mat4& projection) {

    projectedVertices.resize(mesh.vertices.size());

    for (size_t i = 0; i < mesh.vertices.size(); ++i) {
        projectedVertices[i] = worldToScreen(mesh.vertices[i].Position, width, height, view, projection);
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

ProjectedVertex Selector::worldToScreen(const glm::vec3& p, int width, int height, const glm::mat4& view, const glm::mat4& projection) {

    ProjectedVertex result;

    // !! No multiplicamos por model porque actualmente nuestra matriz model es la matriz identidad(mat4(1.0f)), si queremos moverlo en el mundo habra
    // que cambiar esto y añadir la matriz model a la operacion
    glm::vec4 clipSpace = projection * view * glm::vec4(p, 1.0f); // Pasamos p a vector4 para poder operar con las matrices

    // Si w <= 0, se encuentra detras de la camara, y no sera seleccionable, por lo que devolvemos una posicion muy lejana(!!feo y puede fallar en casos extremos)
    if (clipSpace.w <= 0.0f) {
        result.visible = false;
        result.depth = std::numeric_limits<float>::max();
        result.screenPosition = glm::vec2(0.0f);
        return result;
    }

    // Proceso de viewport transform (ndc: normalized device coordinates), nos deja las coordenadas en el espacio [-1,1]
    glm::vec3 ndc = glm::vec3(clipSpace) / clipSpace.w;

    // Convertimos el espacio normalizado a espacio en la pantalla
    result.screenPosition.x = (ndc.x + 1.0f) * 0.5f * width;
    result.screenPosition.y = (1.0f - ndc.y) * 0.5f * height; // Aqui restamos para darle la vuelta, los sistemas de coordenadas estan al reves si no

    result.depth = ndc.z;
    result.visible = true;

    return result;
}

int Selector::pickVertex(const Mesh& mesh, float mouseX, float mouseY, int width, int height, Camera* camera) {

    int selectedVertex = -1;
    float minScreenDistance = minSelectDistancePixels;
    float minDepth = std::numeric_limits<float>::max();

    // Recorremos todos los vertices de la malla
    for (int i = 0; i < mesh.vertices.size(); i++) {

        const ProjectedVertex& projected = projectedVertices[i];

        if (!projected.visible)
            continue;

        // Calculamos la distancia entre vertice en pantalla y raton
        float dist = glm::distance(projected.screenPosition, glm::vec2(mouseX, mouseY));

        // Nos saltamos solamente los pixeles lejanos, todavia no descartamos por minScreenDistance
        if (dist > minSelectDistancePixels)
            continue;

        if (isBetterCandidate(dist, projected.depth, minScreenDistance, minDepth)) {
            selectedVertex = i;
        }
    }

    return selectedVertex;
}

int Selector::pickEdge(const Mesh& mesh, float mouseX, float mouseY, int width, int height, Camera* camera) {

    int selectedEdge = -1;

    float minScreenDistance = minEdgeDistancePixels;
    float minDepth = std::numeric_limits<float>::max();

    glm::vec2 mouse(mouseX, mouseY);

    for (int i = 0; i < mesh.edges.size(); ++i) {

        const Edge& edge = mesh.edges[i];

        // Obtenemos las dos proyecciones de los vertices del segmento
        const ProjectedVertex& pa = projectedVertices[edge.v0];
        const ProjectedVertex& pb = projectedVertices[edge.v1];

        if (!pa.visible || !pb.visible)
            continue;

        glm::vec2 ab = pb.screenPosition - pa.screenPosition;

        // Calculo de longitud del vector ab(hacerlo asi ahorra raices cuadradas)
        float len = glm::dot(ab, ab);

        // Ingoramos si es muy pequeño
        if (len < 0.00001f)
            continue;

        // Determinamos la posicion del raton t sobre el segmento, que resulta en valores entre 0(sobre a) y 1(sobre b)
        float t = glm::dot(mouse - pa.screenPosition, ab) / len;
        t = glm::clamp(t, 0.0f, 1.0f); // Por si da un valor fuera del rango, lo mantenemos dentro de el

        // Punto del segmento mas cercano al raton
        glm::vec2 closest = pa.screenPosition + t * ab;

        float dist = glm::distance(mouse, closest);

        if (dist > minEdgeDistancePixels)
            continue;

        float depth = pa.depth * (1.0f - t) + pb.depth * t;

        if (isBetterCandidate(dist, depth, minScreenDistance, minDepth)) {
            selectedEdge = i;
        }
    }

    return selectedEdge;
}

int Selector::pickFace(const Mesh& mesh, float mouseX, float mouseY, int width, int height, Camera* camera) {

    int selectedPolygon = -1;
    float bestDepth = std::numeric_limits<float>::max();

    glm::vec2 mouse(mouseX, mouseY);

    // Recorremos poligonos
    for (int i = 0; i < mesh.polygons.size(); ++i) {

        const Polygon& polygon = mesh.polygons[i];

        // Por si acaso hay poligonos erroneos con menos de 3 vertices
        if (polygon.vertices.size() < 3)
            continue;

        glm::vec2 a = projectedVertices[polygon.vertices[0]].screenPosition;

        // Recorremos los vertices del poligono
        for (int j = 1; j + 1 < polygon.vertices.size(); ++j) {

            glm::vec2 b = projectedVertices[polygon.vertices[j]].screenPosition;

            glm::vec2 c = projectedVertices[polygon.vertices[j + 1]].screenPosition;

            if (pointInTriangle(mouse, a, b, c)) {

                float depth =
                    (
                        projectedVertices[polygon.vertices[0]].depth +
                        projectedVertices[polygon.vertices[j]].depth +
                        projectedVertices[polygon.vertices[j + 1]].depth
                        ) / 3.0f;

                if (depth < bestDepth) {

                    bestDepth = depth;
                    selectedPolygon = i;
                }
            }
        }
    }

    return selectedPolygon;
}

bool Selector::isBetterCandidate(float distance, float depth, float& bestDistance, float& bestDepth) const {
    
    if (distance < bestDistance - distanceEpsilon) {
        bestDistance = distance;
        bestDepth = depth;
        return true;
    }

    if (std::abs(distance - bestDistance) <= distanceEpsilon && depth < bestDepth) {
        bestDistance = distance;
        bestDepth = depth;
        return true;
    }

    return false;
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


