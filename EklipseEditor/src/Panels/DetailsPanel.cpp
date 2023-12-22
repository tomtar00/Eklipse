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
	bool DetailsPanel::OnGUI(float deltaTime)
	{
		EK_PROFILE();

		if (!GuiPanel::OnGUI(deltaTime)) return false;

		ImGui::Begin("Details");

		DetailsSelectionInfo& info = EditorLayer::Get().GetSelection();

		if (info.type == SelectionType::NONE)
		{
			ImGui::End();
			return true;
		}

		// Split logic based on type
		if (info.type == SelectionType::ENTITY)
		{
			OnEntityGUI(info.entity);
		}
		else if (info.type == SelectionType::MATERIAL)
		{
			OnMaterialGUI(info.material);
		}

		ImGui::End();
		return true;
	}
	void DetailsPanel::OnEntityGUI(Eklipse::Entity entity)
	{
		// Right-click menu
		{
			if (ImGui::BeginPopupContextWindow())
			{
				if (!entity.HasComponent<Eklipse::MeshComponent>() && ImGui::MenuItem("Add Mesh Component"))
					entity.AddComponent<Eklipse::MeshComponent>();

				if (!entity.HasComponent<Eklipse::ScriptComponent>() && ImGui::MenuItem("Add Script Component"))
					entity.AddComponent<Eklipse::ScriptComponent>();

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
			ImGui::Spacing();
		}

		// Camera
		{
			auto* cameraComp = entity.TryGetComponent<Eklipse::CameraComponent>();
			if (cameraComp && ImGui::CollapsingHeader("Camera"))
			{
				ImGui::SliderFloat("FOV", &cameraComp->camera.m_fov, 0.0f, 180.0f);
				ImGui::SliderFloat("Near", &cameraComp->camera.m_nearPlane, 0.01f, 1000.0f);
				ImGui::SliderFloat("Far", &cameraComp->camera.m_farPlane, 0.01f, 1000.0f);
			}
		}

		// Mesh
		{
			auto* meshComp = entity.TryGetComponent<Eklipse::MeshComponent>();
			if (meshComp != nullptr && ImGui::CollapsingHeader("Mesh"))
			{
				ImGui::Indent();
				ImGui::InputText("Mesh##Input", &meshComp->meshPath);
				ImGui::InputText("Material##Input", &meshComp->materialPath);
				if (ImGui::Button("Apply"))
				{
					bool valid = Eklipse::Path::CheckPathValid(meshComp->meshPath, { ".obj" });	 // TODO: Check other formats
					valid = valid && Eklipse::Path::CheckPathValid(meshComp->materialPath, { EK_MATERIAL_EXTENSION });

					if (valid)
					{
						meshComp->mesh = Eklipse::Project::GetActive()->GetAssetLibrary()->GetMesh(meshComp->meshPath).get();
						meshComp->material = Eklipse::Project::GetActive()->GetAssetLibrary()->GetMaterial(meshComp->materialPath).get();
					}
				}
				ImGui::Unindent();
			}
		}

		// Script
		{
			// script switch combo
			auto* scriptComp = entity.TryGetComponent<Eklipse::ScriptComponent>();
			if (scriptComp != nullptr && ImGui::CollapsingHeader("Script"))
			{
				ImGui::Indent();
				if (ImGui::BeginCombo("Script##Combo", scriptComp->scriptName.c_str()))
				{
					for (auto&& [className, classInfo] : Eklipse::Project::GetScriptClasses())
					{
						bool isSelected = (scriptComp->scriptName.c_str() == className);
						if (ImGui::Selectable(className.c_str(), isSelected))
						{
							if (scriptComp->script != nullptr)
								scriptComp->DestroyScript();
							scriptComp->SetScript(className, classInfo, entity);
						}
						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
				ImGui::Unindent();
			}

			// script properties
			
			if (scriptComp != nullptr && scriptComp->script != nullptr)
			{
				ImGui::Spacing();
				ImGui::Text("Properties");
				ImGui::Spacing();

				ImGui::Indent();
				for (auto&& [name, member] : scriptComp->classInfo.members)
				{
					if (member.type == "bool")
						ImGui::Checkbox(name.c_str(), scriptComp->GetScriptValue<bool>(member.offset));
					else if (member.type == "int")
						ImGui::DragInt(name.c_str(), scriptComp->GetScriptValue<int>(member.offset));
					else if (member.type == "float")
						ImGui::DragFloat(name.c_str(), scriptComp->GetScriptValue<float>(member.offset));
					else if (member.type == "std::string")
						ImGui::InputText(name.c_str(), scriptComp->GetScriptValue<std::string>(member.offset));
					else if (member.type == "glm::vec2")
						ImGui::DragFloat2(name.c_str(), glm::value_ptr(*scriptComp->GetScriptValue<glm::vec2>(member.offset)));
					else if (member.type == "glm::vec3")
						ImGui::DragFloat3(name.c_str(), glm::value_ptr(*scriptComp->GetScriptValue<glm::vec3>(member.offset)));
					else if (member.type == "glm::vec4")
						ImGui::DragFloat4(name.c_str(), glm::value_ptr(*scriptComp->GetScriptValue<glm::vec4>(member.offset)));
				}
				ImGui::Unindent();
			}
		}
	}
	void DetailsPanel::OnMaterialGUI(Eklipse::Material* material)
	{
		ImGui::Text(("Material: " + material->GetName()).c_str());
		const char* id = material->GetName().c_str();

		ImGui::InputPath(id, "Shader", material->GetShader()->GetPath(), { EK_SHADER_EXTENSION },
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
					ImGui::DragFloat(valueName.c_str(), (float*)value.data);
				else if (value.type == Eklipse::DataType::FLOAT2)
					ImGui::DragFloat2(valueName.c_str(), (float*)value.data);
				else if (value.type == Eklipse::DataType::FLOAT3)
					ImGui::DragFloat3(valueName.c_str(), (float*)value.data);
				else if (value.type == Eklipse::DataType::FLOAT4)
					ImGui::DragFloat4(valueName.c_str(), (float*)value.data);
				else if (value.type == Eklipse::DataType::INT)
					ImGui::DragInt(valueName.c_str(), (int*)value.data);
				else if (value.type == Eklipse::DataType::INT2)
					ImGui::DragInt2(valueName.c_str(), (int*)value.data);
				else if (value.type == Eklipse::DataType::INT3)
					ImGui::DragInt3(valueName.c_str(), (int*)value.data);
				else if (value.type == Eklipse::DataType::INT4)
					ImGui::DragInt4(valueName.c_str(), (int*)value.data);
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
