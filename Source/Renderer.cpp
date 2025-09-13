#include "Renderer.h"

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
}

void Renderer::Clear(const float& Red, const float& Green, const float& Blue) const
{
	glClearColor(Red, Green, Blue, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::Draw(const VertexArray& VA, const unsigned int& count, const Shader& Shader) const
{
	VA.Bind();
	Shader.Use();
	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
}
