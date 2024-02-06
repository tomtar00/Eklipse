#include "DetailsPanel.h"
#include "EntitiesPanel.h"
#include "EditorLayer.h"

#include <Eklipse/Scene/Components.h>

#include <misc/cpp/imgui_stdlib.h>
#include <glm/gtc/type_ptr.hpp>

namespace Eklipse
{
	void DetailsPanel::Setup(String& name)
	{
		m_entityNameBuffer = name;
	}
	bool DetailsPanel::OnGUI(float deltaTime)
	{
		EK_PROFILE();

		if (!GuiPanel::OnGUI(deltaTime)) return false;

		ImGui::Begin("Details");

		DetailsSelectionInfo& info = EditorLayer::Get().SelectionInfo;

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
	void DetailsPanel::OnEntityGUI(Entity entity)
	{
		// Right-click menu
		{
			if (ImGui::BeginPopupContextWindow())
			{
				if (ImGui::MenuItem("Add Mesh Component") && !entity.HasComponent<MeshComponent>())
					entity.AddComponent<MeshComponent>();

				if (ImGui::MenuItem("Add Script Component") && !entity.HasComponent<ScriptComponent>())
					entity.AddComponent<ScriptComponent>();

				if (ImGui::MenuItem("Add Camera Component") && !entity.HasComponent<CameraComponent>())
					entity.AddComponent<CameraComponent>();

				ImGui::EndPopup();
			}
		}

		// ID
		{
			ImGui::Text("ID: %llu", entity.GetUUID());
		}

		// Name
		{
			if (!ImGui::InputText("Entity Name", &m_entityNameBuffer))
			{
				if (m_entityNameBuffer.size() > 0)
				{
					entity.GetComponent<NameComponent>().name = m_entityNameBuffer;
				}
			}
		}

		// Transform
		{
			auto& transComp = entity.GetComponent<TransformComponent>();
			ImGui::DragFloat3("Position", glm::value_ptr(transComp.transform.position), 0.1f);
			ImGui::DragFloat3("Rotation", glm::value_ptr(transComp.transform.rotation), 0.1f);
			ImGui::DragFloat3("Scale", glm::value_ptr(transComp.transform.scale), 0.1f);
			ImGui::Spacing();
		}

		// Camera
		{
			auto* cameraComp = entity.TryGetComponent<CameraComponent>();
			if (cameraComp && ImGui::CollapsingHeader("Camera"))
			{
				ImGui::Checkbox("Is Main", &cameraComp->camera.m_isMain);
				ImGui::SliderFloat("FOV", &cameraComp->camera.m_fov, 0.0f, 180.0f);
				ImGui::SliderFloat("Near", &cameraComp->camera.m_nearPlane, 0.01f, 1000.0f);
				ImGui::SliderFloat("Far", &cameraComp->camera.m_farPlane, 0.01f, 1000.0f);
			}
		}

		// Mesh
		{
			auto* meshComp = entity.TryGetComponent<MeshComponent>();
			if (meshComp != nullptr && ImGui::CollapsingHeader("Mesh"))
			{
				ImGui::Indent();
				if (ImGui::BeginTable("##MeshComponent", 2))
				{
					ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 100.0f);
					ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::TextUnformatted("Mesh");
					ImGui::TableSetColumnIndex(1);
					if (ImGui::InputAsset(&entity.GetUUID(), nullptr, AssetType::Mesh, meshComp->meshHandle))
					{
						meshComp->mesh = AssetManager::GetAsset<Mesh>(meshComp->meshHandle).get();
					}

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::TextUnformatted("Material");
					ImGui::TableSetColumnIndex(1);
					if (ImGui::InputAsset(&entity.GetUUID(), nullptr, AssetType::Material, meshComp->materialHandle))
					{
						meshComp->material = AssetManager::GetAsset<Material>(meshComp->materialHandle).get();
					}

					ImGui::EndTable();
				}
				ImGui::Unindent();
			}
		}

		// Script
		{
			auto* scriptComp = entity.TryGetComponent<ScriptComponent>();
			if (scriptComp != nullptr && ImGui::CollapsingHeader("Script"))
			{
				ImGui::Indent();

				// script switch combo
				if (ImGui::BeginCombo("Script##Combo", scriptComp->scriptName.c_str()))
				{
					for (auto&& [className, classInfo] : ScriptLinker::Get().GetScriptClasses())
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
			
				// script properties
				if (scriptComp != nullptr && scriptComp->script != nullptr)
				{
					auto& scriptClassMap = ScriptLinker::Get().GetScriptClasses();
					EK_ASSERT(scriptClassMap.find(scriptComp->scriptName) != scriptClassMap.end(), "Script '{}' not found!", scriptComp->scriptName);
					auto& classRef = scriptClassMap.at(scriptComp->scriptName).reflection;

					if (classRef.members.size() > 0)
					{
						ImGui::Spacing();
						ImGui::Text("Properties");
						ImGui::Spacing();

						for (auto& [memberName, memberRef] : classRef.members)
						{
							if (memberRef.memberType == "EK_INT")
								ImGui::DragInt(memberRef.memberName.c_str(), scriptComp->GetScriptValue<int>(memberRef.memberOffset));
							else if (memberRef.memberType == "EK_FLOAT")
								ImGui::DragFloat(memberRef.memberName.c_str(), scriptComp->GetScriptValue<float>(memberRef.memberOffset));
							else if (memberRef.memberType == "EK_STR")
								ImGui::InputText(memberRef.memberName.c_str(), scriptComp->GetScriptValue<String>(memberRef.memberOffset));
							else if (memberRef.memberType == "EK_BOOL")
								ImGui::Checkbox(memberRef.memberName.c_str(), scriptComp->GetScriptValue<bool>(memberRef.memberOffset));
							else if (memberRef.memberType == "EK_VEC2")
								ImGui::DragFloat2(memberRef.memberName.c_str(), glm::value_ptr(*scriptComp->GetScriptValue<glm::vec2>(memberRef.memberOffset)));
							else if (memberRef.memberType == "EK_VEC3")
								ImGui::DragFloat3(memberRef.memberName.c_str(), glm::value_ptr(*scriptComp->GetScriptValue<glm::vec3>(memberRef.memberOffset)));
							else if (memberRef.memberType == "EK_VEC4")
								ImGui::DragFloat4(memberRef.memberName.c_str(), glm::value_ptr(*scriptComp->GetScriptValue<glm::vec4>(memberRef.memberOffset)));
						}
					}
				}

				ImGui::Unindent();
			}
		}
	}
	void DetailsPanel::OnMaterialGUI(Material* material)
	{
		ImGui::Text(("Material: " + material->GetName()).c_str());
		const char* id = material->GetName().c_str();

		if (ImGui::InputAsset(id, "Shader", AssetType::Shader, material->GetShaderHandle()))
		{
			material->SetShader(material->GetShaderHandle());
		}

		ImGui::Spacing();

		uint16_t i = 0;
		for (auto&& [textureName, sampler] : material->GetSamplers())
		{
			if (ImGui::InputAsset(id+(i++), textureName.c_str(), AssetType::Texture2D, sampler.textureHandle))
			{
				EK_CORE_TRACE("Texture path changed to: {0}", sampler.texturePath.string());
			}
		}
		for (auto&& [name, pushConstant] : material->GetPushConstants())
		{
			ImGui::Spacing();
			ImGui::Text("%s", name.c_str());
			for (auto&& [valueName, value] : pushConstant.dataPointers)
			{
				if (value.type == ShaderDataType::BOOL)
					ImGui::Checkbox(valueName.c_str(), (bool*)value.data);
				else if (value.type == ShaderDataType::FLOAT)
					ImGui::DragFloat(valueName.c_str(), (float*)value.data);
				else if (value.type == ShaderDataType::FLOAT2)
					ImGui::DragFloat2(valueName.c_str(), (float*)value.data);
				else if (value.type == ShaderDataType::FLOAT3)
					ImGui::DragFloat3(valueName.c_str(), (float*)value.data);
				else if (value.type == ShaderDataType::FLOAT4)
					ImGui::DragFloat4(valueName.c_str(), (float*)value.data);
				else if (value.type == ShaderDataType::INT)
					ImGui::DragInt(valueName.c_str(), (int*)value.data);
				else if (value.type == ShaderDataType::INT2)
					ImGui::DragInt2(valueName.c_str(), (int*)value.data);
				else if (value.type == ShaderDataType::INT3)
					ImGui::DragInt3(valueName.c_str(), (int*)value.data);
				else if (value.type == ShaderDataType::INT4)
					ImGui::DragInt4(valueName.c_str(), (int*)value.data);
			}
		}

		if (ImGui::Button("Apply"))
		{
			bool allValid = true;
			for (auto&& [textureName, sampler] : material->GetSamplers())
			{
				allValid = AssetManager::IsAssetHandleValid(sampler.textureHandle);
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
