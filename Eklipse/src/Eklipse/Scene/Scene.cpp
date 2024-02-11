#include "precompiled.h"
#include "Scene.h"
#include "Entity.h"
#include "Components.h"

#include <Eklipse/Assets/AssetManager.h>
#include <dylib.hpp>

namespace Eklipse
{
    template<typename... Component>
    static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
    {
        EK_CORE_PROFILE();
        ([&]()
        {
            auto view = src.view<Component>();
            for (auto srcEntity : view)
            {
                entt::entity dstEntity = enttMap.at(src.get<IDComponent>(srcEntity).ID);

                auto& srcComponent = src.get<Component>(srcEntity);
                dst.emplace_or_replace<Component>(dstEntity, srcComponent);
            }
        }(), ...);
    }

    template<typename... Component>
    static void CopyComponent(ComponentGroup<Component...>, entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
    {
        EK_CORE_PROFILE();
        CopyComponent<Component...>(dst, src, enttMap);
    }

    template<typename... Component>
    static void CopyComponentIfExists(Entity dst, Entity src)
    {
        EK_CORE_PROFILE();
        ([&]()
        {
            if (src.HasComponent<Component>())
                dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
        }(), ...);
    }

    template<typename... Component>
    static void CopyComponentIfExists(ComponentGroup<Component...>, Entity dst, Entity src)
    {
        EK_CORE_PROFILE();
        CopyComponentIfExists<Component...>(dst, src);
    }

    Scene::Scene() { Name = "Default"; }
    Scene::~Scene()
    {
        EK_CORE_PROFILE();
        EK_CORE_TRACE("Begin scene '{0}' destruction", Name);

        OnSceneStop();

        EK_CORE_TRACE("Scene '{0}' destroyed", Name);
    }

    // Scene Events
    void Scene::OnSceneStart()
    {
        EK_CORE_PROFILE();
        EK_CORE_TRACE("Starting scene '{0}'", Name);

        // set main camera
        m_registry.view<CameraComponent>().each([&](auto entityID, auto& cameraComponent)
        {
            if (cameraComponent.camera.m_isMain)
            {
                m_mainCamera = &cameraComponent.camera;
                m_mainCameraTransform = &m_registry.get<TransformComponent>(entityID).transform;
            }
        });

        m_registry.view<ScriptComponent>().each([&](auto entityID, auto& scriptComponent)
        {
            EK_ASSERT(scriptComponent.script, "Script is null");
            scriptComponent.script->OnCreate();
        });

        EK_CORE_DBG("Scene '{0}' started", Name);
    }
    void Scene::OnSceneUpdate(float deltaTime)
    {
        EK_CORE_PROFILE();

        try
        {
            m_registry.view<ScriptComponent>().each([&](auto entityID, auto& scriptComponent)
            {
                EK_ASSERT(scriptComponent.script, "Script is null");
                scriptComponent.script->OnUpdate(deltaTime);
            });
        }
        catch (const std::exception& e)
        {
            EK_CORE_ERROR("Exception thrown in script: {0}", e.what());
        }
    }
    void Scene::OnScenePause()
    {
        EK_CORE_PROFILE();
        EK_CORE_TRACE("Pausing scene '{0}'", Name);

        EK_CORE_DBG("Scene '{0}' paused", Name);
    }
    void Scene::OnSceneResume()
    {
        EK_CORE_PROFILE();
        EK_CORE_TRACE("Resuming scene '{0}'", Name);

        EK_CORE_DBG("Scene '{0}' resumed", Name);
    }
    void Scene::OnSceneStop()
    {
        EK_CORE_PROFILE();
        EK_CORE_TRACE("Stoping scene '{0}'", Name);
        DestroyAllScripts();
        EK_CORE_DBG("Scene '{0}' stoped", Name);
    }

    // Scene
    Ref<Scene> Scene::Copy(Scene* other)
    {
        EK_CORE_PROFILE();
        EK_CORE_TRACE("Copying scene '{0}'", other->Name);

        Ref<Scene> newScene = Scene::New(other->Name);

        auto& srcSceneRegistry = other->m_registry;
        auto& dstSceneRegistry = newScene->m_registry;
        std::unordered_map<UUID, entt::entity> enttMap;

        srcSceneRegistry.view<IDComponent>().each([&](auto entityID, auto& idComponent)
        {
            UUID uuid = idComponent.ID;
            const auto& name = srcSceneRegistry.get<NameComponent>(entityID).name;
            Entity newEntity = newScene->CreateEntity(uuid, name);
            enttMap[uuid] = newEntity.GetHandle();

            EK_CORE_TRACE("Copying entity with ID = {0}, name = {1}", uuid, name);
        });

        CopyComponent(AllComponents{}, dstSceneRegistry, srcSceneRegistry, enttMap);

        auto& scriptClassMap = ScriptLinker::Get().GetScriptClasses();
        auto view = srcSceneRegistry.view<ScriptComponent>();
        for (auto srcEntity : view)
        {
            UUID uuid = srcSceneRegistry.get<IDComponent>(srcEntity).ID;
            entt::entity dstEntity = enttMap.at(uuid);
            Entity dstEntityHandle = { dstEntity, newScene.get() };

            auto& srcComponent = srcSceneRegistry.get<ScriptComponent>(srcEntity);
            auto& dstComponent = dstEntityHandle.AddComponent<ScriptComponent>();
            EK_ASSERT(scriptClassMap.find(srcComponent.scriptName) != scriptClassMap.end(), "Script '{}' not found", srcComponent.scriptName);
            auto& scriptClassInfo = scriptClassMap.at(srcComponent.scriptName);

            dstComponent.SetScript(srcComponent.scriptName, scriptClassInfo, dstEntityHandle);

            EK_CORE_TRACE("Copying script '{0}' of entity with ID = {1}, name = {2}", srcComponent.scriptName, uuid, dstEntityHandle.GetName());

            for (auto& [memberName, memberRef] : scriptClassInfo.reflection.members)
            {
                if (memberRef.memberType == "EK_INT")
                {
                    int value = *srcComponent.GetScriptValue<int>(memberRef.memberOffset);
                    dstComponent.SetScriptValue(memberRef.memberOffset, value);
                    EK_CORE_TRACE("Copying script property '{}' of entity with ID = {}, value = {}", memberRef.memberName, uuid, value);
                }
                else if (memberRef.memberType == "EK_FLOAT")
                {
                    float value = *srcComponent.GetScriptValue<float>(memberRef.memberOffset);
                    dstComponent.SetScriptValue(memberRef.memberOffset, value);
                    EK_CORE_TRACE("Copying script property '{}' of entity with ID = {}, value = {}", memberRef.memberName, uuid, value);
                }
                else if (memberRef.memberType == "EK_BOOL")
                {
                    bool value = *srcComponent.GetScriptValue<bool>(memberRef.memberOffset);
                    dstComponent.SetScriptValue(memberRef.memberOffset, value);
                    EK_CORE_TRACE("Copying script property '{}' of entity with ID = {}, value = {}", memberRef.memberName, uuid, value);
                }
                else if (memberRef.memberType == "EK_STR")
                {
                    String& value = *srcComponent.GetScriptValue<String>(memberRef.memberOffset);
                    dstComponent.SetScriptValue(memberRef.memberOffset, value);
                    EK_CORE_TRACE("Copying script property '{}' of entity with ID = {}, value = {}", memberRef.memberName, uuid, value);
                }
                else if (memberRef.memberType == "EK_VEC2")
                    dstComponent.SetScriptValue(memberRef.memberOffset, *srcComponent.GetScriptValue<glm::vec2>(memberRef.memberOffset));
                else if (memberRef.memberType == "EK_VEC3")
                    dstComponent.SetScriptValue(memberRef.memberOffset, *srcComponent.GetScriptValue<glm::vec3>(memberRef.memberOffset));
                else if (memberRef.memberType == "EK_VEC4")
                    dstComponent.SetScriptValue(memberRef.memberOffset, *srcComponent.GetScriptValue<glm::vec4>(memberRef.memberOffset));
            }
        }
        return newScene;
    }
    Ref<Scene> Scene::New(const String& name)
    {
        EK_CORE_PROFILE();
        auto& scene = CreateRef<Scene>();
        scene->Name = name;
        return scene;
    }
    Ref<Scene> Scene::Load(const Path& filePath, const AssetHandle handle)
    {
        EK_CORE_PROFILE();
        EK_CORE_TRACE("Loading scene from '{0}'", filePath.string());

        Ref<Scene> scene = Scene::New("");
        scene->Handle = handle;
        if (!scene->Deserialize(filePath))
        {
            EK_CORE_ERROR("Failed to load scene '{0}'", filePath.string());
            return nullptr;
        }

        scene->InitializeAllScripts(filePath);

        EK_CORE_DBG("Scene '{0}' loaded", scene->Name);
        return scene;
    }
    void Scene::Save(Ref<Scene> scene, const Path& filePath)
    {
        EK_CORE_PROFILE();
        EK_CORE_TRACE("Saving scene '{0}'", scene->Name);
        if (!scene->Serialize(filePath))
        {
            EK_CORE_ERROR("Failed to save scene '{0}'", scene->Name);
        }
        EK_CORE_TRACE("Scene '{0}' saved to path '{1}'", scene->Name, filePath.string());
    }
    
    // Scripts
    void Scene::DestroyAllScripts()
    {
        EK_CORE_PROFILE();
        EK_CORE_TRACE("Destroying all scripts on scene '{0}'", Name);
        m_registry.view<ScriptComponent>().each([&](auto entityID, auto& scriptComponent)
        {
            scriptComponent.DestroyScript();
        });
        EK_CORE_DBG("All scripts on scene '{0}' destroyed", Name);
    }
    void Scene::InitializeAllScripts(const Path& filePath)
    {
        EK_CORE_PROFILE();
        EK_CORE_TRACE("Initializing all scripts on scene '{0}'", Name);
        const ScriptClassMap& scriptClasseMap = ScriptLinker::Get().GetScriptClasses();

        m_registry.view<ScriptComponent>().each([&](auto entityID, auto& scriptComponent)
        {
            if (scriptComponent.script != nullptr)
            {
                EK_CORE_ERROR("Script '{0}' has never been destroyed!", scriptComponent.scriptName);
            }
            else
            {
                auto it = scriptClasseMap.find(scriptComponent.scriptName);
                if (it == scriptClasseMap.end())
                {
                    EK_CORE_ERROR("Failed to initialize script '{0}'", scriptComponent.scriptName);
                }
                else scriptComponent.SetScript(it->first, it->second, Entity(entityID, this));
            }
        });

        DeserializeEveryScriptProperties(filePath);

        EK_CORE_DBG("All scripts on scene '{0}' initialized", Name);
    }

    // Components
    void Scene::ApplyAllComponents()
    {
        EK_CORE_PROFILE();
        EK_CORE_TRACE("Applying all components on scene '{0}'", Name);
        m_registry.view<MeshComponent>().each([&](auto entityID, auto& meshComponent)
        {
            if (AssetManager::IsAssetHandleValid(meshComponent.meshHandle) && AssetManager::IsAssetHandleValid(meshComponent.materialHandle))
            {
                meshComponent.mesh = AssetManager::GetAsset<Mesh>(meshComponent.meshHandle).get();
                meshComponent.material = AssetManager::GetAsset<Material>(meshComponent.materialHandle).get();
            }
        });
        EK_CORE_DBG("All components on scene '{0}' applied", Name);
    }

    // Entity
    Entity Scene::CreateEntity(const String& name)
    {
        EK_CORE_PROFILE();
        return CreateEntity(UUID(), name);
    }
    Entity Scene::CreateEntity(UUID uuid, const String& name)
    {
        EK_CORE_PROFILE();
        EK_CORE_TRACE("Creating entity with ID = {0}, name = {1}", uuid, name);
        Entity entity = { m_registry.create(), this };
        entity.AddComponent<IDComponent>(uuid);
        entity.AddComponent<NameComponent>(name.empty() ? "Empty Entity" : name);
        entity.AddComponent<TransformComponent>();

        m_entityMap[uuid] = entity.GetHandle();

        EK_CORE_DBG("Entity with ID = {0}, name = {1} created", uuid, entity.GetName());
        return entity;
    }
    Entity Scene::GetEntity(UUID uuid)
    {
        EK_CORE_PROFILE();
        auto it = m_entityMap.find(uuid);
        if (it != m_entityMap.end())
            return { it->second, this };

        EK_CORE_ERROR("Failed to find entity {0} on scene '{1}'", uuid, Name);
        return {};
    }
    void Scene::DestroyEntity(Entity entity)
    {
        EK_CORE_PROFILE();
        String name = entity.GetName();
        UUID uuid = entity.GetUUID();
        EK_CORE_TRACE("Destroying entity with ID = {0}, name = {1}", uuid, name);
        m_entityMap.erase(uuid);
        m_registry.destroy(entity.GetHandle());
        EK_CORE_DBG("Entity with ID = {0}, name = {1} destroyed", uuid, name);
    }

    // Serialization
    bool Scene::Serialize(const Path& filePath)
    {
        EK_CORE_PROFILE();
        EK_CORE_TRACE("Serializing scene '{0}'...", Name);

        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Scene" << YAML::Value << Name;
        out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
        m_registry.each([&](auto entityID)
        {
            Entity entity = { entityID, this };
            SerializeEntity(entity, out);
        });
        out << YAML::EndSeq;
        out << YAML::EndMap;

        std::ofstream fout(filePath);
        fout << out.c_str();

        EK_CORE_DBG("Serialized scene '{0}' to path '{1}'", Name, filePath.string());
        return true;
    }
    bool Scene::SerializeEntity(Entity entity, YAML::Emitter& out)
    {
        EK_CORE_PROFILE();
        EK_ASSERT(entity.HasComponent<IDComponent>(), "Tried to serialize entity with no ID!");

        out << YAML::BeginMap;
        out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();

        EK_CORE_TRACE("Serializing entity with ID = {0}", entity.GetUUID());

        if (entity.HasComponent<NameComponent>())
        {
            out << YAML::Key << "NameComponent";
            out << YAML::BeginMap;

            auto& tag = entity.GetComponent<NameComponent>().name;
            out << YAML::Key << "Name" << YAML::Value << tag;

            out << YAML::EndMap;
        }

        if (entity.HasComponent<TransformComponent>())
        {
            out << YAML::Key << "TransformComponent";
            out << YAML::BeginMap;

            auto& tc = entity.GetComponent<TransformComponent>();
            out << YAML::Key << "Position" << YAML::Value << tc.transform.position;
            out << YAML::Key << "Rotation" << YAML::Value << tc.transform.rotation;
            out << YAML::Key << "Scale" << YAML::Value << tc.transform.scale;

            out << YAML::EndMap;
        }

        if (entity.HasComponent<CameraComponent>())
        {
            out << YAML::Key << "CameraComponent";
            out << YAML::BeginMap;

            auto& cameraComponent = entity.GetComponent<CameraComponent>();
            auto& camera = cameraComponent.camera;

            out << YAML::Key << "Camera" << YAML::Value;
            out << YAML::BeginMap;
            out << YAML::Key << "IsMain" << YAML::Value << camera.m_isMain;
            out << YAML::Key << "FOV" << YAML::Value << camera.m_fov;
            out << YAML::Key << "Near" << YAML::Value << camera.m_nearPlane;
            out << YAML::Key << "Far" << YAML::Value << camera.m_farPlane;
            out << YAML::EndMap;

            out << YAML::EndMap;
        }

        if (entity.HasComponent<MeshComponent>())
        {
            out << YAML::Key << "MeshComponent";
            out << YAML::BeginMap;

            auto& meshComponent = entity.GetComponent<MeshComponent>();
            out << YAML::Key << "Mesh" << YAML::Value << meshComponent.meshHandle;
            out << YAML::Key << "Material" << YAML::Value << meshComponent.materialHandle;

            out << YAML::EndMap;
        }

        if (entity.HasComponent<ScriptComponent>())
        {
            out << YAML::Key << "ScriptComponent";
            out << YAML::BeginMap;

            auto& scriptComponent = entity.GetComponent<ScriptComponent>();
            out << YAML::Key << "Name" << YAML::Value << scriptComponent.scriptName;

            out << YAML::Key << "Properties" << YAML::Value;
            out << YAML::BeginSeq;

            Vec<String> toRemove;
            const ScriptClassMap& scriptClasses = ScriptLinker::Get().GetScriptClasses();
            auto scriptIter = scriptClasses.find(scriptComponent.scriptName);
            for (auto& [memberName, memberRef] : scriptIter->second.reflection.members)
            {
                if (scriptComponent.script != nullptr)
                {
                    out << YAML::BeginMap;

                    out << YAML::Key << "Name" << YAML::Value << memberRef.memberName;

                    if (memberRef.memberType == "EK_INT")
                        out << YAML::Key << "Value" << YAML::Value << *scriptComponent.GetScriptValue<int>(memberRef.memberOffset);
                    else if (memberRef.memberType == "EK_FLOAT")
                        out << YAML::Key << "Value" << YAML::Value << *scriptComponent.GetScriptValue<float>(memberRef.memberOffset);
                    else if (memberRef.memberType == "EK_BOOL")
                        out << YAML::Key << "Value" << YAML::Value << *scriptComponent.GetScriptValue<bool>(memberRef.memberOffset);
                    else if (memberRef.memberType == "EK_STR")
                        out << YAML::Key << "Value" << YAML::Value << *scriptComponent.GetScriptValue<String>(memberRef.memberOffset);
                    else if (memberRef.memberType == "EK_VEC2")
                        out << YAML::Key << "Value" << YAML::Value << *scriptComponent.GetScriptValue<glm::vec2>(memberRef.memberOffset);
                    else if (memberRef.memberType == "EK_VEC3")
                        out << YAML::Key << "Value" << YAML::Value << *scriptComponent.GetScriptValue<glm::vec3>(memberRef.memberOffset);
                    else if (memberRef.memberType == "EK_VEC4")
                        out << YAML::Key << "Value" << YAML::Value << *scriptComponent.GetScriptValue<glm::vec4>(memberRef.memberOffset);
                    else
                        EK_CORE_WARN("Unknown type '{0}' for script property '{1}' while serializing entity {2}", memberRef.memberType, memberRef.memberName, entity.GetUUID());

                    out << YAML::Key << "Type" << YAML::Value << memberRef.memberType;
                    out << YAML::Key << "Offset" << YAML::Value << memberRef.memberOffset;
                    
                    out << YAML::EndMap;
                }
                else
                {
                    EK_CORE_WARN("Script '{0}' has never been initialized!", scriptComponent.scriptName);
                }
            }

            out << YAML::EndSeq;
            out << YAML::EndMap;
        }
        out << YAML::EndMap;

        EK_CORE_TRACE("Serialized entity with ID = {0}", entity.GetUUID());

        return true;
    }
    bool Scene::Deserialize(const Path& filePath)
    {
        EK_CORE_PROFILE();
        EK_CORE_TRACE("Deserializing scene...");

        YAML::Node data;
        try
        {
            data = YAML::LoadFile(filePath.string());
        }
        catch (const std::exception& e)
        {
            EK_CORE_ERROR("Failed to load .eksc file '{0}'\n\t{1}", filePath.string(), e.what());
            return false;
        }

        if (!data["Scene"])
            return false;

        String sceneName;
        TryDeserailize<String>(data, "Scene", &sceneName);
        Name = sceneName;

        auto entities = data["Entities"];
        if (entities)
        {
            for (auto entityNode : entities)
            {
                DeserializeEntity(entityNode);
            }
        }

        EK_CORE_DBG("Deserialized scene '{0}'", sceneName);

        return true;
    }
    bool Scene::DeserializeEntity(YAML::Node& entityNode)
    {
        EK_CORE_PROFILE();
        uint64_t uuid;
        TryDeserailize<uint64_t>(entityNode, "Entity", &uuid);

        String name;
        auto nameComponent = entityNode["NameComponent"];
        if (nameComponent)
        {
            TryDeserailize<String>(nameComponent, "Name", &name);
        }

        EK_CORE_TRACE("Deserialing entity with ID = {0}, name = {1}", uuid, name);

        Entity deserializedEntity = CreateEntity(uuid, name);

        auto transformComponent = entityNode["TransformComponent"];
        if (transformComponent)
        {
            auto& tc = deserializedEntity.GetComponent<TransformComponent>();

            TryDeserailize<glm::vec3>(transformComponent, "Position", &tc.transform.position);
            TryDeserailize<glm::vec3>(transformComponent, "Rotation", &tc.transform.rotation);
            TryDeserailize<glm::vec3>(transformComponent, "Scale", &tc.transform.scale);
        }

        auto cameraComponent = entityNode["CameraComponent"];
        if (cameraComponent)
        {
            auto& cc = deserializedEntity.AddComponent<CameraComponent>();
            auto& cameraProps = cameraComponent["Camera"];

            TryDeserailize<bool>(cameraProps, "IsMain", &cc.camera.m_isMain);
            TryDeserailize<float>(cameraProps, "FOV", &cc.camera.m_fov);
            TryDeserailize<float>(cameraProps, "Near", &cc.camera.m_nearPlane);
            TryDeserailize<float>(cameraProps, "Far", &cc.camera.m_farPlane);
        }

        auto meshComponent = entityNode["MeshComponent"];
        if (meshComponent)
        {
            auto& mc = deserializedEntity.AddComponent<MeshComponent>();

            TryDeserailize<AssetHandle>(meshComponent, "Mesh", &mc.meshHandle);
            TryDeserailize<AssetHandle>(meshComponent, "Material", &mc.materialHandle);
        }

        auto scriptComponent = entityNode["ScriptComponent"];
        if (scriptComponent)
        {
            auto scriptName = TryDeserailize<String>(scriptComponent, "Name", "");
            auto& scriptComponent = deserializedEntity.AddComponent<ScriptComponent>();
            scriptComponent.scriptName = scriptName;
            ScriptLinker::Get().FetchScriptClasses({ scriptName });
            /*if (!scriptName.empty())
            {
                EklipseEngine::Reflections::ClassInfo info{};
                library->get_function<void(EklipseEngine::Reflections::ClassInfo&)>("Get__" + sc.scriptName)(info);
                sc.SetScript(sc.scriptName, info, deserializedEntity);

                DeserializeScriptProperties(deserializedEntity, scriptComponent["Properties"]);
            }
            else
            {
                EK_CORE_WARN("Entity {0} has a script component but no script name was provided to instanitiate it!", deserializedEntity.GetUUID());
            }*/
        }

        EK_CORE_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);

        return true;
    }
    bool Scene::DeserializeEveryScriptProperties(const Path& filePath)
    {
        EK_CORE_PROFILE();
        EK_CORE_TRACE("Deserializing all script properties of scene '{0}'...", Name);

        YAML::Node data;
        try
        {
            data = YAML::LoadFile(filePath.string());
        }
        catch (const std::exception& e)
        {
            EK_CORE_ERROR("Failed to load .eksc file '{0}'\n     {1}", filePath.string(), e.what());
            return false;
        }

        if (!data["Scene"])
            return false;

        String sceneName = data["Scene"].as<String>();

        auto entitiesNodes = data["Entities"];
        for (auto entityNode : entitiesNodes)
        {
            Entity entity = GetEntity(entityNode["Entity"].as<uint64_t>());

            auto scriptComponent = entityNode["ScriptComponent"];
            if (scriptComponent)
            {
                auto properties = scriptComponent["Properties"];
                if (properties)
                    DeserializeScriptProperties(entity, properties);
            }
        }

        EK_CORE_TRACE("Deserialized all script properties of scene '{0}'", sceneName);

        return true;
    }
    bool Scene::DeserializeScriptProperties(Entity entity, YAML::Node& propertiesNode)
    {
        EK_CORE_PROFILE();
        EK_CORE_TRACE("Deserializing script properties of entity {0}", entity.GetUUID());

        auto& scriptClassMap = ScriptLinker::Get().GetScriptClasses();
        auto& scriptComponent = entity.GetComponent<ScriptComponent>();
        for (auto propertyNode : propertiesNode)
        {
            String name = TryDeserailize<String>(propertyNode, "Name", "");
            String type = TryDeserailize<String>(propertyNode, "Type", "");
            uint32_t offset = TryDeserailize<uint32_t>(propertyNode, "Offset", (uint32_t)0);

            if (type == "EK_INT")
                scriptComponent.SetScriptValue<int>(offset, TryDeserailize(propertyNode, "Value", 0));
            else if (type == "EK_FLOAT")
                scriptComponent.SetScriptValue<float>(offset, TryDeserailize(propertyNode, "Value", 0.0f));
            else if (type == "EK_BOOL")
                scriptComponent.SetScriptValue<bool>(offset, TryDeserailize(propertyNode, "Value", false));
            else if (type == "EK_STR")
                scriptComponent.SetScriptValue<String>(offset, TryDeserailize(propertyNode, "Value", String{}));
            else if (type == "EK_VEC2")
                scriptComponent.SetScriptValue<glm::vec2>(offset, TryDeserailize(propertyNode, "Value", glm::vec2{}));
            else if (type == "EK_VEC3")
                scriptComponent.SetScriptValue<glm::vec3>(offset, TryDeserailize(propertyNode, "Value", glm::vec3{}));
            else if (type == "EK_VEC4")
                scriptComponent.SetScriptValue<glm::vec4>(offset, TryDeserailize(propertyNode, "Value", glm::vec4{}));
            else
                EK_CORE_WARN("Unknown type '{0}' for script property '{1}' while deserializing script properties on entity {2}", type, name, entity.GetUUID());

            EK_ASSERT(scriptClassMap.find(scriptComponent.scriptName) != scriptClassMap.end(), "Script '{}' not found", scriptComponent.scriptName);
            auto& scriptClassInfo = scriptClassMap.at(scriptComponent.scriptName);
            scriptClassInfo.reflection.members[name] = { name, type, offset };

            EK_CORE_TRACE("Deserialized script property '{0}' of entity {1}", name, entity.GetUUID());
        }

        EK_CORE_TRACE("Deserialized script properties of entity {0}", entity.GetUUID());
        return true;
    }

    void Scene::Dispose()
    {
    }
}