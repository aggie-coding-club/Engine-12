#include "render/shader.h"
#include <iostream>
#include <fstream>
#include <sstream>

Shader::Shader() : shaderID(0) {}

Shader::~Shader() {
    if (shaderID != 0) {
        glDeleteProgram(shaderID);
    }
}

void Shader::UpdateData() {
    this->SendUniformData(1.f,"tempcolor");
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
    if (status == GL_FALSE) {
        GLint logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        GLchar* buffer = new GLchar[logLength];
        GLsizei bufferSize;
        glGetShaderInfoLog(shader, logLength, &bufferSize, buffer);
        std::cerr << "Shader Compilation Failed: " << std::endl << buffer << std::endl;
        delete[] buffer;
    } else {
        std::cout << "Shader Compilation Successful." << std::endl;
    }
}

void Shader::Init(std::shared_ptr<Scene> _Scene){

    scene = _Scene;

    // Step 1: Create shader objects
    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

    // Step 2: Load and set shader source
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

    // Step 3: Compile shaders
    glCompileShader(vertShader);
    if (!CheckShaderCompileStatus(vertShader, "Vertex Shader")) {
        glDeleteShader(vertShader);
        return;
    }

    glCompileShader(fragShader);
    if (!CheckShaderCompileStatus(fragShader, "Fragment Shader")) {
        glDeleteShader(vertShader);
        glDeleteShader(fragShader);
        return;
    }

    // Step 4: Link shaders into a program
    shaderID = glCreateProgram();
    glAttachShader(shaderID, vertShader);
    glAttachShader(shaderID, fragShader);

    // Bind attribute locations before linking
    glBindAttribLocation(shaderID, 0, "aPosition");    // Vertex position
    glBindAttribLocation(shaderID, 1, "aTexCoord");    // Texture coordinates
    glBindAttribLocation(shaderID, 2, "aNormal");      // Vertex normal

    glLinkProgram(shaderID);
    if (!CheckProgramLinkStatus(shaderID)) {
        glDeleteShader(vertShader);
        glDeleteShader(fragShader);
        glDeleteProgram(shaderID);
        return;
    }

    // Cleanup shaders after successful linking
    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    // Step 5: Use the program (optional)
    glUseProgram(shaderID);

    std::cout << "Shader program initialized successfully!" << std::endl;
}

// Helper: Check Shader Compilation
bool Shader::CheckShaderCompileStatus(GLuint shader, const std::string& shaderName) {
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0) {
            std::vector<char> log(logLength);
            glGetShaderInfoLog(shader, logLength, nullptr, log.data());
            std::cerr << shaderName << " Compilation Error: " << std::endl << log.data() << std::endl;
        }
        return false;
    }
    return true;
}

// Helper: Check Program Linking
bool Shader::CheckProgramLinkStatus(GLuint program) {
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLint logLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0) {
            std::vector<char> log(logLength);
            glGetProgramInfoLog(program, logLength, nullptr, log.data());
            std::cerr << "Shader Program Linking Error: " << std::endl << log.data() << std::endl;
        }
        return false;
    }
    return true;
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

void Shader::SendBufferData(std::vector<float>& buffer, const char* name)
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

    GLint aLoc = glGetAttribLocation(shaderID, name);  // Get attribute location
    if (aLoc == -1) {
        std::cerr << "Attribute " << name << " not found in shader program." << std::endl;
        return;
    }

    glEnableVertexAttribArray(aLoc);  // Enable the attribute
    glVertexAttribPointer(aLoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);  // Set the pointer for vertex attributes
}

void Shader::SendUniformData(int input, const char* name)
{
    glUniform1i(glGetUniformLocation(shaderID, name), input);
}

void Shader::SendUniformData(float input, const char* name)
{
    glUniform1f(glGetUniformLocation(shaderID, name), input);
}

void Shader::SendUniformData(glm::vec2 input, const char* name)
{
    glUniform2f(glGetUniformLocation(shaderID, name), input.x, input.y);
}

void Shader::SendUniformData(glm::vec3 input, const char* name)
{
    glUniform3f(glGetUniformLocation(shaderID, name), input.x, input.y, input.z);
}

void Shader::SendUniformData(glm::mat4 &input, const char* name)
{
    glUniformMatrix4fv(glGetUniformLocation(shaderID, name), 1, GL_FALSE, &input[0][0]);
}

void Shader::Bind()
{
    glUseProgram(shaderID);
}

void Shader::Unbind()
{
    glUseProgram(0);
}