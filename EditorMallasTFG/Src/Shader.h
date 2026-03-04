#pragma once

#include <string>

class Shader
{
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);

    ~Shader();

    void use() const;

    unsigned int getID() const { return shaderID; }

private:
    unsigned int shaderID;

    std::string readFile(const std::string& path);
    unsigned int compileShader(const std::string& source, unsigned int type);
};