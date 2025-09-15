#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexBufferLayout.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Renderer.h"
#include "Ray Tracer.h"
#include "Model.h"
#include "Camera.h"
#include "Framebuffer.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include<iostream>
#include<print>

Camera camera;
float speed = 1.0;

int WindowWidth = 900;
int WindowHeight = 900;

int sample = 0;
float AspectRatio = (float)WindowWidth / (float)WindowHeight;
float deltaTime;
float prevTime = 0.0;
float currentTime;

float LastX = 450;
float LastY = 450;
bool FirstMouse = true;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	WindowWidth = width;
	WindowHeight = height;
	glViewport(0, 0, WindowWidth, WindowHeight);
	AspectRatio = (double)WindowWidth / (double)WindowHeight;

	sample = 0;
}

void ProcessInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	speed = 1.5;

	if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		speed *= 2.0;

	if (camera.Move(window, deltaTime * speed))
		sample = 0;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	sample = 0;
	if (FirstMouse)
	{
		LastX = xpos;
		LastY = ypos;
		FirstMouse = false;
	}

	float xoffset = xpos - LastX;
	float yoffset = LastY - ypos;
	LastX = xpos;
	LastY = ypos;

	camera.Turn(xoffset, yoffset);
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WindowWidth, WindowHeight, "Bombombini Windolini", NULL, NULL);
	if (window == nullptr)
	{
		std::cout << "Could not initialize the window" << std::endl;
	}

	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
		std::cout << "GLEW ERROR" << std::endl;

	glViewport(0, 0, WindowWidth, WindowHeight);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);

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

	VertexBuffer WindowVB(Vertices, sizeof(Vertices));
	IndexBuffer WindowIB(Indices, 6);
	VertexArray WindowVA;

	WindowBufferLayout.Push<float>(2);
	WindowBufferLayout.Push<float>(2);
	WindowVA.AddBuffer(WindowVB, WindowBufferLayout);
	WindowIB.Bind();

	RayTracer RayTracer;

	Sphere Sphere1;
	Sphere Sphere2;
	Sphere Sphere3;
	Sphere Sphere4;

	Sphere1.material.BaseColor = Vec3(0.7, 0.8, 1.0);
	Sphere1.material.Roughness = 1.0;
	Sphere1.material.Emission = 0.0;
	Sphere1.Position = Vec3(0.0, 0.0, 0.0);
	Sphere1.Radius = 1.0;

	Sphere2.material.BaseColor = Vec3(0.6);
	Sphere2.material.Roughness = 0.8;
	Sphere2.material.Emission = 0.0;
	Sphere2.Position = Vec3(0.0, -1001.0, 0.0);
	Sphere2.Radius = 1000.0;

	Sphere3.material.BaseColor = Vec3(1.0, 0.8, 0.7);
	Sphere3.material.Roughness = 0.0;
	Sphere3.material.Emission = 0.0;
	Sphere3.Position = Vec3(3.0, 0.0, 0.0);
	Sphere3.Radius = 1.0;

	Sphere4.material.BaseColor = Vec3(1.0);
	Sphere4.material.Roughness = 0.0;
	Sphere4.material.Emission = 30.0;
	Sphere4.Position = Vec3(1.5, 0.0, -2.0);
	Sphere4.Radius = 1.0;

	RayTracer.AddToBuffer(Sphere1);
	RayTracer.AddToBuffer(Sphere2);
	RayTracer.AddToBuffer(Sphere3);
	RayTracer.AddToBuffer(Sphere4);

	RayTracer.shader.SetUniform("max_bounces", 20);

	Renderer renderer;

	Shader Accumulator("res/Accumulator.glsl");
	Shader Display("res/Display.glsl");
	Display.SetUniform("exposure", 0.15);
	Display.SetUniform("gamma", 2.2);

	Framebuffer AccumulationFB(WindowWidth, WindowHeight);
	Framebuffer CurrentSampleFB(WindowWidth, WindowHeight);

	const int CurrentSampleTexSlot = 0;
	const int AccumulatedTexSlot = 1;

	Accumulator.SetUniform("CurrentSampleImage", CurrentSampleTexSlot);
	Accumulator.SetUniform("Accumulated", AccumulatedTexSlot);
	Display.SetUniform("Accumulated", AccumulatedTexSlot);

	while (!glfwWindowShouldClose(window))
	{
		ProcessInput(window);
		glfwPollEvents();

		AccumulationFB.ReSize(WindowWidth, WindowHeight);
		CurrentSampleFB.ReSize(WindowWidth, WindowHeight);

		currentTime = glfwGetTime();
		deltaTime = currentTime - prevTime;
		prevTime = currentTime;

		CurrentSampleFB.Bind(CurrentSampleTexSlot);
		RayTracer.Clear();

		RayTracer.shader.SetUniform("CurrentSample", sample);
		RayTracer.shader.SetUniform("AspectRatio", AspectRatio);
		RayTracer.shader.SetUniform("View", camera.GetViewMatrix());
		RayTracer.shader.SetUniform("CameraPos", Vec3(camera.Position.x, camera.Position.y, camera.Position.z));

		RayTracer.Render();

		AccumulationFB.Bind(AccumulatedTexSlot);
		Accumulator.SetUniform("CurrentSample", (float)sample);

		if (sample == 0)
			renderer.Clear(0.0, 0.0, 0.0);

		renderer.Draw(WindowVA, 6, Accumulator);

		AccumulationFB.UnBind();
		renderer.Draw(WindowVA, 6, Display);

		glfwSwapBuffers(window);
		sample++;
	}

	glfwTerminate();
}