#include "DetailsPanel.h"
#include "EntitiesPanel.h"
#include "EditorLayer.h"

#include <Eklipse/Scene/Components.h>

#include <misc/cpp/imgui_stdlib.h>
#include <glm/gtc/type_ptr.hpp>

namespace Editor
{
	void DetailsPanel::Setup(std::string& name)
	{
		m_entityNameBuffer = name;
	}
	void DetailsPanel::OnGUI(float deltaTime)
	{
		EK_PROFILE();

		ImGui::Begin("Details");

		DetailsSelectionInfo& info = EditorLayer::Get().GetSelection();

		if (info.type == SelectionType::None)
		{
			ImGui::End();
			return;
		}

		// Split logic based on type
		if (info.type == SelectionType::Entity)
		{
			OnEntityGUI(info.entity);
		}
		else if (info.type == SelectionType::Material)
		{
			OnMaterialGUI(info.material);
		}

		ImGui::End();
	}
	void DetailsPanel::OnEntityGUI(Eklipse::Entity entity)
	{
		// Right-click menu
		{
			if (ImGui::BeginPopupContextWindow())
			{
				if (!entity.HasComponent<Eklipse::MeshComponent>() && ImGui::MenuItem("Add Mesh Component"))
					entity.AddComponent<Eklipse::MeshComponent>();

				ImGui::EndPopup();
			}
		}

		// Name
		{
			if (!ImGui::InputText("Entity Name", &m_entityNameBuffer))
			{
				if (m_entityNameBuffer.size() > 0)
				{
					entity.GetComponent<Eklipse::NameComponent>().name = m_entityNameBuffer;
				}
			}
		}

		// Transform
		{
			auto& transComp = entity.GetComponent<Eklipse::TransformComponent>();

			ImGui::DragFloat3("Position", glm::value_ptr(transComp.transform.position), 0.1f);
			ImGui::DragFloat3("Rotation", glm::value_ptr(transComp.transform.rotation), 0.1f);
			ImGui::DragFloat3("Scale", glm::value_ptr(transComp.transform.scale), 0.1f);
		}

		// Camera
		{
			auto* cameraComp = entity.TryGetComponent<Eklipse::CameraComponent>();
			if (cameraComp)
			{
				ImGui::SliderFloat("FOV", &cameraComp->camera.m_fov, 0.0f, 180.0f);
				ImGui::SliderFloat("Near", &cameraComp->camera.m_nearPlane, 0.01f, 1000.0f);
				ImGui::SliderFloat("Far", &cameraComp->camera.m_farPlane, 0.01f, 1000.0f);
			}
		}

		// Mesh
		{
			auto* meshComp = entity.TryGetComponent<Eklipse::MeshComponent>();
			if (meshComp != nullptr)
			{
				ImGui::InputText("Mesh", &meshComp->meshPath);
				ImGui::InputText("Material", &meshComp->materialPath);
				if (ImGui::Button("Apply"))
				{
					bool valid = Eklipse::Path::CheckPathValid(meshComp->meshPath, { ".obj" });	 // TODO: Check other formats
					valid = valid && Eklipse::Path::CheckPathValid(meshComp->materialPath, { ".ekmt" }); // TODO: Check other formats

					if (valid)
					{
						meshComp->mesh = Eklipse::Project::GetActive()->GetAssetLibrary()->GetMesh(meshComp->meshPath).get();
						meshComp->material = Eklipse::Project::GetActive()->GetAssetLibrary()->GetMaterial(meshComp->materialPath).get();
					}
				}
			}
		}
	}
	void DetailsPanel::OnMaterialGUI(Eklipse::Material* material)
	{
		ImGui::Text(("Material: " + material->GetName()).c_str());
		const char* id = material->GetName().c_str();

		ImGui::InputPath(id, "Shader", material->GetShader()->GetPath(), {".eksh"},
		[&]()
		{
			EK_CORE_TRACE("Shader path changed to: {0}", material->GetShader()->GetPath().string());
			material->SetShader(material->GetShader()->GetPath());
		});

		ImGui::Spacing();

		uint16_t i = 0;
		for (auto&& [textureSampler, sampler] : material->GetSamplers())
		{
			ImGui::InputPath(id+(i++), textureSampler.c_str(), sampler.texturePath, {".png", ".jpg"},
			[&]()
			{
				EK_CORE_TRACE("Texture path changed to: {0}", sampler.texturePath.string());
			});
		}
		for (auto&& [name, pushConstant] : material->GetPushConstants())
		{
			ImGui::Spacing();
			ImGui::Text("%s", name.c_str());
			for (auto&& [valueName, value] : pushConstant.dataPointers)
			{
				if (value.type == Eklipse::DataType::BOOL)
					ImGui::Checkbox(valueName.c_str(), (bool*)value.data);
				else if (value.type == Eklipse::DataType::FLOAT)
					ImGui::InputFloat(valueName.c_str(), (float*)value.data);
				else if (value.type == Eklipse::DataType::FLOAT2)
					ImGui::InputFloat2(valueName.c_str(), (float*)value.data);
				else if (value.type == Eklipse::DataType::FLOAT3)
					ImGui::InputFloat3(valueName.c_str(), (float*)value.data);
				else if (value.type == Eklipse::DataType::FLOAT4)
					ImGui::InputFloat4(valueName.c_str(), (float*)value.data);
				else if (value.type == Eklipse::DataType::INT)
					ImGui::InputInt(valueName.c_str(), (int*)value.data);
				else if (value.type == Eklipse::DataType::INT2)
					ImGui::InputInt2(valueName.c_str(), (int*)value.data);
				else if (value.type == Eklipse::DataType::INT3)
					ImGui::InputInt3(valueName.c_str(), (int*)value.data);
				else if (value.type == Eklipse::DataType::INT4)
					ImGui::InputInt4(valueName.c_str(), (int*)value.data);
			}
		}

		if (ImGui::Button("Apply"))
		{
			bool allValid = true;
			for (auto&& [textureSampler, sampler] : material->GetSamplers())
			{
				allValid = Eklipse::Path::CheckPathValid(sampler.texturePath, { ".png", ".jpg"}); // TODO: Check other formats
				if (!allValid) 
				{
					break;
				}
			}

			if (allValid)
				material->ApplyChanges();
		}
	}
}
