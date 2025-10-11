#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Shader.h"
#include "VertexBuffer.h"
#include "VertexArray.h"
#include "IndexBuffer.h"

class Renderer
{
public:
	Renderer(const int& FramebufferWidth, const int& FramebufferHeight);
	~Renderer();
	void FramebufferResize(const int& FramebufferWidth, const int& FramebufferHeight);
	void SetRenderResolution(const int& ResolutionX, const int& ResolutionY);
	void SetDisplayImage(const int& Image);
	void SetZoom(const float& Zoom);
	void SetShift(const Vec2& Shift);
	void Display();
	void Clear(const float& Red = 0.0f, const float& Green = 0.0f, const float& Blue = 0.0f) const;

private:
	mutable Shader m_DisplayShader = Shader("res/Display.glsl");
	VertexBuffer m_WindowVB;
	IndexBuffer m_WindowIB;
	VertexArray m_WindowVA;

	int m_FrameBufferWidth;
	int m_FrameBufferHeight;
};