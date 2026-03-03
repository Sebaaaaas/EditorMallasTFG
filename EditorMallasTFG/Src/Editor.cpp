#include "Editor.h"

#define GLFW_INCLUDE_NONE

#include <glad/gl.h>
// GLFW include siempre despues de glad
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <stdio.h>
#include <assert.h>

#include "Shader.h"
#include "Mesh.h"

Editor::Editor()
{
    defaultMesh = nullptr;
    defaultShader = nullptr;
    window = nullptr;
}

Editor::~Editor()
{
    delete defaultShader;
    defaultShader = nullptr;

    delete defaultMesh;
    defaultMesh = nullptr;

    //glfwDestroyWindow(window); glfwTerminate cierra y borra todas las ventanas, en este caso es suficiente y no hace falta esto
    glfwTerminate();
}

static void error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW ERROR: code %i msg: %s.\n", error, description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(window, GLFW_TRUE);

}

bool Editor::init()
{
	// Deteccion de errores
    glfwSetErrorCallback(error_callback);

    // Inicializacion de la GLWF y ventana que usara
    if (!initializeGLFWAndWindow())
        return false;

    // Deteccion de pulsaciones para esta ventana, que glfwPollEvents captura
    glfwSetKeyCallback(window, key_callback);

    // Inicializamos glad para poder llamar a funciones de OpenGL
    if (!initializeGlad())
        return false;

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);


    defaultMesh = new Mesh(Mesh::LoadOBJ("Assets/modelo.obj"));
    // Creacion de shader
    defaultShader = new Shader("Assets/testcube.vert", "Assets/testcube.frag");
    defaultShader->use();

	return true;
}

void Editor::run()
{
    while (!glfwWindowShouldClose(window)) {

        // Update window events.
        glfwPollEvents();

        // Manejo de redimensionamiento de la pantalla
        glfwGetWindowSize(window, &win_w, &win_h);
        glViewport(0, 0, win_w, win_h);

        // Wipe the drawing surface clear.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Create MVP matrix
        double time = glfwGetTime();

        glm::mat4 model = glm::mat4(1.0f);

        model = glm::rotate(model,
            (float)time,
            glm::vec3(0.0f, 1.0f, 0.0f));

        model = glm::scale(model,
            glm::vec3(0.4f));

        glm::mat4 view = glm::translate(glm::mat4(1.0f),
            glm::vec3(0.0f, -1.0f, -5.0f));

        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            (float)win_w / win_h,
            0.1f,
            100.0f
        );

        glm::mat4 MVP = projection * view * model;

        glUniformMatrix4fv(glGetUniformLocation(defaultShader->getID(), "MVP"),
            1, GL_FALSE, glm::value_ptr(MVP));

        glUniformMatrix4fv(glGetUniformLocation(defaultShader->getID(), "model"),
            1, GL_FALSE, glm::value_ptr(model));

        // Sun coming from top-right
        glUniform3f(glGetUniformLocation(defaultShader->getID(), "lightDir"),
            -0.5f, -1.0f, -0.3f);

        glUniform3f(glGetUniformLocation(defaultShader->getID(), "objectColor"),
            0.6f, 0.7f, 1.0f);

        defaultMesh->Draw(*defaultShader);

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

