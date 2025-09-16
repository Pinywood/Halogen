#pragma once

#include<GL/glew.h>
#include<GLFW/glfw3.h>
#include<print>
#include<vector>

struct VertexAttribute
{
	unsigned int type;
	unsigned int count;
	unsigned char normalized;

	unsigned int GetSize() const;
};

class VertexBufferLayout
{
private:
	std::vector<VertexAttribute> m_LayoutArray;
	unsigned int m_Stride;

public:
	VertexBufferLayout();
	~VertexBufferLayout();

	template<typename T>
	void Push(unsigned int count)
	{
		std::println("Unmatched Type");
	}

	template<>
	void Push<float>(unsigned int count)
	{
		m_LayoutArray.push_back({ GL_FLOAT, count, GL_FALSE });
		m_Stride += sizeof(float) * count;
	}

	template<>
	void Push<int>(unsigned int count)
	{
		m_LayoutArray.push_back({ GL_INT, count, GL_FALSE });
		m_Stride += sizeof(int) * count;
	}

	template<>
	void Push<char>(unsigned int count)
	{
		m_LayoutArray.push_back({ GL_BYTE, count, GL_TRUE });
		m_Stride += sizeof(char) * count;
	}

	const std::vector<VertexAttribute>& GetAttributeArray() const;
	const unsigned int& GetStride() const;
};