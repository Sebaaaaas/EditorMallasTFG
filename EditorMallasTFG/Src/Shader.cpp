#include "Shader.h"

#include <iostream>
#include <glad/gl.h>


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

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) // CUIDADO, SI FALLA CONSTRUCTORA DEJA LEAK, MEJOR CON INICIALIZACION EN DOS FASES
{
    std::string vertexCode = readFile(vertexPath);
    std::string fragmentCode = readFile(fragmentPath);

    unsigned int vertex = compileShader(vertexCode, GL_VERTEX_SHADER);
    unsigned int fragment = compileShader(fragmentCode, GL_FRAGMENT_SHADER);

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);

    int success;
    char infoLog[1024];

    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        int max_length = 2048, actual_length = 0;
        char plog[2048];
        glGetProgramInfoLog(ID, max_length, &actual_length, plog);
        fprintf(stderr, "ERROR: Could not link shader program GL index %u.\n%s\n", ID, plog);
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::~Shader()
{
    //glDeleteProgram(ID);
}

void Shader::use() const
{
    glUseProgram(ID);
}

std::string Shader::readFile(const std::string& path) {
    FILE* file = nullptr;

    errno_t err = fopen_s(&file, path.c_str(), "rb");
    if (err != 0 || file == nullptr)
    {
        std::cout << "Error al intentar abrir el fichero: " << path << std::endl;
        return "";
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    if (fileSize <= 0)
    {
        fclose(file);
        return "";
    }

    // Allocate string with correct size
    std::string result(fileSize, '\0');

    size_t bytesRead = fread(&result[0], 1, fileSize, file);
    fclose(file);

    if (bytesRead != static_cast<size_t>(fileSize))
    {
        std::cout << "Error reading shader file: " << path << std::endl;
        return "";
    }

    return result;
}

unsigned int Shader::compileShader(const std::string& source, unsigned int type)
{
    const char* src = source.c_str();

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    // Comprobacion de si ha tenido exito la compilacion del shader
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        int max_length = 2048, actual_length = 0;
        char slog[2048];
        glGetShaderInfoLog(shader, max_length, &actual_length, slog);
        fprintf(stderr, "ERROR: Shader index %u did not compile.\n%s\n", shader, slog);
        return 1;
    }

    return shader;
}