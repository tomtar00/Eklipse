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
	struct IDComponent
	{
		UUID ID;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
	};
	struct NameComponent
	{
		std::string name;

		NameComponent() = default;
		NameComponent(const NameComponent& name) = default;
		NameComponent(const std::string& name) : name(name) {}
	};
	struct TransformComponent
	{
		Transform transform;
		glm::mat4 transformMatrix;

		glm::mat4 GetTransformMatrix() const;

		TransformComponent() = default;
		TransformComponent(const TransformComponent& transform) = default;
		TransformComponent(const Transform& transform) : transform(transform) {}
	};
	struct MeshComponent
	{
		Mesh* mesh;
		Material* material;

		std::string meshPath;
		std::string materialPath;

		MeshComponent() = default;
		MeshComponent(const MeshComponent& mesh) = default;
		MeshComponent(Mesh* mesh, Material* material) : mesh(mesh), material(material) {}
	};
	struct CameraComponent
	{
		Camera camera;

		CameraComponent() = default;
		CameraComponent(const CameraComponent& camera) = default;
	};

	struct ScriptComponent
	{
		EklipseEngine::Script* script;
		std::string scriptName;
		EklipseEngine::Reflections::ClassInfo classInfo;

		ScriptComponent() = default;
		ScriptComponent(const ScriptComponent& script) = default;

		void SetScript(const std::string& name, const EklipseEngine::Reflections::ClassInfo& info, Entity entity);
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
}