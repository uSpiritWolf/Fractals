#include "Shader.h"

#include <vector>
#include "Logger/Logger.h"

Shader::Shader()
	: m_programShader(0)
{
}

Shader::~Shader()
{
	if (m_programShader)
	{
		glDeleteProgram(m_programShader);
	}
}

void Shader::Load(const GLchar* const vertexShaderSource, const GLchar* const fragmentShaderSource)
{
	m_programShader = CompileProgramShader(vertexShaderSource, fragmentShaderSource);
}

void Shader::Bind()
{
	glUseProgram(m_programShader);
	for (auto it = m_uniforms.begin(); it != m_uniforms.end(); ++it)
	{
		Uniform& uniform = it->second;
		if (uniform.IsValidLocation() && uniform.IsValidType())
		{
			uniform.Apply();
		}
		else if (!uniform.IsValidType())
		{
			Logger::Log(LogLevel::DEBUG, "Uniform is not set: " + it->first);
		}
		else
		{
			Logger::Log(LogLevel::DEBUG, "No uniform found in shader: " + it->first);
		}
	}
}

Uniform& Shader::RegisterUniform(const std::string& name)
{
	GLint location = glGetUniformLocation(m_programShader, name.c_str());
	auto pair = m_uniforms.emplace(std::piecewise_construct, std::forward_as_tuple(name), std::forward_as_tuple(location));
	return pair.first->second;
}

Uniform& Shader::operator[](const std::string& nameUniform)
{
	auto it = m_uniforms.find(nameUniform);
	if (it == m_uniforms.end())
	{
		return RegisterUniform(nameUniform);
	}
	return it->second;
}

GLuint Shader::CompileShader(const GLchar* const source, GLenum type)
{
	const GLint shader = glCreateShader(type);

	if (!shader)
	{
		Logger::Log(LogLevel::ERR, "Cannot create a shader of type");
		return 0;
	}

	const GLint size = static_cast<GLint>(strlen(source));
	glShaderSource(shader, 1, &source, &size);
	glCompileShader(shader);

	{
		GLint compiled;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		if (!compiled)
		{
			GLint maxLength = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> errorLog(maxLength);
			glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);
			std::string error(errorLog.begin(), errorLog.end());

			Logger::Log(LogLevel::ERR, "Cannot compile shader:\n" + error);
			return 0;
		}
	}

	return shader;
}

GLuint Shader::CompileProgramShader(const GLchar* const vertexShaderSource, const GLchar * const fragmentShaderSource)
{
	const GLint program = glCreateProgram();

	if (!program)
	{
		Logger::Log(LogLevel::ERR, "Cannot create a shader program");
		return 0;
	}

	GLuint vertexShader = CompileShader(vertexShaderSource, GL_VERTEX_SHADER);
	GLuint fragmentShader = CompileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	{
		GLint linked;
		glGetShaderiv(program, GL_LINK_STATUS, &linked);
		if (!linked)
		{
			Logger::Log(LogLevel::ERR, "Cannot link shader program with shaders ");
			return 0;
		}
	}

	return program;
}