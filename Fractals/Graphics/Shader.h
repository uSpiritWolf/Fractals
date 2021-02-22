#pragma once
#include <string>
#include <map>

#include "gl/glew.h"
#include "Shader/Uniform.h"

class Shader
{
public:
	Shader();
	~Shader();

	void Load(const GLchar* const vertexShaderSource, const GLchar* const fragmentShaderSource);

	void Bind();
	void Unbind();

	Uniform& RegisterUniform(const std::string& name);

	Uniform& operator[](const std::string& nameUniform);

private:

	static GLuint CompileShader(const GLchar* const source, GLenum type);
	static GLuint CompileProgramShader(const GLchar* const vertexShaderSource, const GLchar* const fragmentShaderSource);

	GLuint m_programShader;

	std::map<std::string, Uniform> m_uniforms;
};

inline void Shader::Unbind()
{
	glUseProgram(0);
}