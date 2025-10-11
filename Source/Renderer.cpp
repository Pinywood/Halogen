#include "Renderer.h"

Renderer::Renderer(const int& FramebufferWidth, const int& FramebufferHeight)
	:m_FrameBufferWidth(FramebufferWidth), m_FrameBufferHeight(FramebufferHeight)
{
	float DisplayAspectRatio = (float)m_FrameBufferWidth / (float)m_FrameBufferHeight;
	m_DisplayShader.SetUniform("DisplayAspectRatio", DisplayAspectRatio);
	m_DisplayShader.SetUniform("Zoom", 1.0);

	float Vertices[] =
	{				   //Tex Coords
		-1.0f, -1.0f,	0.0, 0.0,
		 1.0f, -1.0f,	1.0, 0.0,
		 1.0f,  1.0f,	1.0, 1.0,
		-1.0f,  1.0f,	0.0, 1.0
	};

	unsigned int Indices[6] =
	{
		0, 1, 2,
		2, 3, 0
	};
	VertexBufferLayout WindowBufferLayout;

	m_WindowVB.Load(Vertices, sizeof(Vertices));
	m_WindowIB.Load(Indices, 6);

	WindowBufferLayout.Push<float>(2);
	WindowBufferLayout.Push<float>(2);
	m_WindowVA.AddBuffer(m_WindowVB, WindowBufferLayout);
	m_WindowIB.Bind();
}

Renderer::~Renderer()
{
}

void Renderer::FramebufferResize(const int& FramebufferWidth, const int& FramebufferHeight)
{
	m_FrameBufferWidth = FramebufferWidth;
	m_FrameBufferHeight = FramebufferHeight;

	float DisplayAspectRatio = (float)m_FrameBufferWidth / (float)m_FrameBufferHeight;
	m_DisplayShader.SetUniform("DisplayAspectRatio", DisplayAspectRatio);
}

void Renderer::SetRenderResolution(const int& ResolutionX, const int& ResolutionY)
{
	float AspectRatio = (float)ResolutionX / (float)ResolutionY;
	m_DisplayShader.SetUniform("RenderAspectRatio", AspectRatio);
}

void Renderer::SetDisplayImage(const int& Image)
{
	m_DisplayShader.SetUniform("Image", Image);
}

void Renderer::SetZoom(const float& Zoom)
{
	m_DisplayShader.SetUniform("Zoom", Zoom);
}

void Renderer::SetShift(const Vec2& Shift)
{
	m_DisplayShader.SetUniform("Shift", Shift);
}

void Renderer::Display()
{
	m_WindowVA.Bind();
	m_DisplayShader.Use();
	glViewport(0, 0, m_FrameBufferWidth, m_FrameBufferHeight);
	Clear(0.2, 0.2, 0.2);
	glDrawElements(GL_TRIANGLES, m_WindowIB.GetCount(), GL_UNSIGNED_INT, nullptr);
}

void Renderer::Clear(const float& Red, const float& Green, const float& Blue) const
{
	glClearColor(Red, Green, Blue, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}