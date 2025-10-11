#pragma once

#include <ImGui/imgui.h>

#include "Ray Tracer.h"
#include "Scene.h"

namespace HalogenUI
{
	void RenderSettings(RayTracer& RayTracer, Scene& scene, ImGuiIO& io, const float& SinceLastSave);
	void SceneSettings(RayTracer& RayTracer, Scene& scene);
	void MaterialSettings(RayTracer& RayTracer, Scene& scene);
}
