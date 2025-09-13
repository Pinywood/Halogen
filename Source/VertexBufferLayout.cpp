#include"VertexBufferLayout.h"

#include <iostream>

unsigned int VertexAttribute::GetSize() const
{
	switch (type)
	{
	case GL_FLOAT:	return 4;
	case GL_INT:	return 4;
	case GL_BYTE:	return 1;
	}

	return 0;
}

VertexBufferLayout::VertexBufferLayout()
	:m_Stride(0)
{
}

VertexBufferLayout::~VertexBufferLayout()
{
}

template<typename T>
void VertexBufferLayout::Push(unsigned int count)
{
	std::cout << "Type unmatched" << std::endl;
}

template<>
void VertexBufferLayout::Push<float>(unsigned int count)
{
	m_LayoutArray.push_back({ GL_FLOAT, count, GL_FALSE });
	m_Stride += sizeof(float) * count;
}

template<>
void VertexBufferLayout::Push<int>(unsigned int count)
{
	m_LayoutArray.push_back({ GL_INT, count, GL_FALSE });
	m_Stride += sizeof(int) * count;
}

template<>
void VertexBufferLayout::Push<char>(unsigned int count)
{
	m_LayoutArray.push_back({ GL_BYTE, count, GL_TRUE });
	m_Stride += sizeof(char) * count;
}

const std::vector<VertexAttribute>& VertexBufferLayout::GetAttributeArray() const
{
	return m_LayoutArray;
}

const unsigned int& VertexBufferLayout::GetStride() const
{
	return m_Stride;
}

