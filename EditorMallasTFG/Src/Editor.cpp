#include "Editor.h"

#include <glad/gl.h>

//#include <glm.hpp> - actualmente en el .h
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <stdio.h>
#include <assert.h>

#include "Selector.h"
#include "Camera.h"
#include "Shader.h"
#include "Input.h"
#include "Mesh.h"
#include "MeshManipulator.h"
#include "DebugRenderer.h"

#include <QOpenGLContext>
#include <QApplication>

Editor::Editor() {
    defaultMesh = nullptr;
    defaultShader = nullptr;
    debugShader = nullptr;

    camera = nullptr;
    selector = nullptr;
    meshManipulator = nullptr;
    debugRenderer = nullptr;
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

    camera = new Camera((float)win_w, (float)win_h);

    defaultMesh = new Mesh("Assets/modelo.obj");

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

    int version_glad = gladLoadGL( // !! revisar
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

    input();
    logic();
    render();
}

void Editor::setWindowSize(int w, int h) {
    win_w = w;
    win_h = h;
}

void Editor::input() {

    Input::beginFrame();
    Input::update();
}

void Editor::logic() {

    if (Input::isKeyDown(Qt::Key_Escape))
        QApplication::quit();

    camera->manageInput();

    if (!meshManipulator->isDragging()) {
        selectedVertex = selector->pickVertex(*defaultMesh, Input::getMouseX(), Input::getMouseY(), win_w, win_h, camera);
    }

    // Click izquierdo
    if (Input::isMouseButtonDown(0) && !meshManipulator->isDragging()) {
        bool additive = Input::isKeyDown(Qt::Key_Shift);

        if (selectedVertex != -1)
        {
            meshManipulator->selectVertex(defaultMesh, selectedVertex, additive);
            meshManipulator->beginDrag(defaultMesh, selectedVertex, *camera);
        }
        else if (!additive)
        {
            meshManipulator->clearSelection();
        }
    }

    if (meshManipulator->isDragging() && !Input::isMouseButtonDown(0)) {
        meshManipulator->endDrag();
    }

    if (meshManipulator->isDragging()) {
        meshManipulator->updateDrag(defaultMesh, Input::getMouseX(), Input::getMouseY(), win_w, win_h, *camera);
    }

    // Guardar modelo !! siempre se guarda como lo mismo
    if (Input::isKeyDown(Qt::Key_Control) && Input::isKeyDown(Qt::Key_S)) {
        defaultMesh->saveOBJ("Assets/edited.obj");
    }
}

void Editor::render() {
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camera->getViewMatrix();
    glm::mat4 projection = camera->getProjectionMatrix();

    glm::mat4 MVP = projection * view * model;

    defaultShader->use();

    glUniformMatrix4fv(glGetUniformLocation(defaultShader->getID(), "MVP"), 1, GL_FALSE, glm::value_ptr(MVP));

    glUniformMatrix4fv(glGetUniformLocation(defaultShader->getID(), "model"), 1, GL_FALSE, glm::value_ptr(model));

    glUniform3f(glGetUniformLocation(defaultShader->getID(), "lightDir"), -0.5f, -1.0f, -0.3f);

    glUniform3f(glGetUniformLocation(defaultShader->getID(), "objectColor"), 0.6f, 0.7f, 1.0f);

    defaultMesh->draw();

    // Debug
    if (selectedVertex != -1) {
        debugShader->use();

        glUniformMatrix4fv(
            glGetUniformLocation(debugShader->getID(), "MVP"),
            1, GL_FALSE, glm::value_ptr(MVP)
        );

        glm::vec3 pos = defaultMesh->vertices[selectedVertex].Position;

        for (unsigned int group : meshManipulator->getSelectedGroups())
        {
            for (unsigned int idx : defaultMesh->vertexGroups[group])
            {
                debugRenderer->drawPoint(defaultMesh->vertices[idx].Position);
            }
        }

        glDisable(GL_DEPTH_TEST);
        debugRenderer->drawPoint(pos);
        glEnable(GL_DEPTH_TEST);
    }
}

