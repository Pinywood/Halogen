#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "VectorMath.h"

#include <iostream>
#include <print>
#include <format>
#include <functional>

enum class glslType
{
	glslInt, glslFloat, glslVec2, glslVec3, glslVec4, glslStruct, glslMat3, glslMat4
};

template<>
struct std::formatter<glslType> : std::formatter<std::string>
{
	auto format(const glslType& type, format_context& ctx) const
	{
		if (type == glslType::glslInt)
			return std::formatter<std::string>::format(std::format("{}", "int"), ctx);

		if (type == glslType::glslFloat)
			return std::formatter<std::string>::format(std::format("{}", "float"), ctx);

		if (type == glslType::glslVec2)
			return std::formatter<std::string>::format(std::format("{}", "vec2"), ctx);

		if (type == glslType::glslVec3)
			return std::formatter<std::string>::format(std::format("{}", "vec3"), ctx);

		if (type == glslType::glslVec4)
			return std::formatter<std::string>::format(std::format("{}", "vec4"), ctx);

		if (type == glslType::glslStruct)
			return std::formatter<std::string>::format(std::format("{}", "glslStruct"), ctx);

		if (type == glslType::glslMat3)
			return std::formatter<std::string>::format(std::format("{}", "mat3"), ctx);

		if (type == glslType::glslMat4)
			return std::formatter<std::string>::format(std::format("{}", "mat4"), ctx);

		else
			return std::formatter<std::string>::format(std::format("{}", "<NO_TYPE>"), ctx);
	}
};

const std::unordered_map<std::string, glslType> glslTypeMap =
{
	std::pair("int", glslType::glslInt),
	std::pair("float", glslType::glslFloat),
	std::pair("vec2", glslType::glslVec2),
	std::pair("vec3", glslType::glslVec3),
	std::pair("vec4", glslType::glslVec4),
	std::pair("mat3", glslType::glslMat3),
	std::pair("mat4", glslType::glslMat4),
	std::pair("sampler2D", glslType::glslInt)
};

struct Uniform
{
	int Location;
	glslType Type;
	bool Is_Array = false;
	bool Set = false;
};

struct glslStruct
{
	std::string Name;
};

class Shader
{
public:
	Shader() = default;
	Shader(const std::string& filepath);
	~Shader();
	void Use() const;

	void SetBool(const std::string& name, const bool& value) const;
	void SetInt(const std::string& name, int value) const;
	void SetFloat(const std::string& name, float value) const;
	void SetFloat(const std::string& name, float value1, float value2) const;
	void SetFloat(const std::string& name, float value1, float value2, float value3) const;
	void SetFloat(const std::string& name, float value1, float value2, float value3, float value4) const;
	void SetFloat(const std::string& name, const Vec3& Vector) const;
	void SetMat3(const std::string& name, const glm::mat3& matrix) const;
	void SetMat4(const std::string& name, const glm::mat4& matrix) const;

	void SetUniform(const std::string& name, const int& value);
	void SetUniform(const std::string& name, const float& value);
	void SetUniform(const std::string& name, const double& value);
	void SetUniform(const std::string& name, const Vec2& value);
	void SetUniform(const std::string& name, const Vec3& value);
	void SetUniform(const std::string& name, const glm::mat3& value);
	void SetUniform(const std::string& name, const glm::mat4& value);

	std::unordered_map<std::string, Uniform> GetUniformMap() const;
	bool CheckUniformStatus(const std::string& name) const;

private:
	std::tuple<std::string, std::string> ParseShader(std::stringstream& stream);
	std::stringstream ProcessIncludes(const std::string& filepath) const;
	std::stringstream PreProcess(const std::string& filepath) const;
	int CompileShader(unsigned int type, const std::string& source);
	void SetUniformLocations();

	template<typename ...argTypes>
	int SetUniformTemplate(const std::string& name, const glslType& Type, std::function<void(int, argTypes...)> glFunc, argTypes ...args);

private:
	unsigned int m_RendererID;
	std::unordered_map<std::string, glslStruct> m_glslStructMap;
	std::unordered_map<std::string, Uniform> m_UniformMap;
};
