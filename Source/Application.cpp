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
#include "Scene.h"

#include <iostream>
#include <print>
#include <memory>

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
bool MoveEnable = false;
bool Resized = false;
bool Turn = false;
bool Move = false;
bool CtrlHeld = false;
bool Save = false;

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
		MoveEnable = !MoveEnable;

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		if(MoveEnable)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	if (key == GLFW_KEY_S && CtrlHeld && action == GLFW_PRESS)
		Save = true;

	if (key == GLFW_KEY_LEFT_CONTROL)
	{
		CtrlHeld = false;
		if(action != GLFW_RELEASE)
			CtrlHeld = true;
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

	Turn = true;
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

int main(int argc, char** argv)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WindowWidth, WindowHeight, "Bombombini Windolini", NULL, NULL);
	if (window == nullptr)
		std::println("Could not initialize the window");

	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
		std::println("GLEW ERROR");

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(GLMessageCallback, 0);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_FALSE);
	glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_ERROR, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR, GL_DONT_CARE, 0, nullptr, GL_TRUE);

	std::println("OpenGL Version: {}", (char*)glGetString(GL_VERSION));
	std::println("OpenGL Implementation: {}", (char*)glGetString(GL_VENDOR));
	std::println("Rendering Using: {}\n", (char*)glGetString(GL_RENDERER));

	glViewport(0, 0, WindowWidth, WindowHeight);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	glfwSwapInterval(1);

	ImGuiIO& io = SetupImGui(window);

	std::unique_ptr<RayTracer> raytracer = std::make_unique<RayTracer>(WindowWidth, WindowHeight);
	RayTracer& RayTracer = *raytracer;
	
	Scene scene;
	if (argc > 1 && scene.Load(argv[1]))
		std::println("Loaded {} successfully\n", argv[1]);

	else
		scene.Load("res/Scene.hgns");

	RayTracer.LoadScene(scene);

	const int RenderedImage = 1;						//TexSlot 0 is used for binding through indirect calls (like resize)
	const int AccumulatedImage = 2;

	Shader Display("res/Display.glsl");
	Display.SetUniform("Image", RenderedImage);

	RayTracer.StartAccumulation(RenderedImage, AccumulatedImage);

	float LastSave = 0.0;
	while (!glfwWindowShouldClose(window))
	{
		ProcessInput(window);
		glfwPollEvents();

		if (Resized)
			RayTracer.FramebufferReSize(WindowWidth, WindowHeight);

		if (MoveEnable)
		{
			if (Turn)
			{
				RayTracer.TurnCamera(xoffset, yoffset);
				RayTracer.ResetAccumulation();
			}

			if (Move)
			{
				RayTracer.MoveCamera(deltaX, deltaY, deltaZ);
				RayTracer.ResetAccumulation();
			}
		}
		
		else if (Save)
		{
			LastSave = currentTime;
			scene.m_Camera = RayTracer.GetCamera();
			scene.Save();
		}

		Save = false;
		Resized = false;
		Turn = false;
		Move = false;

		currentTime = glfwGetTime();
		deltaTime = currentTime - prevTime;
		prevTime = currentTime;

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

		{
			ImGui::Begin("Render Settings");

			bool modified = false;

			modified |= ImGui::SliderFloat("Sun Radius", &scene.m_SunRadius, 0.0f, 15.0f);
			modified |= ImGui::SliderFloat("Sun Intensity", &scene.m_SunIntensity, 0.0f, 1000.0f);
			modified |= ImGui::SliderFloat("Sun Altitude", &scene.m_SunAltitude, -90.0, 90.0);
			modified |= ImGui::SliderFloat("Sun Azimuthal", &scene.m_SunAzimuthal, 0.0, 360.0);
			modified |= ImGui::SliderFloat("Sky Variation", &scene.m_SkyVariation, 0.0f, 1.0f);
			modified |= ImGui::SliderInt("Max Bounces", &scene.m_MaxBounces, 0, 60);
			modified |= ImGui::SliderFloat("Sensor Size", &scene.m_SensorSize, 35.0f, 150.0f);
			modified |= ImGui::SliderFloat("Focal Length", &scene.m_FocalLength, 35.0f, 200.0f);
			modified |= ImGui::SliderFloat("Focus Distance", &scene.m_FocusDist, 0.0f, 5.0f);
			modified |= ImGui::SliderFloat("F-Stop", &scene.m_FStop, 0.0f, 2.0f);

			ImGui::SliderFloat("Exposure", &scene.m_Exposure, 0.0, 3.0);
			ImGui::SliderFloat("Gamma", &scene.m_Gamma, 0.0, 3.0);
			RayTracer.Setting(PostProcess_Setting::Gamma, scene.m_Gamma);
			RayTracer.Setting(PostProcess_Setting::Exposure, scene.m_Exposure);

			if (modified)
			{
				RayTracer.Setting(RT_Setting::Sun_Radius, scene.m_SunRadius / 200.0);
				RayTracer.Setting(RT_Setting::Sun_Intensity, scene.m_SunIntensity);
				RayTracer.Setting(RT_Setting::Sun_Altitude, glm::radians(scene.m_SunAltitude));
				RayTracer.Setting(RT_Setting::Sun_Azimuthal, glm::radians(scene.m_SunAzimuthal));
				RayTracer.Setting(RT_Setting::Sky_Variation, scene.m_SkyVariation);
				RayTracer.Setting(RT_Setting::Max_Bounces, scene.m_MaxBounces);
				RayTracer.Setting(RT_Setting::Sensor_Size, scene.m_SensorSize / 1000.0);
				RayTracer.Setting(RT_Setting::Focal_Length, scene.m_FocalLength / 1000.0);
				RayTracer.Setting(RT_Setting::Focus_Dist, scene.m_FocusDist);
				RayTracer.Setting(RT_Setting::F_Stop, scene.m_FStop);
			}

			std::stringstream ss;
			ss << "Samples: " << RayTracer.RenderedSamples();
			ImGui::Text(ss.str().c_str());

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

			if (currentTime - LastSave < 0.5)
				ImGui::Text("Saved");

			ImGui::End();
		}

		{
			ImGui::Begin("Scene Settings");

			ImGui::Text("Spheres");
			for (auto& [name, sphere] : scene.m_SphereMap)
			{
				ImGui::PushID(name.c_str());
				bool modified = false;

				if (name != "Ground")
				{
					ImGui::Text(name.c_str());
					modified |= ImGui::SliderFloat("Radius", &sphere.Radius, 0.0f, 1.0f);
				}

				if (modified)
					RayTracer.SwapBufferObject(name, sphere);

				ImGui::Separator();
				ImGui::PopID();
			}

			ImGui::End();
		}

		{
			ImGui::Begin("Materials");

			for (auto& [name, material] : scene.m_MaterialMap)
			{
				ImGui::PushID(name.c_str());
				bool modified = false;

				ImGui::Text(name.c_str());

				modified |= ImGui::ColorEdit3("Albedo", &material.Albedo.x);

				if(material.Type == BSDFType::Glass)
					modified |= ImGui::SliderFloat("IOR", &material.IOR, 0.0f, 30.0f);

				else
				{
					modified |= ImGui::SliderFloat("Roughness", &material.Roughness, 0.0f, 1.0f);
					modified |= ImGui::SliderFloat("Emission", &material.Emission, 0.0f, 30.0f);
				}

				if (modified)
					RayTracer.SwapMaterial(name, material);

				ImGui::Separator();
				ImGui::PopID();
			}

			ImGui::End();
		}

		RayTracer.Accumulate();
		RayTracer.Accumulate();
		RayTracer.Accumulate();
		RayTracer.Accumulate();
		RayTracer.Accumulate();

		RayTracer.PostProcess();

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