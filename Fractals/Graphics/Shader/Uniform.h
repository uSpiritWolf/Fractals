#pragma once

#include "gl/glew.h"
#include "Math/vec.h"

enum class UniformType
{
	NONE,
	FLOAT,
	VEC2,
	INT,
	DOUBLE
};

class Uniform
{
public:
	Uniform(GLint uniformLocation) : m_uniformLocation(uniformLocation)
		, m_type(UniformType::NONE)
		, m_vec2Value()
	{
	}

	Uniform(const Uniform&) = delete;
	Uniform(const Uniform&&) = delete;

	void Apply() const;

	bool IsValidType();
	bool IsValidLocation();

	Uniform& operator=(float value);
	Uniform& operator=(double value);
	Uniform& operator=(int value);
	Uniform& operator=(const math::vec2f& value);

private:

	UniformType m_type;
	GLint m_uniformLocation;

	union
	{
		math::vec2f m_vec2Value;
		float m_fValue;
		int m_iValue;
		double m_dValue;
	};
};

inline Uniform& Uniform::operator=(const float value)
{
	m_type = UniformType::FLOAT;
	m_fValue = value;
	return *this;
}

inline Uniform& Uniform::operator=(double value)
{
	m_type = UniformType::DOUBLE;
	m_dValue = value;
	return *this;
}

inline Uniform& Uniform::operator=(int value)
{
	m_type = UniformType::INT;
	m_iValue = value;
	return *this;
}

inline Uniform& Uniform::operator=(const math::vec2f& value)
{
	m_type = UniformType::VEC2;
	m_vec2Value = value;
	return *this;
}

inline void Uniform::Apply() const
{
	switch (m_type)
	{
	case UniformType::NONE:
		break;
	case UniformType::FLOAT:
		glUniform1f(m_uniformLocation, m_fValue);
		break;
	case UniformType::DOUBLE:
		glUniform1d(m_uniformLocation, m_dValue);
		break;
	case UniformType::INT:
		glUniform1i(m_uniformLocation, m_iValue);
		break;
	case UniformType::VEC2:
		glUniform2f(m_uniformLocation, m_vec2Value.x, m_vec2Value.y);
		break;

	}
}

inline bool Uniform::IsValidType()
{
	return m_type != UniformType::NONE;
}

inline bool Uniform::IsValidLocation()
{
	return m_uniformLocation != -1;
}
