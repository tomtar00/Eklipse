#pragma once
#include <glm/glm.hpp>

#include "Transform.h"
#include "Camera.h"
#include <Eklipse/Renderer/Material.h>
#include <Eklipse/Renderer/Mesh.h>

namespace Eklipse
{
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

		glm::mat4& GetTransformMatrix() const;

		TransformComponent() = default;
		TransformComponent(const TransformComponent& transform) = default;
		TransformComponent(const Transform& transform) : transform(transform) {}
	};
	struct MeshComponent
	{
		Mesh* mesh;
		Material* material;

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
}