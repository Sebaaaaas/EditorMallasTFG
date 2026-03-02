#include "Editor.h"

#include <iostream>
#include <assert.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "Shader.h"
#include "Mesh.h"

Editor::Editor()
{
    shader = nullptr;
    window = nullptr;
}

Editor::~Editor()
{
    delete shader;
    shader = nullptr;
}

static void error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW ERROR: code %i msg: %s.\n", error, description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(window, GLFW_TRUE);

}

bool Editor::init()
{
	// Deteccion de errores
    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW3.\n");
		return false;
    }

    // Inicializacion de la ventana
    if (!initializeWindow()) {
        glfwTerminate();

        return false;
    }

    // Deteccion de pulsaciones para esta ventana, que glfwPollEvents captura
    glfwSetKeyCallback(window, key_callback);

    glfwSwapInterval(1); // FPS limitados a velocidad de refresco del monitor, a 0 para que sea inmediato(causa posible "tearing")

    // Inicializamos glad para poder llamar a funciones de OpenGL
    int version_glad = gladLoadGL(glfwGetProcAddress);
    if (version_glad == 0) {
        fprintf(stderr, "ERROR: Failed to initialize OpenGL context.\n");
        return 1;
    }
    printf("Loaded OpenGL %i.%i\n", GLAD_VERSION_MAJOR(version_glad), GLAD_VERSION_MINOR(version_glad));

    // Ejemplo de funciones de OpenGL, dandonos informacion sobre nuestro sistema y version OpenGL
    printf("Renderer: %s.\n", glGetString(GL_RENDERER));
    printf("OpenGL version supported %s.\n", glGetString(GL_VERSION));


    // Creacion de shader
    shader = new Shader("Assets/testcube.vert", "Assets/testcube.frag");

	return true;
}

void Editor::release() {
    glfwTerminate();
}

void Editor::run()
{
    //glEnable(GL_DEPTH_TEST); No se para que se usa esto
    
    // Vertex buffer object, podemos guardar un gran numero de vertices en la memoria de la GPU
    //GLuint vbo = 0; // aqui guardaremos el id del buffer que vamos a usar
    //glGenBuffers(1, &vbo); // Genera id's para buffers
    //glBindBuffer(GL_ARRAY_BUFFER, vbo);
    //glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), points, GL_STATIC_DRAW); // Copia los vertices a la memoria del buffer, el ultimo parametro: 
    //                                                                          // GL_STREAM_DRAW: the data is set only once and used by the GPU at most a few times. 
    //                                                                          // GL_STATIC_DRAW: the data is set only once and used many times. 
    //                                                                          // GL_DYNAMIC_DRAW : the data is changed a lot and used many times.

    //GLuint vao = 0;
    //glGenVertexArrays(1, &vao);
    //glBindVertexArray(vao);
    //glEnableVertexAttribArray(0);
    //glBindBuffer(GL_ARRAY_BUFFER, vbo);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL); // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);, 0 es el "stride", o espacio entre valores, que como
    //                                                          // tenemos "tightly-packed", ponerlo a 0 se puede hacer y OpenGL sabe como separarlos, en caso contrario indicar con, por ejemplo,
    //                                                          // 3 * sizeof(float) (que tambien se podria poner en este caso)


    //// Creacion de un programa de shader, donde linkeamos nuestros shaders anteriores
    //GLuint shader_program = glCreateProgram();
    //glAttachShader(shader_program, vs);
    //glAttachShader(shader_program, fs);
    //glLinkProgram(shader_program);

    //// Comprobacion de si hemos tenido exito
    //glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    //if (!success) {
    //    glGetProgramInfoLog(shader_program, 512, NULL, infoLog);
    //    std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;

    //}

    //// Una vez linkeados, los podemos borrar
    //glDeleteShader(vs);
    //glDeleteShader(fs);

    //// Put the shader program, and the VAO, in focus in OpenGL's state machine.
    //glUseProgram(shader_program);
    //glBindVertexArray(vao);

    Mesh cube = Mesh::LoadOBJ("Assets/modelo.obj");

    //glEnable(GL_DEPTH_TEST);

    shader->use();

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    while (!glfwWindowShouldClose(window)) {

        //double curr_s = glfwGetTime(); // Get the current time. 

        //int time_loc = glGetUniformLocation(shader->getID(), "time"); // Para mover triangulo
        //assert(time_loc > -1);

        // Update window events.
        glfwPollEvents();

        // Manejo de redimensionamiento de la pantalla
        glfwGetWindowSize(window, &win_w, &win_h);
        glViewport(0, 0, win_w, win_h);

        // Wipe the drawing surface clear.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        //glUniform1f(time_loc, (float)curr_s);
        //glBindVertexArray(vao);
        // Draw points 0-3 from the currently bound VAO with current in-use shader.
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        // Create MVP matrix
        float time = glfwGetTime();

        glm::mat4 model = glm::mat4(1.0f);

        model = glm::rotate(model,
            time,
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

        glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "MVP"),
            1, GL_FALSE, glm::value_ptr(MVP));

        glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "model"),
            1, GL_FALSE, glm::value_ptr(model));

        // Sun coming from top-right
        glUniform3f(glGetUniformLocation(shader->getID(), "lightDir"),
            -0.5f, -1.0f, -0.3f);

        glUniform3f(glGetUniformLocation(shader->getID(), "objectColor"),
            0.6f, 0.7f, 1.0f);

        cube.Draw(*shader);

        // Put the stuff we've been drawing onto the visible area.
        glfwSwapBuffers(window);
    }

}

bool Editor::initializeWindow()
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Ayuda a evitar usar partes antiguas no soportadas de la API sin querer
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 8); // Como de "lisas" son las lineas diagonales

    window = glfwCreateWindow(win_w, win_h, windowTitle.c_str(), NULL, NULL);

    if (!window) {
        // Window or OpenGL context creation failed
        glfwTerminate();

        return false;
    }

    glfwMakeContextCurrent(window);

    return true;
}

