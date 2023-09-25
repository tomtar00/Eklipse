#pragma once
#include <glm/glm.hpp>

#include "Camera.h"
#include <Eklipse/Renderer/Mesh.h>

namespace Eklipse
{
	struct Transform
	{
		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale;
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

		glm::mat4& GetTransformMatrix(glm::mat4 viewProjMatrix) const;

		TransformComponent() = default;
		TransformComponent(const TransformComponent& transform) = default;
		TransformComponent(const Transform& transform) : transform(transform) {}
	};
	struct MeshComponent
	{
		Mesh mesh;

		MeshComponent() = default;
		MeshComponent(const MeshComponent& mesh) = default;
		MeshComponent(const Mesh& mesh) : mesh(mesh) {}
	};
	struct CameraComponent
	{
		Camera camera;
		bool isActive = true;

		CameraComponent() = default;
		CameraComponent(const CameraComponent& camera) = default;
	};
}