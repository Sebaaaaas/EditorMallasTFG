#include "Editor.h"

#include <iostream>

#include "Shader.h"

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

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

}


float points[] = {
   0.0f,  0.5f,  0.0f, // x,y,z of first point.
   0.5f, -0.5f,  0.0f, // x,y,z of second point.
  -0.5f, -0.5f,  0.0f  // x,y,z of third point.

};

bool Editor::init()
{
	if (!glfwInit())
		return false;

	// Deteccion de errores
    glfwSetErrorCallback(error_callback);

    // Inicializacion de la ventana
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    windowTitle = "Mesh editor";
    window = glfwCreateWindow(640, 480, windowTitle.c_str(), NULL, NULL);

    if (!window) {
        // Window or OpenGL context creation failed
        glfwTerminate();

        return false;
    }

    glfwMakeContextCurrent(window);

    // Deteccion de pulsaciones para esta ventana, que glfwPollEvents captura
    glfwSetKeyCallback(window, key_callback);

    glfwSwapInterval(1);

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
    shader = new Shader("Bin/Assets/test.vert", "Bin/Assets/test.frag");

	return true;
}

void Editor::release() {
    glfwTerminate();
}

void Editor::run()
{
    //glEnable(GL_DEPTH_TEST); No se para que se usa esto
    
    // Vertex buffer object, podemos guardar un gran numero de vertices en la memoria de la GPU
    GLuint vbo = 0; // aqui guardaremos el id del buffer que vamos a usar
    glGenBuffers(1, &vbo); // Genera id's para buffers
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), points, GL_STATIC_DRAW); // Copia los vertices a la memoria del buffer, el ultimo parametro: 
                                                                              // GL_STREAM_DRAW: the data is set only once and used by the GPU at most a few times. 
                                                                              // GL_STATIC_DRAW: the data is set only once and used many times. 
                                                                              // GL_DYNAMIC_DRAW : the data is changed a lot and used many times.

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL); // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);, 0 es el "stride", o espacio entre valores, que como
                                                              // tenemos "tightly-packed", ponerlo a 0 se puede hacer y OpenGL sabe como separarlos, en caso contrario indicar con, por ejemplo,
                                                              // 3 * sizeof(float) (que tambien se podria poner en este caso)


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

    shader->use();

    while (!glfwWindowShouldClose(window)) {
        // Update window events.
        glfwPollEvents();

        // Wipe the drawing surface clear.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // Draw points 0-3 from the currently bound VAO with current in-use shader.
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Put the stuff we've been drawing onto the visible area.
        glfwSwapBuffers(window);
    }

}

