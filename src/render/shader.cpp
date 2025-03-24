#include "render/shader.h"
#include <iostream>
#include <fstream>
#include <sstream>

Shader::Shader() : programID(0) {}

Shader::~Shader() {
    if (programID != 0) {
        glDeleteProgram(programID);
    }
}

void Shader::SetShadersFileName(const std::string &vFileName, const std::string &sFileName)
{
    vertexShaderFileName = vFileName;
    fragmentShaderFileName = sFileName;
}

void Shader::CheckShaderCompileStatus(GLuint shader)
{
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        GLchar* buffer = new GLchar[logLength];
        GLsizei bufferSize;
        glGetShaderInfoLog(shader, logLength, &bufferSize, buffer);
        std::cerr << "Shader Compilation Failed: " << std::endl << buffer << std::endl;
        delete[] buffer;
    }
    // else
    // {
    //     std::cout << "Shader Compilation Successful." << std::endl;
    // }
}

void Shader::Init()
{
    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

    std::string vstr = ReadShader(vertexShaderFileName);
    std::string fstr = ReadShader(fragmentShaderFileName);

    if (vstr.empty() || fstr.empty()) {
        std::cerr << "Shader source code is empty, check file paths." << std::endl;
        return;
    }

    const char* vsText = vstr.c_str();
    glShaderSource(vertShader, 1, &vsText, nullptr);
    const char* fsText = fstr.c_str();
    glShaderSource(fragShader, 1, &fsText, nullptr);

    glCompileShader(vertShader);
    // std::cout << "Vertex shader compilation: ";
    // CheckShaderCompileStatus(vertShader);

    glCompileShader(fragShader);
    // std::cout << "Fragment shader compilation: ";
    // CheckShaderCompileStatus(fragShader);

    programID = glCreateProgram();
    glAttachShader(programID, vertShader);
    glAttachShader(programID, fragShader);

    glLinkProgram(programID);
    GLint status;
    glGetProgramiv(programID, GL_LINK_STATUS, &status);
    if (!status) {
        GLint logLength;
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0) {
            std::vector<char> log(logLength);
            glGetProgramInfoLog(programID, logLength, nullptr, log.data());
            std::cerr << "Shader Program Linking Error: " << std::endl << log.data() << std::endl;
        }
        return;
    }

    // Cleanup shaders after linking
    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
}

std::string Shader::ReadShader(const std::string &name)
{
    std::ifstream ifs(name);
    if (!ifs) {
        std::cerr << "Failed to open the shader file: " << name << std::endl;
        return "";
    }

    std::stringstream ss;
    ss << ifs.rdbuf();
    return ss.str();
}

void Shader::SendAttributeData(std::vector<float>& buffer, const char* name)
{
    if (buffer.empty()) {
        std::cerr << "Empty buffer for attribute: " << name << std::endl;
        return;
    }

    GLuint bufferID = 0;

    // Check if the buffer exists for the given attribute name
    if (bufferMap.find(name) == bufferMap.end()) {
        glGenBuffers(1, &bufferID);  // Generate a new buffer
        bufferMap[name] = bufferID;  // Store the buffer ID in the map
    } else {
        bufferID = bufferMap[name];  // Reuse the existing buffer ID
    }

    glBindBuffer(GL_ARRAY_BUFFER, bufferID);  // Bind the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * buffer.size(), buffer.data(), GL_STATIC_DRAW);  // Update buffer data

    GLint aLoc = glGetAttribLocation(programID, name);  // Get attribute location
    if (aLoc == -1) {
        std::cerr << "Attribute " << name << " not found in shader program." << std::endl;
        return;
    }

    glEnableVertexAttribArray(aLoc);  // Enable the attribute
    glVertexAttribPointer(aLoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);  // Set the pointer for vertex attributes

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}





void Shader::SendUniformData(int input, const char* name)
{
    glUniform1i(glGetUniformLocation(programID, name), input);
}

void Shader::SendUniformData(float input, const char* name)
{
    glUniform1f(glGetUniformLocation(programID, name), input);
}

void Shader::SendUniformData(glm::vec3 input, const char* name)
{
    glUniform3f(glGetUniformLocation(programID, name), input.x, input.y, input.z);
}

void Shader::SendUniformData(glm::mat4 &input, const char* name)
{
    glUniformMatrix4fv(glGetUniformLocation(programID, name), 1, GL_FALSE, &input[0][0]);
}

void Shader::Bind()
{
    glUseProgram(programID);
}

void Shader::Unbind()
{
    glUseProgram(0);
}