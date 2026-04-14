#include "Editor.h"

#define GLFW_INCLUDE_NONE

#include <glad/gl.h>
// GLFW include siempre despues de glad
#include <GLFW/glfw3.h>

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

#include <iostream>

Editor::Editor()
{
    defaultMesh = nullptr;
    defaultShader = nullptr;
    debugShader = nullptr;

    camera = nullptr;
    window = nullptr;
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


    //glfwDestroyWindow(window); glfwTerminate cierra y borra todas las ventanas, en este caso es suficiente y no hace falta esto
    glfwTerminate();
}

static void error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW ERROR: code %i msg: %s.\n", error, description);
}

bool Editor::init()
{
	// Deteccion de errores
    glfwSetErrorCallback(error_callback);

    // Inicializacion de la GLWF y ventana que usara
    if (!initializeGLFWAndWindow())
        return false;

    // Deteccion de input
    Input::init(window);

    // Inicializamos glad para poder llamar a funciones de OpenGL
    if (!initializeGlad())
        return false;

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f); // magicky number !

    camera = new Camera((float)win_w, (float)win_h);

    defaultMesh = new Mesh(Mesh::loadOBJ("Assets/cubo.obj"));

    // Creacion de shader
    defaultShader = new Shader("Assets/testcube.vert", "Assets/testcube.frag");
    debugShader = new Shader("Assets/debugShader.vert", "Assets/debugShader.frag");

    selector = new Selector();
    meshManipulator = new MeshManipulator();
    debugRenderer = new DebugRenderer();

	return true;
}

void Editor::run()
{
    while (!glfwWindowShouldClose(window)) {

        // Input
        manageInput();

        // Manejo de redimensionamiento de la pantalla
        glfwGetWindowSize(window, &win_w, &win_h);
        glViewport(0, 0, win_w, win_h);

        camera->setAspectRatio((float)win_w, (float)win_h);


        // Renderizado
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //defaultShader->use();

        // Creamos matriz MVP
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera->getViewMatrix();
        glm::mat4 projection = camera->getProjectionMatrix();

        glm::mat4 MVP = projection * view * model;

        // Asignamos valores al defaultShader
        glUniformMatrix4fv(glGetUniformLocation(defaultShader->getID(), "MVP"), 1, GL_FALSE, glm::value_ptr(MVP));
        glUniformMatrix4fv(glGetUniformLocation(defaultShader->getID(), "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniform3f(glGetUniformLocation(defaultShader->getID(), "lightDir"), -0.5f, -1.0f, -0.3f);
        glUniform3f(glGetUniformLocation(defaultShader->getID(), "objectColor"), 0.6f, 0.7f, 1.0f);

        defaultMesh->draw(*defaultShader);

        selectedVertex = selector->pickVertex(*defaultMesh, Input::getMouseX(), Input::getMouseY(), win_w, win_h, camera);
        //std::cout << selectedVertex << std::endl;

        // Input
        if (Input::isMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT) && !meshManipulator->isDragging()) {  
            meshManipulator->beginDrag(defaultMesh, selectedVertex, *camera);
        }
        
        if (meshManipulator->isDragging() && !Input::isMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT)) {
            meshManipulator->endDrag();
            selectedVertex = -1;
        }

        // Arrastrar punto
        if (meshManipulator->isDragging()) {
            //std::cout << "Dragging" << std::endl;
            meshManipulator->updateDrag(defaultMesh, Input::getMouseX(), Input::getMouseY(), win_w, win_h, *camera);
        }

        
        //std::cout << selectedVertex << std::endl;
        if (selectedVertex != -1) {
            glm::vec3 v(defaultMesh->vertices[selectedVertex].Position);

            //debugShader->use();

            glUniformMatrix4fv(glGetUniformLocation(debugShader->getID(), "MVP"), 1, GL_FALSE, glm::value_ptr(MVP));

            debugRenderer->drawPoint(v);
        }        

        if (Input::isKeyDown(GLFW_KEY_LEFT_CONTROL) && Input::isKeyDown(GLFW_KEY_S)) {
            defaultMesh->saveOBJ("Assets/edited.obj");
        }

        glfwSwapBuffers(window);
    }

}

bool Editor::initializeGLFWAndWindow()
{
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: no se ha podido inicializar GLFW3.\n");
        return false;
    }

    // Estos hints se pueden llamar antes o despues de glfwInit, y no tienen efecto hasta la siguiente vez que la libreria se cierra y reinicializa
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Ayuda a evitar usar partes antiguas no soportadas de la API sin querer
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 8); // Cómo de "lisas" son las lineas diagonales

    window = glfwCreateWindow(win_w, win_h, windowTitle.c_str(), NULL, NULL);

    if (!window) {
        // Window or OpenGL context creation failed
        glfwTerminate();

        return false;
    }

    glfwMakeContextCurrent(window);

    glfwSwapInterval(1); // FPS limitados a velocidad de refresco del monitor(valor 1), a 0 para que sea inmediato(causa posible "tearing")

    return true;
}

bool Editor::initializeGlad()
{
    int version_glad = gladLoadGL(glfwGetProcAddress);
    
    if (version_glad == 0) {
        fprintf(stderr, "ERROR: Fallo en inicializacion de glad\n");
        return false;
    }

    return true;
}

void Editor::manageInput() {

    Input::beginFrame();

    glfwPollEvents();

    Input::update();

    camera->manageInput();

    if (Input::isKeyDown(GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

