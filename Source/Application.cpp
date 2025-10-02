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
#include "Ray Tracer.h"
#include "Model.h"
#include "Framebuffer.h"
#include "OpenGLError.h"

#include <iostream>
#include <print>

float speed = 1.0;

int WindowWidth = 1600;
int WindowHeight = 800;

float deltaTime;
float prevTime = 0.0;
float currentTime;

float LastX = 450;
float LastY = 450;
float xoffset;
float yoffset;

float deltaX = 0.0;
float deltaY = 0.0;
float deltaZ = 0.0;

bool FirstMouse = true;
bool TurnEnable = false;
bool Resized = false;
bool Turn = false;
bool Move = false;

const float Pi = 3.141592653589;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	WindowWidth = width;
	WindowHeight = height;
	
	glViewport(0, 0, WindowWidth, WindowHeight);

	Resized = true;
}

void ProcessInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	speed = 1.5;

	if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		speed *= 2.5;

	const float distance = deltaTime * speed;

	deltaX = 0.0;
	deltaY = 0.0;
	deltaZ = 0.0;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		Move = true;
		deltaZ = -distance;
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		Move = true;
		deltaX = -distance;
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		Move = true;
		deltaZ = distance;
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		Move = true;
		deltaX = distance;
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		Move = true;
		deltaY = distance;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		Move = true;
		deltaY = -distance;
	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_E && action == GLFW_PRESS)
	{
		TurnEnable = !TurnEnable;

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		if(TurnEnable)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (FirstMouse)
	{
		LastX = xpos;
		LastY = ypos;
		FirstMouse = false;
	}

	xoffset = xpos - LastX;
	yoffset = LastY - ypos;
	LastX = xpos;
	LastY = ypos;

	if (TurnEnable)
	{
		Turn = true;
	}
}

