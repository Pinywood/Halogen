#include "Shader.h"

Shader::Shader(const std::string& filepath)
	:m_filepath(filepath)
{
	std::println("Compiling Shader: {}\n", m_filepath);
	m_PreProcessedCode = PreProcess(filepath);
	const auto& [VertexSource, FragmentSource] = ParseShader(m_PreProcessedCode);

	unsigned int VertexShaderID = CompileShader(GL_VERTEX_SHADER, VertexSource);
	unsigned int FragmentShaderID = CompileShader(GL_FRAGMENT_SHADER, FragmentSource);

	m_RendererID = glCreateProgram();
	glAttachShader(m_RendererID, VertexShaderID);
	glAttachShader(m_RendererID, FragmentShaderID);
	glLinkProgram(m_RendererID);

	int success;
	glGetProgramiv(m_RendererID, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		int length;
		glGetProgramiv(m_RendererID, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetProgramInfoLog(m_RendererID, length, &length, message);
		std::println("Failed To Link Shader Program");
		std::print("{}\n", message);
		glDeleteShader(m_RendererID);
		return;
	}

	glValidateProgram(m_RendererID);

	SetUniformLocations();

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);
}

void Shader::ReCompile()
{
	const auto& [VertexSource, FragmentSource] = ParseConstants(m_PreProcessedCode);
	unsigned int VertexShaderID = CompileShader(GL_VERTEX_SHADER, VertexSource);
	unsigned int FragmentShaderID = CompileShader(GL_FRAGMENT_SHADER, FragmentSource);

	glDeleteProgram(m_RendererID);
	m_RendererID = glCreateProgram();

	glAttachShader(m_RendererID, VertexShaderID);
	glAttachShader(m_RendererID, FragmentShaderID);
	glLinkProgram(m_RendererID);

	int success;
	glGetProgramiv(m_RendererID, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		int length;
		glGetProgramiv(m_RendererID, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetProgramInfoLog(m_RendererID, length, &length, message);
		std::println("Failed To Link Shader Program during recompilation");
		std::print("{}\n", message);
		glDeleteShader(m_RendererID);
		return;
	}

	glValidateProgram(m_RendererID);

	SetUniformLocations();
	SetCachedUniforms();

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	std::println("Recompiling Shader: {}\n", m_filepath);
}

Shader::~Shader()
{
	glDeleteProgram(m_RendererID);
}

void Shader::Use() const
{
	glUseProgram(m_RendererID);
}

void Shader::SetBool(const std::string& name, const bool& value) const
{
	Use();
	glUniform1i(glGetUniformLocation(m_RendererID, name.c_str()), value);
}

void Shader::SetInt(const std::string& name, int value) const
{
	Use();
	glUniform1i(glGetUniformLocation(m_RendererID, name.c_str()), value);
}

void Shader::SetFloat(const std::string& name, float value) const
{
	Use();
	glUniform1f(glGetUniformLocation(m_RendererID, name.c_str()), value);
}

void Shader::SetFloat(const std::string& name, float value1, float value2) const
{
	Use();
	glUniform2f(glGetUniformLocation(m_RendererID, name.c_str()), value1, value2);
}

void Shader::SetFloat(const std::string& name, float value1, float value2, float value3) const
{
	Use();
	glUniform3f(glGetUniformLocation(m_RendererID, name.c_str()), value1, value2, value3);
}

void Shader::SetFloat(const std::string& name, float value1, float value2, float value3, float value4) const
{
	Use();
	glUniform4f(glGetUniformLocation(m_RendererID, name.c_str()), value1, value2, value3, value4);
}

void Shader::SetFloat(const std::string& name, const Vec3& Vector) const
{
	Use();
	glUniform3f(glGetUniformLocation(m_RendererID, name.c_str()), Vector.x, Vector.y, Vector.z);
}

void Shader::SetMat3(const std::string& name, const glm::mat3& matrix) const
{
	Use();
	glUniformMatrix3fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::SetMat4(const std::string& name, const glm::mat4& matrix) const
{
	Use();
	glUniformMatrix4fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}

bool Shader::CheckUniformValidity(const std::string& name, const glslType& Type)
{
	const auto& found = m_UniformMap.find(name);

	if (found == m_UniformMap.end())
	{
		std::println("ERROR: At line {}, in {}:", __LINE__, __FILE__);
		std::println("Attempted to set uniform '{}', no such uniform exists\n", name);
		return false;
	}

	if (found->second.Type != Type)
	{
		std::println("ERROR: At line {}, in {}:", __LINE__, __FILE__);
		std::println("Attempted to set uniform '{}' to type {} should be {}\n", name, Type, found->second.Type);
		return false;
	}

	Use();
	found->second.Set = true;
	return true;
}

void Shader::SetUniform(const std::string& name, const int& value)
{
	if (!CheckUniformValidity(name, glslType::glslInt))
		return;

	const auto& found = m_UniformMap.find(name);
	found->second.SetValue(value);
}

void Shader::SetUniform(const std::string& name, const float& value)
{
	if (!CheckUniformValidity(name, glslType::glslFloat))
		return;

	const auto& found = m_UniformMap.find(name);
	found->second.SetValue(value);
}

void Shader::SetUniform(const std::string& name, const double& value)
{
	if (!CheckUniformValidity(name, glslType::glslFloat))
		return;

	const auto& found = m_UniformMap.find(name);
	found->second.SetValue(value);
}

void Shader::SetUniform(const std::string& name, const Vec2& value)
{
	if (!CheckUniformValidity(name, glslType::glslVec2))
		return;

	const auto& found = m_UniformMap.find(name);
	found->second.SetValue(value);
}

void Shader::SetUniform(const std::string& name, const Vec3& value)
{
	if (!CheckUniformValidity(name, glslType::glslVec3))
		return;

	const auto& found = m_UniformMap.find(name);
	found->second.SetValue(value);
}

void Shader::SetUniform(const std::string& name, const glm::mat3& value)
{
	if (!CheckUniformValidity(name, glslType::glslMat3))
		return;

	const auto& found = m_UniformMap.find(name);
	found->second.SetValue(value);
}

void Shader::SetUniform(const std::string& name, const glm::mat4& value)
{
	if (!CheckUniformValidity(name, glslType::glslMat4))
		return;

	const auto& found = m_UniformMap.find(name);
	found->second.SetValue(value);
}

std::unordered_map<std::string, Uniform> Shader::GetUniformMap() const
{
	return m_UniformMap;
}

bool Shader::CheckUniformStatus(const std::string& name) const
{
	const auto& found = m_UniformMap.find(name);
	if(found != m_UniformMap.end())
		return m_UniformMap.at(name).Set;

	std::println("Attempting to check uniform {}'s status: Does not exist", name);
	return false;
}

void Shader::AddToLookUp(const std::string name, const int& value)
{
	m_ConstantLookUpMap[name] = std::to_string(value);
}

void Shader::AddToLookUp(const std::string name, const size_t& value)
{
	m_ConstantLookUpMap[name] = std::to_string(value);
}

void Shader::AddToLookUp(const std::string name, const float& value)
{
	m_ConstantLookUpMap[name] = std::to_string(value);
}

void Shader::AddToLookUp(const std::string name, const double& value)
{
	m_ConstantLookUpMap[name] = std::to_string(value);
}

std::tuple<std::string, std::string> Shader::ParseShader(const std::string& ShaderCode)
{
	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream stream;
	stream << ShaderCode;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;

	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
			{
				type = ShaderType::VERTEX;
			}

			else if(line.find("fragment") != std::string::npos)
			{
				type = ShaderType::FRAGMENT;
			}
		}

		else if (TokenPresent(line, "const") && TokenPresent(line, "="))
		{
			for (auto& [name, value] : m_ConstantLookUpMap)
			{
				if (TokenPresent(line, name))
				{
					int pos = line.find("=") + 1;
					int end = line.find(";");
					line.replace(pos, end - pos, value);
				}
			}

			ss[(int)type] << line << '\n';
		}

		else
		{
			ss[(int)type] << line << '\n';
		}

		if (TokenPresent(line, "struct"))
		{
			glslStruct Struct;
			char c;
			std::string TokenBuffer;

			for (int index = std::string("struct").length(); index < line.length(); index++)
			{
				c = line.at(index);

				if(std::isalnum(c))
					TokenBuffer.push_back(c);
			}

			Struct.Name = TokenBuffer;
			m_glslStructMap[Struct.Name] = Struct;
		}

		if (TokenPresent(line, "uniform"))
		{
			Uniform uniform;
			char c;
			std::string TokenBuffer;
			for (int index = std::string("uniform").length(); index < line.length(); index++)
			{
				TokenBuffer.clear();
				c = line.at(index);

				while (std::isalnum(c) || c == '_')		//[FIX]: [] are actually not a part of the ident
				{
					TokenBuffer.push_back(c);
					index++;
					c = line.at(index);
				}

				const auto& found = glslTypeMap.find(TokenBuffer);
				if (found != glslTypeMap.end())
				{
					uniform.Type = found->second;
				}

				else if (m_glslStructMap.find(TokenBuffer) != m_glslStructMap.end())
				{
					uniform.Type = glslType::glslStruct;
				}

				else if (!TokenBuffer.empty())
				{
					m_UniformMap[TokenBuffer] = uniform;
					break;
				}

				if (c == '[')
				{
					uniform.Is_Array = true;
					m_UniformMap[TokenBuffer] = uniform;
					break;
				}
			}
		}
	}

	return { ss[0].str(), ss[1].str() };
}

