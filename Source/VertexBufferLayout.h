#pragma once

#include<GL/glew.h>
#include<GLFW/glfw3.h>

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
	void Push(unsigned int count);

	template<>
	void Push<float>(unsigned int count);

	template<>
	void Push<int>(unsigned int count);

	template<>
	void Push<char>(unsigned int count);

	const std::vector<VertexAttribute>& GetAttributeArray() const;
	const unsigned int& GetStride() const;
};