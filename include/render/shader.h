#pragma once
#include "GL/glew.h"
#include "core/game_engine.h"
#include <unordered_map>

class Shader
{
	GLint shaderID;
	std::string vertexShaderFileName, fragmentShaderFileName;
	std::unordered_map<std::string, GLuint> bufferMap;
	static bool CheckShaderCompileStatus(GLuint shader, const std::string& shaderName);
	static bool CheckProgramLinkStatus(GLuint program);

public:
	Shader();
	~Shader();
	void UpdateData();
	void SetShadersFileName(const std::string &vFileName, const std::string &sFileName);
	void CheckShaderCompileStatus(GLuint shader);
	void Init(std::shared_ptr<Scene> _Scene);
	std::string ReadShader(const std::string &name);
	std::shared_ptr<Scene> scene;

	void SendUniformData(glm::vec2 input, const char* name);
	void SendBufferData(std::vector<float>& buffer, const char* name);
	void SendUniformData(int a, const char* name);
	void SendUniformData(float a, const char* name);
	void SendUniformData(glm::vec3 input, const char* name);
	void SendUniformData(glm::mat4 &mat, const char* name);
	void Bind();
	void Unbind();
};

