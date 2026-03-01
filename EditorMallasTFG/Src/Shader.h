#pragma once

#include <string>
#include <glad/gl.h>

class Shader
{
public:
    Shader(const std::string& vertexPath,
        const std::string& fragmentPath);

    ~Shader();

    void use() const;

    // Uniform helpers
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;

    unsigned int getID() const { return ID; }

private:
    unsigned int ID;

    std::string readFile(const std::string& path);
    unsigned int compileShader(const std::string& source, unsigned int type);
};