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

const std::vector<VertexAttribute>& VertexBufferLayout::GetAttributeArray() const
{
	return m_LayoutArray;
}

const unsigned int& VertexBufferLayout::GetStride() const
{
	return m_Stride;
}

