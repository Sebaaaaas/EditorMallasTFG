#include "Shader.h"

#include <iostream>
#include <glad/gl.h>

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) // !! CUIDADO, SI FALLA CONSTRUCTORA DEJA LEAK, MEJOR CON INICIALIZACION EN DOS FASES
{
    std::string vertexCode = readFile(vertexPath);
    std::string fragmentCode = readFile(fragmentPath);

    unsigned int vertex = compileShader(vertexCode, GL_VERTEX_SHADER);
    unsigned int fragment = compileShader(fragmentCode, GL_FRAGMENT_SHADER);

    shaderID = glCreateProgram();
    glAttachShader(shaderID, vertex);
    glAttachShader(shaderID, fragment);
    glLinkProgram(shaderID);

    int success;
    char infoLog[1024];

    glGetProgramiv(shaderID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderID, 1024, NULL, infoLog);
        std::cout << "Error al compilar shader: " << shaderID << "\n" << infoLog << std::endl;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::~Shader()
{
    //glDeleteProgram(shaderID); // You might want to do this if you wish to live edit, and recreate your shaders interactively while your program is still running. - https://antongerdelan.net/opengl/
}

void Shader::use() const
{
    glUseProgram(shaderID);
}

std::string Shader::readFile(const std::string& path) {

    FILE* file = nullptr;

    if (fopen_s(&file, path.c_str(), "rb"))
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

    std::string result(fileSize, '\0');

    size_t bytesRead = fread(&result[0], 1, fileSize, file);
    fclose(file);

    if (bytesRead != static_cast<size_t>(fileSize))
    {
        std::cout << "Error leyendo fichero de shader: " << path << std::endl;
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
        std::cout << "Error al compilar shader con indice: " << shader << "\n" << slog << std::endl;

        return 1;
    }

    return shader;
}