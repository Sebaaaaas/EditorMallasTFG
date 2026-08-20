#pragma once

#include <string>

#include <unordered_map>
#include <glm/glm.hpp>

class Shader
{
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);

    ~Shader();

    void use() const;

    unsigned int getID() const { return shaderID; }

    unsigned int getUniformLocation(const std::string& name);

    void setMat4(const std::string& name, const glm::mat4& matrix);
    void setVec3(const std::string& name, const glm::vec3& value);
    void setFloat(const std::string& name, float value);
    void setInt(const std::string& name, int value);

private:
    unsigned int shaderID;

    std::unordered_map<std::string, unsigned int> uniformLocations;

    std::string readFile(const std::string& path);

    // Compilamos shader encontrado en "source" y devolvemos su indice para identificarlo
    unsigned int compileShader(const std::string& source, unsigned int type);
};