std::tuple<std::string, std::string> Shader::ParseConstants(const std::string& ShaderCode)
{
	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream stream;
	stream << ShaderCode;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;

	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
			{
				type = ShaderType::VERTEX;
			}

			else if (line.find("fragment") != std::string::npos)
			{
				type = ShaderType::FRAGMENT;
			}
		}

		else if (TokenPresent(line, "const") && TokenPresent(line, "="))
		{
			for (auto& [name, value] : m_ConstantLookUpMap)
			{
				if (TokenPresent(line, name))
				{
					int pos = line.find("=") + 1;
					int end = line.find(";");
					line.replace(pos, end - pos, value);
				}
			}

			ss[(int)type] << line << '\n';
		}

		else
			ss[(int)type] << line << '\n';
	}

	return { ss[0].str(), ss[1].str() };
}

void Shader::SetCachedUniforms()
{
	for (auto& [name, uniform] : m_UniformMap)
	{
		if (!uniform.Set)
			continue;

		switch (uniform.Type)
		{
		case glslType::glslInt:
			SetUniform(name, uniform.IntValue);
			break;

		case glslType::glslFloat:
			SetUniform(name, uniform.FloatValue);
			break;

		case glslType::glslVec2:
			SetUniform(name, uniform.Vec2Value);
			break;

		case glslType::glslVec3:
			SetUniform(name, uniform.Vec3Value);
			break;

		case glslType::glslMat3:
			SetUniform(name, uniform.Mat3Value);
			break;

		case glslType::glslMat4:
			SetUniform(name, uniform.Mat4Value);
			break;
		}
	}
}

