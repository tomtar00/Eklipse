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
        else if (info.type == SelectionType::SHADER)
        {
            OnShaderGUI(info.shader);
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

#ifdef EK_DEBUG
        // ID
        {
            ImGui::DrawProperty("id", "ID", [&]() {
                ImGui::Text("%llu", entity.GetUUID());
            });
        }
#endif

        // Name
        {
            ImGui::DrawProperty("namecomp", "Name", [&]() {
                auto& name = entity.GetComponent<NameComponent>().name;
                ImGui::InputText("##EntityName", &name);
            });
            ImGui::Dummy({ 0.0f, 5.0f });
        }

        // Transform
        {
            auto& transComp = entity.GetComponent<TransformComponent>();
            ImGui::InputVec3("pos", "Position", EK_PROPERTY_WIDTH, transComp.transform.position, 0.1f, 0.0f);
            ImGui::InputVec3("rot", "Rotation", EK_PROPERTY_WIDTH, transComp.transform.rotation, 0.1f, 0.0f);
            ImGui::InputVec3("scal", "Scale", EK_PROPERTY_WIDTH, transComp.transform.scale, 0.1f, 1.0f);
            ImGui::Dummy({ 0.0f, 5.0f });
        }

        // Camera
        {
            auto* cameraComp = entity.TryGetComponent<CameraComponent>();
            if (cameraComp && ImGui::CollapsingHeader("Camera"))
            {
                ImGui::DrawProperty("camera_main", "Is Main", [&]() {
                    ImGui::Checkbox("##IsMain", &cameraComp->camera.m_isMain);
                });
                ImGui::DrawProperty("camera_fov", "Field of view", [&]() {
                    ImGui::SliderFloat("##FOV", &cameraComp->camera.m_fov, 0.0f, 180.0f);
                });
                ImGui::DrawProperty("camera_near", "Near plane", [&]() {
                    ImGui::SliderFloat("##Near", &cameraComp->camera.m_nearPlane, 0.01f, 1000.0f);
                });
                ImGui::DrawProperty("camera_far", "Far plane", [&]() {
                    ImGui::SliderFloat("##Far", &cameraComp->camera.m_farPlane, 0.01f, 1000.0f);
                });
            }
        }

        // Mesh
        {
            auto* meshComp = entity.TryGetComponent<MeshComponent>();
            if (meshComp != nullptr && ImGui::CollapsingHeader("Mesh"))
            {
                if (ImGui::InputAsset(&entity.GetUUID(), "Mesh", AssetType::Mesh, meshComp->meshHandle))
                {
                    meshComp->mesh = AssetManager::GetAsset<Mesh>(meshComp->meshHandle).get();
                }

                if (ImGui::InputAsset(&entity.GetUUID(), "Material", AssetType::Material, meshComp->materialHandle))
                {
                    meshComp->material = AssetManager::GetAsset<Material>(meshComp->materialHandle).get();
                }
            }
        }

        // Script
        {
            auto* scriptComp = entity.TryGetComponent<ScriptComponent>();
            if (scriptComp != nullptr && ImGui::CollapsingHeader("Script"))
            {
                // script switch combo
                ImGui::DrawProperty("script_combo", "Script", [&]() {
                    if (ImGui::BeginCombo("##ScriptCombo", scriptComp->scriptName.c_str()))
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
                });

                ImGui::Dummy({ 0.0f, 3.0f });
            
                // script properties
                if (scriptComp != nullptr && scriptComp->script != nullptr)
                {
                    auto& scriptClassMap = ScriptLinker::Get().GetScriptClasses();
                    EK_ASSERT(scriptClassMap.find(scriptComp->scriptName) != scriptClassMap.end(), "Script '{}' not found!", scriptComp->scriptName);
                    auto& classRef = scriptClassMap.at(scriptComp->scriptName).reflection;

                    if (classRef.members.size() > 0)
                    {
                        int i = 0;
                        for (auto& [memberName, memberRef] : classRef.members)
                        {
                            if (memberRef.memberType == "EK_INT")
                            {
                                ImGui::DrawProperty((void*)i++, memberRef.memberName.c_str(), [&]() {
                                    ImGui::DragInt("##prop", scriptComp->GetScriptValue<int>(memberRef.memberOffset));
                                });
                            }
                            else if (memberRef.memberType == "EK_FLOAT")
                            {
                                ImGui::DrawProperty((void*)i++, memberRef.memberName.c_str(), [&]() {
                                    ImGui::DragFloat("##prop", scriptComp->GetScriptValue<float>(memberRef.memberOffset));
                                });
                            }
                            else if (memberRef.memberType == "EK_STR")
                            {
                                ImGui::DrawProperty((void*)i++, memberRef.memberName.c_str(), [&]() {
                                    ImGui::InputText("##prop", scriptComp->GetScriptValue<String>(memberRef.memberOffset));
                                });
                            }
                            else if (memberRef.memberType == "EK_BOOL")
                            {
                                ImGui::DrawProperty((void*)i++, memberRef.memberName.c_str(), [&]() {
                                    ImGui::Checkbox("##prop", scriptComp->GetScriptValue<bool>(memberRef.memberOffset));
                                });
                            }
                            else if (memberRef.memberType == "EK_VEC2")
                            {
                                ImGui::DrawProperty((void*)i++, memberRef.memberName.c_str(), [&]() {
                                    ImGui::DragFloat2("##prop", glm::value_ptr(*scriptComp->GetScriptValue<glm::vec2>(memberRef.memberOffset)));
                                });
                            }
                            else if (memberRef.memberType == "EK_VEC3")
                            {
                                ImGui::DrawProperty((void*)i++, memberRef.memberName.c_str(), [&]() {
                                    ImGui::DragFloat3("##prop", glm::value_ptr(*scriptComp->GetScriptValue<glm::vec3>(memberRef.memberOffset)));
                                   });
                            }
                            else if (memberRef.memberType == "EK_VEC4")
                            {
                                ImGui::DrawProperty((void*)i++, memberRef.memberName.c_str(), [&]() {
                                    ImGui::DragFloat4("##prop", glm::value_ptr(*scriptComp->GetScriptValue<glm::vec4>(memberRef.memberOffset)));
                                });
                            }
                        }
                    }
                }
            }
        }
    }
    void DetailsPanel::OnMaterialGUI(Material* material)
    {
        ImGui::Text(("Material: " + material->Name).c_str());
        const char* id = material->Name.c_str();

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
                auto& metadata = AssetManager::GetMetadata(sampler.textureHandle);
                material->ApplyChanges(metadata.FilePath);
            }
        }
        for (auto&& [name, pushConstant] : material->GetPushConstants())
        {
            ImGui::Spacing();
            for (auto&& [valueName, value] : pushConstant.dataPointers)
            {
                if (value.type == ShaderDataType::BOOL)
                {
                    ImGui::DrawProperty((void*)(id + i++), valueName.c_str(), [&]() {
                        ImGui::Checkbox("##check", (bool*)value.data);
                    });
                }
                else if (value.type == ShaderDataType::FLOAT)
                {
                    ImGui::DrawProperty((void*)(id + i++), valueName.c_str(), [&]() {
                        ImGui::DragFloat("##check", (float*)value.data);
                    });
                }
                else if (value.type == ShaderDataType::FLOAT2)
                {
                    ImGui::DrawProperty((void*)(id + i++), valueName.c_str(), [&]() {
                        ImGui::DragFloat2("##check", (float*)value.data);
                    });
                }
                else if (value.type == ShaderDataType::FLOAT3)
                {
                    ImGui::DrawProperty((void*)(id + i++), valueName.c_str(), [&]() {
                        ImGui::DragFloat3("##check", (float*)value.data);
                    });
                }
                else if (value.type == ShaderDataType::FLOAT4)
                {
                    ImGui::DrawProperty((void*)(id + i++), valueName.c_str(), [&]() {
                        ImGui::ColorEdit4("##check", (float*)value.data);
                    });
                }
                else if (value.type == ShaderDataType::INT)
                {
                    ImGui::DrawProperty((void*)(id + i++), valueName.c_str(), [&]() {
                        ImGui::DragInt("##check", (int*)value.data);
                    });
                }
                else if (value.type == ShaderDataType::INT2)
                {
                    ImGui::DrawProperty((void*)(id + i++), valueName.c_str(), [&]() {
                        ImGui::DragInt2("##check", (int*)value.data);
                    });
                }
                else if (value.type == ShaderDataType::INT3)
                {
                    ImGui::DrawProperty((void*)(id + i++), valueName.c_str(), [&]() {
                        ImGui::DragInt3("##check", (int*)value.data);
                    });
                }
                else if (value.type == ShaderDataType::INT4)
                {
                    ImGui::DrawProperty((void*)(id + i++), valueName.c_str(), [&]() {
                        ImGui::DragInt4("##check", (int*)value.data);
                    });
                }
            }
        }
    }
    void DetailsPanel::OnShaderGUI(Shader* shader)
    {
        ImGui::DrawProperty("shader_name", "Name", [&]() {
            ImGui::Text(shader->Name.c_str());
        });

        ImGui::Spacing();

        auto& reflections = shader->GetReflections();
        for (auto& [stage, reflection] : reflections)
        {
            if (ImGui::CollapsingHeader(ShaderStageToString(stage).c_str()))
            {
                if (reflection.inputs.size() > 0)
                    ImGui::SeparatorText("Input");
                for (auto& input : reflection.inputs)
                {
                    ImGui::DrawProperty((void*)input.offset, input.name.c_str(), [&]() {
                        ImGui::Text(ShaderDataTypeToString(input.type).c_str());
                    });
                }
                ImGui::Spacing();

                if (reflection.outputs.size() > 0)
                    ImGui::SeparatorText("Output");
                for (auto& output : reflection.outputs)
                {
                    ImGui::DrawProperty((void*)output.name.c_str(), output.name.c_str(), [&]() {
                        ImGui::Text(ShaderDataTypeToString(output.type).c_str());
                    });
                }
                ImGui::Spacing();

                if (reflection.uniformBuffers.size() > 0)
                    ImGui::SeparatorText("Uniform");
                for (auto& uniform : reflection.uniformBuffers)
                {
                    for (auto& member : uniform.members)
                    {
                        ImGui::DrawProperty((void*)member.offset, member.name.c_str(), [&]() {
                            ImGui::Text(ShaderDataTypeToString(member.type).c_str());
                        });
                    }
                }
                ImGui::Spacing();

                if (reflection.pushConstants.size() > 0)
                    ImGui::SeparatorText("Push Constant");
                for (auto& pushConstant : reflection.pushConstants)
                {
                    for (auto& member : pushConstant.members)
                    {
                        ImGui::DrawProperty((void*)member.offset, member.name.c_str(), [&]() {
                            ImGui::Text(ShaderDataTypeToString(member.type).c_str());
                        });
                    }
                }
            }
            ImGui::Spacing();
        }

        ImGui::Spacing();

        if (ImGui::Button("Recompile"))
        {
            Path shaderPath = AssetManager::GetMetadata(shader->Handle).FilePath;
            shader->Recompile(shaderPath);
        }
    }
}