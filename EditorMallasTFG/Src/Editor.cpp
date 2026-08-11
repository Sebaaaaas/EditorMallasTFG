#include "Editor.h"

#include <glad/gl.h>

//#include <glm.hpp> - actualmente en el .h
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <stdio.h>
#include <assert.h>

#include "MeshManipulator.h"
#include "DebugRenderer.h"
#include "Selector.h"
#include "Camera.h"
#include "Shader.h"
#include "Input.h"
#include "Mesh.h"

#include <QOpenGLContext>
#include <QApplication>
#include <iostream>

Editor::Editor() {
    defaultMesh = nullptr;
    defaultShader = nullptr;
    debugShader = nullptr;

    camera = nullptr;
    selector = nullptr;
    meshManipulator = nullptr;
    debugRenderer = nullptr;

    hoveredElement = -1;
    renderMode = RenderMode::Solid;
}

Editor::~Editor() {
    delete defaultShader;
    defaultShader = nullptr;
    
    delete debugShader;
    debugShader = nullptr;

    delete defaultMesh;
    defaultMesh = nullptr;

    delete camera;
    camera = nullptr;

    delete selector;
    selector = nullptr;

    delete meshManipulator;
    meshManipulator = nullptr;

    delete debugRenderer;
    debugRenderer = nullptr;

}

bool Editor::init() {

    // Inicializamos glad para poder llamar a funciones de OpenGL
    if (!initializeGlad())
        return false;

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    // Para el transparente de debug
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    camera = new Camera((float)win_w, (float)win_h);

    //defaultMesh = nullptr; // !! cuando queramos entregar, poner a nullptr
    defaultMesh = new Mesh("Assets/cubo.obj");

    // Creacion de shader
    defaultShader = new Shader("Assets/mainShader.vert", "Assets/mainShader.frag");
    debugShader = new Shader("Assets/debugShader.vert", "Assets/debugShader.frag");

    selector = new Selector();
    meshManipulator = new MeshManipulator();
    debugRenderer = new DebugRenderer();

	return true;
}

bool Editor::initializeGlad() {

    QOpenGLContext* context = QOpenGLContext::currentContext();

    if (!context) {
        fprintf(stderr, "ERROR: No hay un contexto OpenGL activo.\n");
        return false;
    }

    int version_glad = gladLoadGL(
        [](const char* name) -> GLADapiproc {
            QOpenGLContext* ctx = QOpenGLContext::currentContext();
            if (!ctx)
                return nullptr;
            return reinterpret_cast<GLADapiproc>(ctx->getProcAddress(name));
        }
    );

    if (version_glad == 0) {
        fprintf(stderr, "ERROR: Fallo en inicializacion de GLAD\n");
        return false;
    }

    return true;
}

void Editor::run() {

    Input::update();

    logic();
    render();

    // Debemos llamarlo al final para resetear valores de delta para movimientos
    Input::endFrame();
}

void Editor::setWindowSize(int w, int h) {
    win_w = w;
    win_h = h;

    if (camera != nullptr)
        camera->setAspectRatio(w, h);
}

bool Editor::loadMesh(const std::string& path) { // !! aviso, nunca devuelve false, si esta corrupto el archivo o es un path invalido, explota la aplicacion

    Mesh* newMesh = new Mesh(path);

    delete defaultMesh;
    defaultMesh = newMesh;

    hoveredElement = -1;
    meshManipulator->clearSelection();

    return true;
}

void Editor::saveMesh(const std::string& path) {
    defaultMesh->saveOBJ(path);
}

void Editor::setSelectionMode(SelectionMode mode) {
    selector->setSelectionMode(mode);
}

MeshManipulator* Editor::getMeshManipulator() const {
    return meshManipulator;
}

void Editor::setRenderMode(RenderMode mode) {
    renderMode = mode;
}

