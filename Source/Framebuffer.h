#pragma once
#include <GL/glew.h>
#include <utility>
#include <print>

class FrameBufferTexture
{
public:
	FrameBufferTexture();
	~FrameBufferTexture();

	void GenerateTexture(const int& Width, const int& Height);
	void Bind(int slot = 0) const;
	void UnBind() const;

public:
	unsigned int m_RendererID;
};

class Framebuffer
{
public:
	Framebuffer();
	Framebuffer(const int& Width, const int& Height);
	~Framebuffer();

	void ReSize(const int& Width, const int& Height);
	void Bind(const int& slot = 0) const;
	void UnBind() const;

private:
	unsigned int m_RendererID;
	int m_Width;
	int m_Height;
	FrameBufferTexture m_Texture;
};