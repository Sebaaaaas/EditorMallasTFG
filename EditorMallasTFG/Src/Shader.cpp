#include "Shader.h"

#include <cstdio>
#include <iostream>
#include <string>

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
        std::cout << "Failed to open shader file: " << path << std::endl;
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
    int  success;
    char infoLog[512];
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

void Shader::setBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}