void Editor::logic() {

    if (!defaultMesh)
        return;

    if (Input::isKeyDown(Qt::Key_Escape))
        QApplication::quit();

    meshManipulator->setEditingMesh(defaultMesh);

    camera->manageInput();

    // !! no creo que haga falta hacer esto continuamente
    selector->projectVerticesToScreen(*defaultMesh, win_w, win_h, camera->getViewMatrix(), camera->getProjectionMatrix());

    hoveredElement = selector->pick(*defaultMesh, Input::getMouseX(), Input::getMouseY(), win_w, win_h, camera);

    // Click izquierdo
    if (Input::isMouseButtonDown(0) && !meshManipulator->isDragging()) {

        bool additive = Input::isKeyDown(Qt::Key_Shift);

        if (hoveredElement != -1) {

            SelectionMode currentSelectionMode = selector->getSelectionMode();

            switch (currentSelectionMode)
            {
            case SelectionMode::Vertex:
                meshManipulator->selectVertex(hoveredElement, additive);
                break;
            case SelectionMode::Edge:
                meshManipulator->selectEdge(hoveredElement, additive);
                break;
            case SelectionMode::Face:
                meshManipulator->selectPolygon(hoveredElement, additive);
                break;
            default:
                break;
            }

            meshManipulator->beginTransform(*camera, Input::getMouseX(), Input::getMouseY(), win_w, win_h);
        }
        else if (!additive) {
            meshManipulator->clearSelection();
        }
    }

    if (meshManipulator->isDragging() && Input::isMouseButtonUp(0)) {
        meshManipulator->endTransform();
    }

    if (meshManipulator->isDragging()) {
        meshManipulator->updateTransform(Input::getMouseX(), Input::getMouseY(), win_w, win_h, *camera);
    }

    if (Input::isKeyPressed(Qt::Key_E))
        meshManipulator->extrudeSelection(0.5);

    if (Input::isKeyPressed(Qt::Key_Delete)) {
        meshManipulator->deleteSelection();
        hoveredElement = -1;
    }
}

void Editor::render() {
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    switch (renderMode) {

    case RenderMode::Solid:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;

    case RenderMode::Wireframe:
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;
    }

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camera->getViewMatrix();
    glm::mat4 projection = camera->getProjectionMatrix();

    glm::mat4 MVP = projection * view * model;

    if (!defaultMesh)
        return;

    defaultShader->use();

    defaultShader->setMat4("MVP", MVP);
    defaultShader->setMat4("model", model);
    defaultShader->setVec3("lightDir", glm::vec3(-0.5f, -1.0f, -0.3f));
    defaultShader->setVec3("objectColor", glm::vec3(0.6f, 0.7f, 1.0f));

    defaultMesh->draw();

    // Aseguramos que otras cosas no se rendericen con el modo equivocado
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Lineas debug de elementos seleccionados
    drawDebug(MVP);
    
}

void Editor::drawDebug(const glm::mat4& MVP) {

    if (!meshManipulator->hasSelection())
        return;

    debugShader->use();

    debugShader->setMat4("MVP", MVP);

    glDisable(GL_DEPTH_TEST);

    SelectionMode currentSelectionMode = selector->getSelectionMode();

    switch (currentSelectionMode) {

    case SelectionMode::Vertex: {

        for (unsigned int group : meshManipulator->getSelectedGroups()) {
            for (unsigned int idx : defaultMesh->vertexGroups[group]) {
                debugRenderer->drawPoint(defaultMesh->vertices[idx].Position);
            }
        }

        //glm::vec3 pos = defaultMesh->vertices[selectedElement].Position;
        //debugRenderer->drawPoint(pos);
    }
        break;
    case SelectionMode::Edge: {

        for (unsigned int edgeIndex : meshManipulator->getSelectedEdges()) {

            const Edge& edge = defaultMesh->edges[edgeIndex];
            debugRenderer->drawEdge(*defaultMesh, edge);
        }
    }
        break;
    case SelectionMode::Face: {

        for (unsigned int polygonIndex : meshManipulator->getSelectedPolygons()) {

            const Polygon& polygon = defaultMesh->polygons[polygonIndex];
            debugRenderer->drawPolygon(*defaultMesh, polygon);
        }
    }
        break;
    default:
        break;
    }

    glEnable(GL_DEPTH_TEST);

}