std::string Shader::GetFileDirectory(const std::string& filepath) const
{
	std::string directory = filepath;
	char c = directory.back();
	while (c != '/')
	{
		directory.pop_back();
		c = directory.back();
	}

	return directory;
}

std::string Shader::ProcessIncludes(const std::string& filepath) const
{
	std::ifstream stream(filepath);
	std::string line;
	std::stringstream ss;

	if (!stream.is_open())
	{
		std::println("Failed to Open: {}", filepath);
		return ss.str();
	}

	while (getline(stream, line))
	{
		if (line.find("#include") != std::string::npos)
		{
			std::string FilePathBuffer;
			char c = ' ';
			FilePathBuffer = GetFileDirectory(filepath);

			for (int i = line.find('"') + 1; i < line.length(); i++)
			{
				c = line.at(i);
				if (c == '"')
					break;

				FilePathBuffer.push_back(c);
			}
			
			ss << ProcessIncludes(FilePathBuffer) << '\n';
		}

		else
		{
			ss << line << '\n';
		}
	}

	stream.close();
	return ss.str();
}

std::string Shader::PreProcess(const std::string& filepath) const
{
	std::string ShaderCode = ProcessIncludes(filepath);
	return ShaderCode;
}

int Shader::CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int success;
	glGetShaderiv(id, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::println("Failed To Compile {} Shader", type == GL_VERTEX_SHADER ? "Vertex" : "Fragment");
		std::print("{}\n", message);
		glDeleteShader(id);
		return 0;
	}

	return id;
}

void Shader::SetUniformLocations()
{
	for (auto& [name, uniform] : m_UniformMap)
	{
		uniform.Location = glGetUniformLocation(m_RendererID, name.c_str());
	}
}

void Uniform::SetValue(const int& value)
{
	glUniform1i(Location, value);
	IntValue = value;
}

void Uniform::SetValue(const float& value)
{
	glUniform1f(Location, value);
	FloatValue = value;
}

void Uniform::SetValue(const double& value)
{
	glUniform1f(Location, value);
	FloatValue = value;
}

void Uniform::SetValue(const Vec2& value)
{
	glUniform2f(Location, value.x, value.y);
	Vec2Value = value;
}

void Uniform::SetValue(const Vec3& value)
{
	glUniform3f(Location, value.x, value.y, value.z);
	Vec3Value = value;
}

void Uniform::SetValue(const glm::mat3& value)
{
	glUniformMatrix3fv(Location, 1, GL_FALSE, glm::value_ptr(value));
	Mat3Value = value;
}

void Uniform::SetValue(const glm::mat4& value)
{
	glUniformMatrix4fv(Location, 1, GL_FALSE, glm::value_ptr(value));
	Mat4Value = value;
}