ImGuiIO& SetupImGui(GLFWwindow* window)
{
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

	return io;
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

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(GLMessageCallback, 0);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_FALSE);
	glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_ERROR, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR, GL_DONT_CARE, 0, nullptr, GL_TRUE);

	std::println("OpenGL Version: {}", (char*)glGetString(GL_VERSION));
	std::println("OpenGL Implementation: {}", (char*)glGetString(GL_VENDOR));
	std::println("Rendering Using: {}", (char*)glGetString(GL_RENDERER));

	glViewport(0, 0, WindowWidth, WindowHeight);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	glfwSwapInterval(1);

	RayTracer RayTracer(WindowWidth, WindowHeight);

	Sphere GlassSphere;
	Sphere Ground;
	Sphere MetalSphere;
	Sphere EmissiveSphere;

	GlassSphere.material.Type = BSDFType::Glass;
	GlassSphere.material.Albedo = Vec3(1.0);
	GlassSphere.material.IOR = 1.5;
	GlassSphere.Position = Vec3(0.0, 0.01, 0.0);
	GlassSphere.Radius = 1.0;

	Ground.material.Albedo = Vec3(0.6);
	Ground.material.Roughness = 0.9;
	Ground.Position = Vec3(0.0, -1001.0, 0.0);
	Ground.Radius = 1000.0;

	MetalSphere.material.Albedo = Vec3(1.0);
	MetalSphere.material.Roughness = 0.0;
	MetalSphere.Position = Vec3(3.0, 0.0, 0.0);
	MetalSphere.Radius = 1.0;

	EmissiveSphere.material.Albedo = Vec3(1.0);
	EmissiveSphere.material.Emission = 30.0;
	EmissiveSphere.Position = Vec3(1.5, 0.0, -2.0);
	EmissiveSphere.Radius = 1.0;

	RayTracer.AddToBuffer("Glass", GlassSphere);
	RayTracer.AddToBuffer("Ground", Ground);
	RayTracer.AddToBuffer("Metal", MetalSphere);
	RayTracer.AddToBuffer("Emission", EmissiveSphere);
	
	ImGuiIO& io = SetupImGui(window);

	int max_bounces = 30;
	float Sensor_Size = 100.0;
	float Focal_Length = 35.0;
	float Focus_Dist = 1.0;
	float F_Stop = 1.4;

	float SunIntensity = 800.0;
	float SunRadius = 0.8;
	float SunAltitude = 30.0;
	float SunAzimuthal = 0.0;
	float SkyVariation = 0.2;

	float gamma = 2.2;
	float exposure = 0.3;

	Shader Display("res/Display.glsl");

	const int RenderedImage = 1;						//TexSlot 0 is used for binding through indirect calls (like resize)
	const int AccumulatedImage = 2;

	Display.SetUniform("Image", AccumulatedImage);

	RayTracer.StartAccumulation(RenderedImage, AccumulatedImage);

	while (!glfwWindowShouldClose(window))
	{
		ProcessInput(window);
		glfwPollEvents();

		if (Resized)
			RayTracer.FramebufferReSize(WindowWidth, WindowHeight);

		if(Turn)
			RayTracer.TurnCamera(xoffset, yoffset);

		if(Move)
			RayTracer.MoveCamera(deltaX, deltaY, deltaZ);

		Resized = false;
		Turn = false;
		Move = false;

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

			modified |= ImGui::SliderFloat("Sun Radius", &SunRadius, 0.0f, 15.0f);
			modified |= ImGui::SliderFloat("Sun Intensity", &SunIntensity, 0.0f, 1000.0f);
			modified |= ImGui::SliderFloat("Sun Altitude", &SunAltitude, -90.0, 90.0);
			modified |= ImGui::SliderFloat("Sun Azimuthal", &SunAzimuthal, 0.0, 360.0);
			modified |= ImGui::SliderFloat("Sky Variation", &SkyVariation, 0.0f, 1.0f);
			modified |= ImGui::SliderInt("Max Bounces", &max_bounces, 0, 60);
			modified |= ImGui::SliderFloat("Sensor Size", &Sensor_Size, 35.0f, 150.0f);
			modified |= ImGui::SliderFloat("Focal Length", &Focal_Length, 35.0f, 200.0f);
			modified |= ImGui::SliderFloat("Focus Distance", &Focus_Dist, 0.0f, 5.0f);
			modified |= ImGui::SliderFloat("F-Stop", &F_Stop, 0.0f, 2.0f);
			modified |= ImGui::SliderFloat("Sphere 1 Radius", &GlassSphere.Radius, 0.0f, 1.0f);
			modified |= ImGui::SliderFloat("Sphere 1 Roughness", &GlassSphere.material.Roughness, 0.0f, 1.0f);
			modified |= ImGui::SliderFloat("Sphere 2 Radius", &MetalSphere.Radius, 0.0f, 1.0f);
			modified |= ImGui::SliderFloat("Sphere 2 Roughness", &MetalSphere.material.Roughness, 0.0f, 1.0f);
			modified |= ImGui::SliderFloat("Sphere 3 Radius", &EmissiveSphere.Radius, 0.0f, 1.0f);
			modified |= ImGui::SliderFloat("Sphere 3 Emission", &EmissiveSphere.material.Emission, 0.0f, 100.0f);

			modified |= ImGui::ColorEdit3("Sphere 1 Color", (float*)&GlassSphere.material.Albedo);
			modified |= ImGui::ColorEdit3("Sphere 2 Color", (float*)&MetalSphere.material.Albedo);
			modified |= ImGui::ColorEdit3("Sphere 3 Color", (float*)&EmissiveSphere.material.Albedo);
			modified |= ImGui::ColorEdit3("Ground Color", (float*)&Ground.material.Albedo);
			modified |= ImGui::SliderFloat("Ground Roughness", &Ground.material.Roughness, 0.0f, 1.0f);

			ImGui::SliderFloat("Exposure", &exposure, 0.0, 1.0);
			ImGui::SliderFloat("Gamma", &gamma, 0.0, 3.0);

			if (modified)
			{
				RayTracer.SwapBufferObject("Glass", GlassSphere);
				RayTracer.SwapBufferObject("Ground", Ground);
				RayTracer.SwapBufferObject("Metal", MetalSphere);
				RayTracer.SwapBufferObject("Emission", EmissiveSphere);

				RayTracer.Setting(RT_Setting::Sun_Radius, SunRadius / 200.0);
				RayTracer.Setting(RT_Setting::Sun_Intensity, SunIntensity);
				RayTracer.Setting(RT_Setting::Sun_Altitude, glm::radians(SunAltitude));
				RayTracer.Setting(RT_Setting::Sun_Azimuthal, glm::radians(SunAzimuthal));
				RayTracer.Setting(RT_Setting::Sky_Variation, SkyVariation);
				RayTracer.Setting(RT_Setting::Max_Bounces, max_bounces);
				RayTracer.Setting(RT_Setting::Sensor_Size, Sensor_Size / 1000.0);
				RayTracer.Setting(RT_Setting::Focal_Length, Focal_Length / 1000.0);
				RayTracer.Setting(RT_Setting::Focus_Dist, Focus_Dist);
				RayTracer.Setting(RT_Setting::F_Stop, F_Stop);
			}

			std::stringstream ss;
			ss << "Samples: " << RayTracer.RenderedSamples();
			ImGui::Text(ss.str().c_str());

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
			ImGui::End();
		}

		RayTracer.Accumulate();
		RayTracer.Accumulate();
		RayTracer.Accumulate();
		RayTracer.Accumulate();
		RayTracer.Accumulate();
	
		Display.SetUniform("gamma", gamma);
		Display.SetUniform("exposure", exposure / 2.0);
		Display.Use();
		RayTracer.Draw();

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
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
}