#include "HalogenUI.h"

namespace HalogenUI
{
	void RenderSettings(Renderer& renderer, RayTracer& RayTracer, Scene& scene, ImGuiIO& io, const float& SinceLastSave, const float& SinceLastRender)
	{
		ImGui::Begin("Render Settings");

		const float DisplayTime = 0.5;

		bool modified = false;

		int ResX = RayTracer.GetFramebufferWidth();
		int ResY = RayTracer.GetFramebufferHeight();
		ImGui::Text("Image");
		modified |= ImGui::DragInt("Resolution X", &ResX, 1.0, 1, INT32_MAX);
		modified |= ImGui::DragInt("Resolution Y", &ResY, 1.0, 1, INT32_MAX);
		ImGui::Separator();

		if (modified)
		{
			RayTracer.FramebufferReSize(ResX, ResY);
			renderer.SetRenderResolution(ResX, ResY);
			RayTracer.ResetAccumulation();
		}

		ImGui::Text("Light Paths");
		modified |= ImGui::SliderInt("Max Bounces", &scene.m_MaxBounces, 0, 60);
		ImGui::Separator();

		ImGui::Text("World");
		modified |= ImGui::SliderFloat("Sun Radius", &scene.m_SunRadius, 0.0f, 15.0f);
		modified |= ImGui::SliderFloat("Sun Intensity", &scene.m_SunIntensity, 0.0f, 1000.0f);
		modified |= ImGui::SliderFloat("Sun Altitude", &scene.m_SunAltitude, -90.0, 90.0);
		modified |= ImGui::SliderFloat("Sun Azimuthal", &scene.m_SunAzimuthal, 0.0, 360.0);
		modified |= ImGui::SliderFloat("Sky Variation", &scene.m_SkyVariation, 0.0f, 1.0f);
		ImGui::Separator();

		ImGui::Text("Camera");
		modified |= ImGui::SliderFloat("Sensor Size", &scene.m_SensorSize, 35.0f, 150.0f);
		modified |= ImGui::SliderFloat("Focal Length", &scene.m_FocalLength, 35.0f, 200.0f);
		modified |= ImGui::SliderFloat("Focus Distance", &scene.m_FocusDist, 0.0f, 5.0f);
		modified |= ImGui::SliderFloat("F-Stop", &scene.m_FStop, 0.0f, 8.0f);
		ImGui::Separator();

		ImGui::Text("Post Processing");
		ImGui::SliderFloat("Exposure", &scene.m_Exposure, 0.0, 3.0);
		ImGui::SliderFloat("Gamma", &scene.m_Gamma, 0.0, 3.0);
		ImGui::Separator();

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

		if (SinceLastSave < DisplayTime)
			ImGui::Text("Saved");

		if (SinceLastRender < DisplayTime)
			ImGui::Text("Exported");

		ImGui::End();
	}

	void SceneSettings(RayTracer& RayTracer, Scene& scene)
	{
		ImGui::Begin("Scene");

		size_t MaterialCount = scene.m_MaterialMap.size();
		std::string* Materials = new std::string[MaterialCount];
		size_t i = 0;

		for (auto& [name, material] : scene.m_MaterialMap)
		{
			Materials[i] = name;
			i++;
		}

		ImGui::Text("Spheres");
		for (auto& [name, sphere] : scene.m_SphereMap)
		{
			ImGui::PushID(name.c_str());

			ImGui::Text(name.c_str());
			const char* current = sphere.MaterialName.c_str();

			if (ImGui::BeginCombo("Material", current))
			{
				for (size_t n = 0; n < MaterialCount; n++)
				{
					bool is_selected = (current == Materials[n]);
					if (ImGui::Selectable(Materials[n].c_str(), is_selected))
					{
						sphere.MaterialName = Materials[n];
						current = sphere.MaterialName.c_str();
						RayTracer.SwapBufferObject(name, sphere);
					}

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			bool modified = false;

			if (name != "Ground")
			{
				modified |= ImGui::DragFloat("Radius", &sphere.Radius, 0.05f, 0.0f, 100.0f);
				modified |= ImGui::DragFloat3("Position", &sphere.Position.x, 0.1f);
			}

			if (modified)
				RayTracer.SwapBufferObject(name, sphere);

			ImGui::Separator();
			ImGui::PopID();
		}

		ImGui::End();
	}

	void MaterialSettings(RayTracer& RayTracer, Scene& scene)
	{
		ImGui::Begin("Materials");

		for (auto& [name, material] : scene.m_MaterialMap)
		{
			ImGui::PushID(name.c_str());
			bool modified = false;

			ImGui::Text(name.c_str());

			modified |= ImGui::ColorEdit3("Albedo", &material.Albedo.x);

			if (material.Type == BSDFType::Glass)
				modified |= ImGui::SliderFloat("IOR", &material.IOR, 0.0f, 30.0f);

			else
			{
				modified |= ImGui::SliderFloat("Roughness", &material.Roughness, 0.0f, 1.0f);
				modified |= ImGui::DragFloat("Emission", &material.Emission, 1.0f, 0.0, 1000.0f);
			}

			if (modified)
				RayTracer.SwapMaterial(name, material);

			ImGui::Separator();
			ImGui::PopID();
		}

		ImGui::End();
	}
}