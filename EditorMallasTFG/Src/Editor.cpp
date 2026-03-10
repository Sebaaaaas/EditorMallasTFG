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

#include "Camera.h"
#include "Shader.h"
#include "Mesh.h"
#include "Input.h"

void drawDebugPoint(const glm::vec3& pos)
{
    glPointSize(10.0f);

    float v[3] = { pos.x, pos.y, pos.z };

    GLuint vao, vbo;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    glDrawArrays(GL_POINTS, 0, 1);

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

Editor::Editor()
{
    defaultMesh = nullptr;
    defaultShader = nullptr;
    camera = nullptr;
    window = nullptr;
}

Editor::~Editor()
{
    delete defaultShader;
    defaultShader = nullptr;

    delete defaultMesh;
    defaultMesh = nullptr;

    delete camera;
    camera = nullptr;

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
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    camera = new Camera((float)win_w, (float)win_h);

    defaultMesh = new Mesh(Mesh::loadOBJ("Assets/modelo.obj"));
    // Creacion de shader
    defaultShader = new Shader("Assets/testcube.vert", "Assets/testcube.frag");
    defaultShader->use();

	return true;
}

void Editor::run()
{
    while (!glfwWindowShouldClose(window)) {

        Input::beginFrame();

        glfwPollEvents();

        Input::update();

        camera->manageInput();

        if (Input::isKeyDown(GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(window, GLFW_TRUE);

        

        // Manejo de redimensionamiento de la pantalla
        glfwGetWindowSize(window, &win_w, &win_h);
        glViewport(0, 0, win_w, win_h);

        camera->setAspectRatio((float)win_w, (float)win_h);




        // Renderizado
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Create MVP matrix
        double time = glfwGetTime();

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera->getViewMatrix();
        glm::mat4 projection = camera->getProjectionMatrix();

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

        defaultMesh->draw(*defaultShader);



        glm::vec3 mouseCastRay = mouseClickRay(Input::getMouseX(), Input::getMouseY(), win_w, win_h, camera->getViewMatrix(), camera->getProjectionMatrix());
        glm::vec3 rayOrigin = camera->getPosition();

        int selectedVertex = -1;
        float minDist = 0.05f;

        for (int i = 0; i < defaultMesh->vertices.size(); i++)
        {
            glm::vec3 v(defaultMesh->vertices[i].Position[0], defaultMesh->vertices[i].Position[1], defaultMesh->vertices[i].Position[2]);

            float dist = pointToRayDistance(v, rayOrigin, mouseCastRay);

            if (dist < minDist)
            {
                minDist = dist;
                selectedVertex = i;
                defaultMesh->selectedVertex = i;
            }
        }
        if (defaultMesh->selectedVertex != -1)
        {
            glm::vec3 pos(defaultMesh->vertices[defaultMesh->selectedVertex].Position[0], defaultMesh->vertices[defaultMesh->selectedVertex].Position[1], defaultMesh->vertices[defaultMesh->selectedVertex].Position[2]);

            drawDebugPoint(pos);
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

// https://antongerdelan.net/opengl/raycasting.html
glm::vec3 Editor::mouseClickRay(float mouseX, float mouseY, int w, int h, glm::mat4 view, glm::mat4 proj)
{
    // Dejamos los valores en los rangos [-1...1], invirtiendo la y por ir al reves en OpenGL. Z es innecesaria
    float x = (2.0f * mouseX) / w - 1.0f;
    float y = 1.0f - (2.0f * mouseY) / h;

    // Apuntamos hacia delante, que en OpenGL es negativo en el eje Z, y convertimos a vec4 para posteriores calculos
    glm::vec4 rayClip = glm::vec4(x, y, -1.0, 1.0);

    // Coordenadas de la camara
    glm::vec4 rayEye = glm::inverse(proj) * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0, 0.0);

    // Coordenadas en el mundo
    glm::vec3 rayWorld = glm::normalize(glm::vec3(glm::inverse(view) * rayEye));

    return rayWorld;
}

float Editor::pointToRayDistance(glm::vec3 point, glm::vec3 rayOrigin, glm::vec3 rayDir)
{
    glm::vec3 diff = point - rayOrigin;
    glm::vec3 proj = glm::dot(diff, rayDir) * rayDir;

    return glm::length(diff - proj);
}

