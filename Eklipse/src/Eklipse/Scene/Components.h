#pragma once
#include "Transform.h"
#include "Camera.h"

#include <glm/glm.hpp>
#include <Eklipse/Renderer/Material.h>
#include <Eklipse/Renderer/Mesh.h>
#include <Eklipse/Core/UUID.h>

#include <EklipseScriptAPI.h>

namespace Eklipse
{
	struct EK_API IDComponent
	{
		UUID ID;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
	};
	struct EK_API NameComponent
	{
		String name;

		NameComponent() = default;
		NameComponent(const NameComponent& name) = default;
		NameComponent(const String& name) : name(name) {}
	};
	struct EK_API TransformComponent
	{
		Transform transform;
		glm::mat4 transformMatrix;

		glm::mat4 GetTransformMatrix() const;
		glm::vec3 GetForward() const;
		glm::vec3 GetRight() const;
		glm::vec3 GetUp() const;

		TransformComponent() = default;
		TransformComponent(const TransformComponent& transform) = default;
		TransformComponent(const Transform& transform) : transform(transform), transformMatrix(glm::mat4(1.0f)) {}
	};
	struct EK_API MeshComponent
	{
		Mesh* mesh;
		Material* material;

		AssetHandle meshHandle;
		AssetHandle materialHandle;

		MeshComponent() = default;
		MeshComponent(const MeshComponent& mesh) = default;
		MeshComponent(Mesh* mesh, Material* material) : mesh(mesh), material(material) {}
	};
	struct EK_API CameraComponent
	{
		Camera camera;

		CameraComponent() = default;
		CameraComponent(const CameraComponent& camera) = default;
	};
	struct EK_API RayTracingMeshComponent
	{
		uint32_t index;
		RayTracingMaterial material;

		RayTracingMeshComponent() = default;
		RayTracingMeshComponent(const RayTracingMeshComponent& comp) = default;
	};
	struct EK_API RayTracingSphereComponent
	{
		float radius;
		RayTracingMaterial material;

		RayTracingSphereComponent() = default;
		RayTracingSphereComponent(const RayTracingSphereComponent& comp) = default;
	};

	struct EK_API ScriptComponent
	{
		EklipseEngine::Script* script = nullptr;
		String scriptName;

		ScriptComponent() = default;
		ScriptComponent(const ScriptComponent& script) = default;

		void SetScript(const String& name, const EklipseEngine::Reflections::ClassInfo& info, Entity entity);
		void DestroyScript();

		template<typename T>
		T* GetScriptValue(size_t offset) {
			return (T*)((char*)script + offset);
		}
		template<typename T>
		void SetScriptValue(size_t offset, T value) {
			*(T*)((char*)script + offset) = value;
		}
	};

	template<typename... Component>
	struct ComponentGroup {};

	using AllComponents =
		ComponentGroup<TransformComponent, MeshComponent, CameraComponent, RayTracingMeshComponent, RayTracingSphereComponent>;
}