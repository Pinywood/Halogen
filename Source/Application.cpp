#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_glfw.h>
#include <ImGui/imgui_impl_opengl3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "stb_image_write.h"

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
#include "HalogenUI.h"
#include "Renderer.h"

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
bool SaveImage = false;

float Zoom = 1.0;

Vec2 Shift;

const float Pi = 3.141592653589;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	WindowWidth = width;
	WindowHeight = height;
	
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

	if(!MoveEnable && !CtrlHeld)
		Shift += Vec2(-deltaX, deltaZ) * (1.0 / Zoom);
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

	if (key == GLFW_KEY_F11 && action == GLFW_PRESS)
		SaveImage = true;
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

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	float Sensitivity = 1.1;
	Sensitivity = pow(Sensitivity, yoffset);

	if(!MoveEnable)
		Zoom *= Sensitivity;
}

void SaveRender(const char* filepath, RayTracer& RayTracer)
{
	unsigned char* Image = RayTracer.GetRenderedImage();
	stbi_flip_vertically_on_write(1);
	stbi_write_jpg(filepath, RayTracer.GetFramebufferWidth(), RayTracer.GetFramebufferHeight(), 3, Image, 100);
	delete[] Image;
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
	glfwSetScrollCallback(window, scroll_callback);

	glfwSwapInterval(1);

	ImGuiIO& io = SetupImGui(window);

	int RenderResolutionX = 1920;
	int RenderResolutionY = 1080;

	RayTracer RayTracer(RenderResolutionX, RenderResolutionY);
	Renderer renderer(WindowWidth, WindowHeight);
	renderer.SetRenderResolution(RenderResolutionX, RenderResolutionY);
	
	Scene scene;
	if (argc > 1 && scene.Load(argv[1]))
		std::println("Loaded {} successfully\n", argv[1]);

	else
		scene.Load("res/Scene.hgns");

	RayTracer.LoadScene(scene);

	const int RenderedImage = 1;						//TexSlot 0 is used for binding through indirect calls (like resize)
	const int AccumulatedImage = 2;

	RayTracer.StartAccumulation(RenderedImage, AccumulatedImage);
	renderer.SetDisplayImage(RenderedImage);

	float SinceLastSceneSave = 0.0;
	float SinceLastRender = 0.0;
	while (!glfwWindowShouldClose(window))
	{
		ProcessInput(window);
		glfwPollEvents();

		if (Resized)
			renderer.FramebufferResize(WindowWidth, WindowHeight);

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
		
		else
		{
			if (Save)
			{
				SinceLastSceneSave = 0.0;
				scene.m_Camera = RayTracer.GetCamera();
				scene.Save();
			}
			
			renderer.SetZoom(Zoom);
			renderer.SetShift(Shift);
		}

		if (SaveImage)
		{
			SaveRender("render.jpg", RayTracer);
			SinceLastRender = 0.0;
		}

		Save = false;
		Resized = false;
		Turn = false;
		Move = false;
		SaveImage = false;
		SinceLastSceneSave += deltaTime;
		SinceLastRender += deltaTime;

		currentTime = glfwGetTime();
		deltaTime = currentTime - prevTime;
		prevTime = currentTime;

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

		HalogenUI::RenderSettings(renderer, RayTracer, scene, io, SinceLastSceneSave, SinceLastRender, RenderResolutionX, RenderResolutionY);
		HalogenUI::SceneSettings(RayTracer, scene);
		HalogenUI::MaterialSettings(RayTracer, scene);

		RayTracer.Accumulate();
		RayTracer.Accumulate();
		RayTracer.Accumulate();
		RayTracer.Accumulate();
		RayTracer.Accumulate();

		RayTracer.PostProcess();

		renderer.Display();

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