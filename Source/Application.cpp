#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_glfw.h>
#include <ImGui/imgui_impl_opengl3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

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

#include <iostream>
#include <print>

Camera camera;
float speed = 1.0;

int WindowWidth = 1600;
int WindowHeight = 800;

int sample = 0;
float AspectRatio = (float)WindowWidth / (float)WindowHeight;
float deltaTime;
float prevTime = 0.0;
float currentTime;

float LastX = 450;
float LastY = 450;
bool FirstMouse = true;
bool TurnEnable = false;

const float Pi = 3.141592653589;

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

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_E && action == GLFW_PRESS)
		TurnEnable = !TurnEnable;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
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

	if (TurnEnable)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		camera.Turn(xoffset, yoffset);
		sample = 0;
	}

	else
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

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
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	glfwSwapInterval(1);

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

	Renderer renderer;

	Shader Accumulator("res/Accumulator.glsl");
	Shader Display("res/Display.glsl");
	Display.SetUniform("gamma", 2.2);

	Framebuffer AccumulationFB(WindowWidth, WindowHeight);
	Framebuffer CurrentSampleFB(WindowWidth, WindowHeight);

	const int CurrentSampleTexSlot = 0;
	const int AccumulatedTexSlot = 1;

	Accumulator.SetUniform("CurrentSampleImage", CurrentSampleTexSlot);
	Accumulator.SetUniform("Accumulated", AccumulatedTexSlot);
	Display.SetUniform("Accumulated", AccumulatedTexSlot);
	
	//ImGUI setup
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);

	const char* glsl_version = "#version 330";
	ImGui_ImplOpenGL3_Init(glsl_version);
	//setup end

	int max_bounces = 10;
	float Sensor_Size = 100.0;
	float Focal_Length = 35.0;

	float SunIntensity = 500.0;
	float SunRadius = 0.8;
	float SunAltitude = 30.0;
	float SunAzimuthal = 0.0;
	float SkyVariation = 0.2;
	float exposure = 0.3;

	while (!glfwWindowShouldClose(window))
	{
		ProcessInput(window);
		glfwPollEvents();

		AccumulationFB.ReSize(WindowWidth, WindowHeight);
		CurrentSampleFB.ReSize(WindowWidth, WindowHeight);

		currentTime = glfwGetTime();
		deltaTime = currentTime - prevTime;
		prevTime = currentTime;

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		{
			ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
			ImGui::Begin("Settings");

			bool modified = false;

			modified |= ImGui::SliderFloat("Sun Radius", &SunRadius, 0.0f, 50.0f);
			modified |= ImGui::SliderFloat("Sun Intensity", &SunIntensity, 0.0f, 1000.0f);
			modified |= ImGui::SliderFloat("Sun Altitude", &SunAltitude, -90.0, 90.0);
			modified |= ImGui::SliderFloat("Sun Azimuthal", &SunAzimuthal, 0.0, 360.0);
			modified |= ImGui::SliderFloat("Sky Variation", &SkyVariation, 0.0f, 1.0f);
			modified |= ImGui::SliderInt("Max Bounces", &max_bounces, 0, 30);
			modified |= ImGui::SliderFloat("Sensor Size", &Sensor_Size, 35.0f, 150.0f);
			modified |= ImGui::SliderFloat("Focal Length", &Focal_Length, 35.0f, 200.0f);
			modified |= ImGui::SliderFloat("Sphere 1 Radius", &Sphere1.Radius, 0.0f, 1.0f);
			modified |= ImGui::SliderFloat("Sphere 1 Roughness", &Sphere1.material.Roughness, 0.0f, 1.0f);
			modified |= ImGui::SliderFloat("Sphere 2 Radius", &Sphere3.Radius, 0.0f, 1.0f);
			modified |= ImGui::SliderFloat("Sphere 2 Roughness", &Sphere3.material.Roughness, 0.0f, 1.0f);
			modified |= ImGui::SliderFloat("Sphere 3 Radius", &Sphere4.Radius, 0.0f, 1.0f);
			modified |= ImGui::SliderFloat("Sphere 3 Roughness", &Sphere4.material.Roughness, 0.0f, 1.0f);

			modified |= ImGui::ColorEdit3("Sphere 1 Color", (float*)&Sphere1.material.BaseColor);
			modified |= ImGui::ColorEdit3("Sphere 2 Color", (float*)&Sphere3.material.BaseColor);
			modified |= ImGui::ColorEdit3("Sphere 3 Color", (float*)&Sphere4.material.BaseColor);
			modified |= ImGui::ColorEdit3("Ground Color", (float*)&Sphere2.material.BaseColor);
			modified |= ImGui::SliderFloat("Ground Roughness", &Sphere2.material.Roughness, 0.0f, 1.0f);

			ImGui::SliderFloat("Exposure", &exposure, 0.0, 1.0);

			if (modified)
			{
				sample = 0;
				RayTracer.SwapBufferObject(0, Sphere1);
				RayTracer.SwapBufferObject(1, Sphere2);
				RayTracer.SwapBufferObject(2, Sphere3);
				RayTracer.SwapBufferObject(3, Sphere4);
			}

			std::stringstream ss;
			ss << "Samples: " << sample;

			ImGui::Text(ss.str().c_str());

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
			ImGui::End();
		}

		CurrentSampleFB.Bind(CurrentSampleTexSlot);
		RayTracer.Clear();

		Display.SetUniform("exposure", exposure / 2.0);
		RayTracer.Setting(RT_Setting::Current_Sample, sample);
		RayTracer.Setting(RT_Setting::Sun_Radius, SunRadius / 200.0);
		RayTracer.Setting(RT_Setting::Sun_Intensity, SunIntensity);
		RayTracer.Setting(RT_Setting::Sun_Altitude, glm::radians(SunAltitude));
		RayTracer.Setting(RT_Setting::Sun_Azimuthal, glm::radians(SunAzimuthal));
		RayTracer.Setting(RT_Setting::Sky_Variation, SkyVariation);
		RayTracer.Setting(RT_Setting::Aspect_Ratio, AspectRatio);
		RayTracer.Setting(RT_Setting::Max_Bounces, max_bounces);
		RayTracer.Setting(RT_Setting::Sensor_Size, Sensor_Size);
		RayTracer.Setting(RT_Setting::Focal_Length, Focal_Length);
		RayTracer.Setting(RT_Setting::View, camera.GetViewMatrix());
		RayTracer.Setting(RT_Setting::Camera_Position, Vec3(camera.Position.x, camera.Position.y, camera.Position.z));

		RayTracer.Render();

		AccumulationFB.Bind(AccumulatedTexSlot);
		Accumulator.SetUniform("CurrentSample", (float)sample);

		if (sample == 0)
			renderer.Clear(0.0, 0.0, 0.0);

		renderer.Draw(WindowVA, 6, Accumulator);

		AccumulationFB.UnBind();
		renderer.Draw(WindowVA, 6, Display);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}

		glfwSwapBuffers(window);
		sample++;
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
}