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

void Editor::manageInput() {

    Input::beginFrame();

    Input::update();

    camera->manageInput();

    if (Input::isKeyDown(Qt::Key_Escape))
        QApplication::quit();
}

void Editor::renderFrame() {

    // Control del input
    manageInput();

    camera->setAspectRatio((float)win_w, (float)win_h);


    // Renderizado
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Creamos matriz MVP
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camera->getViewMatrix();
    glm::mat4 projection = camera->getProjectionMatrix();

    glm::mat4 MVP = projection * view * model;

    // Activamos shader al que le vamos a asignar las variables
    defaultShader->use();

    // Asignamos valores a las variables del shader
    glUniformMatrix4fv(glGetUniformLocation(defaultShader->getID(), "MVP"), 1, GL_FALSE, glm::value_ptr(MVP));
    glUniformMatrix4fv(glGetUniformLocation(defaultShader->getID(), "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform3f(glGetUniformLocation(defaultShader->getID(), "lightDir"), -0.5f, -1.0f, -0.3f);
    glUniform3f(glGetUniformLocation(defaultShader->getID(), "objectColor"), 0.6f, 0.7f, 1.0f);

    defaultMesh->draw();



    // Seleccion
    if (!meshManipulator->isDragging()) {

        // !! AVISO, SE PUEDE SELECCIONAR VERTICE NO VISIBLE
        selectedVertex = selector->pickVertex(*defaultMesh, Input::getMouseX(), Input::getMouseY(), win_w, win_h, camera);

    }

    // Debug
    if (selectedVertex != -1) {

        debugShader->use();

        glUniformMatrix4fv(glGetUniformLocation(debugShader->getID(), "MVP"), 1, GL_FALSE, glm::value_ptr(MVP));

        // Mostramos los vertices actualmente seleccionados
        glm::vec3 pos = defaultMesh->vertices[selectedVertex].Position;

        for (unsigned int group : meshManipulator->getSelectedGroups()) {
            for (unsigned int idx : defaultMesh->vertexGroups[group])
            {
                glm::vec3 pos = defaultMesh->vertices[idx].Position;
                debugRenderer->drawPoint(pos);
            }
        }

        // Para ver vertices ocultos que estamos moviendo
        glDisable(GL_DEPTH_TEST);
        debugRenderer->drawPoint(pos);
        glEnable(GL_DEPTH_TEST);
    }

    // Input
    if (Input::isMouseButtonDown(0) && !meshManipulator->isDragging()) {

        // Shift = selección aditiva
        bool additive = Input::isKeyDown(Qt::Key_Shift);

        // Si hemos hecho click sobre un vértice
        if (selectedVertex != -1) {

            // Actualiza la selección por grupos
            meshManipulator->selectVertex(defaultMesh, selectedVertex, additive);

            // Comienza el arrastre usando el vértice clicado (solo se usa para calcular dragStartPoint)
            meshManipulator->beginDrag(defaultMesh, selectedVertex, *camera);
        }
        else {
            // Click en el vacío sin shift limpia la selección
            if (!additive) {
                meshManipulator->clearSelection();
            }
        }
    }

    if (meshManipulator->isDragging() && !Input::isMouseButtonDown(0)) {
        meshManipulator->endDrag();
    }

    // Arrastrar puntos
    if (meshManipulator->isDragging()) {
        meshManipulator->updateDrag(defaultMesh, Input::getMouseX(), Input::getMouseY(), win_w, win_h, *camera);
    }

    if (Input::isKeyDown(Qt::Key_Control) && Input::isKeyDown(Qt::Key_S)) { // !! probar si sigue funcionando
        defaultMesh->saveOBJ("Assets/edited.obj");
    }
}

void Editor::setWindowSize(int w, int h) {
    win_w = w;
    win_h = h;
}

