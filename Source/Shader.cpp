#include "Shader.h"

Shader::Shader(const std::string& filepath)
{
	std::stringstream ss = PreProcess(filepath);
	const auto& [VertexSource, FragmentSource] = ParseShader(ss);

	unsigned int VertexShaderID = CompileShader(GL_VERTEX_SHADER, VertexSource);
	unsigned int FragmentShaderID = CompileShader(GL_FRAGMENT_SHADER, FragmentSource);

	m_RendererID = glCreateProgram();
	glAttachShader(m_RendererID, VertexShaderID);
	glAttachShader(m_RendererID, FragmentShaderID);
	glLinkProgram(m_RendererID);
	glValidateProgram(m_RendererID);

	SetUniformLocations();

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);
}

Shader::~Shader()
{
	glDeleteProgram(m_RendererID);
}

void Shader::Use() const
{
	glUseProgram(m_RendererID);
}

void Shader::SetBool(const std::string& name, const bool& value)
{
	Use();
	glUniform1i(glGetUniformLocation(m_RendererID, name.c_str()), value);
}

void Shader::SetInt(const std::string& name, int value)
{
	Use();
	glUniform1i(glGetUniformLocation(m_RendererID, name.c_str()), value);
}

void Shader::SetFloat(const std::string& name, float value)
{
	Use();
	glUniform1f(glGetUniformLocation(m_RendererID, name.c_str()), value);
}

void Shader::SetFloat(const std::string& name, float value1, float value2)
{
	Use();
	glUniform2f(glGetUniformLocation(m_RendererID, name.c_str()), value1, value2);
}

void Shader::SetFloat(const std::string& name, float value1, float value2, float value3)
{
	Use();
	glUniform3f(glGetUniformLocation(m_RendererID, name.c_str()), value1, value2, value3);
}

void Shader::SetFloat(const std::string& name, float value1, float value2, float value3, float value4)
{
	Use();
	glUniform4f(glGetUniformLocation(m_RendererID, name.c_str()), value1, value2, value3, value4);
}

void Shader::SetFloat(const std::string& name, const Vec3& Vector)
{
	Use();
	glUniform3f(glGetUniformLocation(m_RendererID, name.c_str()), Vector.x, Vector.y, Vector.z);
}

void Shader::SetMat3(const std::string& name, const glm::mat3& matrix)
{
	Use();
	glUniformMatrix3fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::SetMat4(const std::string& name, const glm::mat4& matrix)
{
	Use();
	glUniformMatrix4fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}

template<typename T>
void Shader::SetUniform(const std::string& name, const T& value)
{
	std::println("Attempted to set uniform to invalid type");
}

std::unordered_map<std::string, Uniform>::const_iterator Shader::CheckUniformError(const std::string& name, const glslType& Type)
{
	const auto& found = m_UniformMap.find(name);

	if (found == m_UniformMap.end())
	{
		std::println("Attempted to set uniform '{}', no such uniform exists", name);
		return found;
	}

	if (found->second.Type != Type)
	{
		std::println("Attempted to set uniform '{}' to type {} should be {}", name, Type, found->second.Type);
		return found;
	}

	return found;
}

template<>
void Shader::SetUniform<int>(const std::string& name, const int& value)
{
	const auto& found = CheckUniformError(name, glslType::glslInt);
	Use();
	glUniform1i(found->second.Location, value);
}

template<>
void Shader::SetUniform<float>(const std::string& name, const float& value)
{
	const auto& found = CheckUniformError(name, glslType::glslFloat);
	Use();
	glUniform1f(found->second.Location, value);
}

template<>
void Shader::SetUniform<double>(const std::string& name, const double& value)
{
	const auto& found = CheckUniformError(name, glslType::glslFloat);
	Use();
	glUniform1f(found->second.Location, value);
}

template<>
void Shader::SetUniform<Vec2>(const std::string& name, const Vec2& value)
{
	const auto& found = CheckUniformError(name, glslType::glslVec2);
	Use();
	glUniform2f(found->second.Location, value.x, value.y);
}

template<>
void Shader::SetUniform<Vec3>(const std::string& name, const Vec3& value)
{
	const auto& found = CheckUniformError(name, glslType::glslVec3);
	Use();
	glUniform3f(found->second.Location, value.x, value.y, value.z);
}

template<>
void Shader::SetUniform<glm::mat3>(const std::string& name, const glm::mat3& value)
{
	const auto& found = CheckUniformError(name, glslType::glslMat3);
	Use();
	glUniformMatrix3fv(found->second.Location, 1, GL_FALSE, glm::value_ptr(value));
}

template<>
void Shader::SetUniform<glm::mat4>(const std::string& name, const glm::mat4& value)
{
	const auto& found = CheckUniformError(name, glslType::glslMat4);
	Use();
	glUniformMatrix4fv(found->second.Location, 1, GL_FALSE, glm::value_ptr(value));
}

std::tuple<std::string, std::string> Shader::ParseShader(std::stringstream& stream)
{
	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
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

		else
		{
			ss[(int)type] << line << '\n';
		}

		if (line.find("struct") != std::string::npos && line.find("//") > line.find("struct"))
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

		if (line.find("uniform") != std::string::npos && line.find("//") > line.find("uniform"))
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

std::stringstream Shader::ProcessIncludes(const std::string& filepath) const
{
	std::ifstream stream(filepath);
	std::string line;
	std::stringstream ss;

	while (getline(stream, line))
	{
		if (line.find("#include") != std::string::npos)
		{
			std::string FilePathBuffer;
			char c = ' ';
			for (int i = line.find('"') + 1; i < line.length(); i++)
			{
				c = line.at(i);
				if (c == '"')
					break;

				FilePathBuffer.push_back(c);
			}
			
			ss << ProcessIncludes(FilePathBuffer).str() << '\n';
		}

		else
		{
			ss << line << '\n';
		}
	}

	return ss;
}

std::stringstream Shader::PreProcess(const std::string& filepath) const
{
	std::stringstream ss;
	ss = ProcessIncludes(filepath);

	return ss;
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
		std::cout << "Failed To Compile " << (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment") << " Shader" << std::endl;
		std::cout << message << std::endl;
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
