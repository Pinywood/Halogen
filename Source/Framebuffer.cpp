#include "Framebuffer.h"

FrameBufferTexture::FrameBufferTexture()
{
	glGenTextures(1, &m_RendererID);
}

void FrameBufferTexture::GenerateTexture(const int& Width, const int& Height)
{
	Bind();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	UnBind();
}

void FrameBufferTexture::Bind(int slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);
}

void FrameBufferTexture::UnBind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

FrameBufferTexture::~FrameBufferTexture()
{
	glDeleteTextures(1, &m_RendererID);
}

Framebuffer::Framebuffer()
	:m_Width(0), m_Height(0)
{
	glGenFramebuffers(1, &m_RendererID);
}

Framebuffer::Framebuffer(const int& Width, const int& Height)
	:m_Width(Width), m_Height(Height)
{
	glGenFramebuffers(1, &m_RendererID);
	Texture.GenerateTexture(m_Width, m_Height);

	Bind();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Texture.m_RendererID, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::println("Framebuffer incomplete");

	UnBind();
}

void Framebuffer::ReSize(const int& Width, const int& Height)
{
	m_Width = Width;
	m_Height = Height;

	Texture.GenerateTexture(m_Width, m_Height);

	Bind();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Texture.m_RendererID, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::println("Framebuffer incomplete");

	UnBind();
}

void Framebuffer::Bind(const int& slot) const
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
	Texture.Bind(slot);
}

void Framebuffer::UnBind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer::~Framebuffer()
{
	glDeleteFramebuffers(1, &m_RendererID);
}
