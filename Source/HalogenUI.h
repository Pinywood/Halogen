#pragma once

#include <ImGui/imgui.h>

#include "Ray Tracer.h"
#include "Renderer.h"
#include "Scene.h"

namespace HalogenUI
{
	void RenderSettings(Renderer& renderer, RayTracer& RayTracer, Scene& scene, ImGuiIO& io, const float& SinceLastSave, const float& SinceLastRender, int& ResX, int& ResY);
	void SceneSettings(RayTracer& RayTracer, Scene& scene);
	void MaterialSettings(RayTracer& RayTracer, Scene& scene);
}
