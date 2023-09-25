#pragma once
#include <glm/glm.hpp>
#include <Eklipse/Renderer/Mesh.h>

namespace Eklipse
{
	/*class Component
	{
	public:
		virtual void Begin() = 0;
		virtual void Update(float deltaTime) = 0;
		virtual void End() = 0;
	};

	class Mesh : public Component
	{
	public:
		Mesh() = default;
		Mesh(const char* modelPath, const char* texturePath);

		virtual void Begin();
		virtual void Update(float deltaTime);
		virtual void End();

		TextureData m_textureData;

		std::vector<float> m_vertices;
		std::vector<uint32_t> m_indices;

	private:
		void Load(const char* modelPath, const char* texturePath);
	};*/

#define EK_COMPONENT_CONSTRUCTS(name, type, varName)	name##Component() = default;\
														name##Component(const name##Component& varName) = default;\
														name##Component(const type& varName) : varName(varName) {}\

#define EK_CAST_OPERATOR(type, varName)		operator type& () { return varName; }\
											operator const type& () const { return varName; }\

	struct NameComponent
	{
		std::string name;

		/*NameComponent() = default;
		NameComponent(const NameComponent& name) = default;
		NameComponent(const std::string& name) : name(name) {}*/

		EK_COMPONENT_CONSTRUCTS(Name, std::string, name);
		EK_CAST_OPERATOR(std::string, name);
	};
	struct TransformComponent
	{
		glm::mat4 transform;

		/*TransformComponent() = default;
		TransformComponent(const TransformComponent& transform) = default;
		TransformComponent(const glm::mat4& transform) : transform(transform) {}*/
		EK_COMPONENT_CONSTRUCTS(Transform, glm::mat4, transform);

		/*operator glm::mat4& () { return transform; }
		operator const glm::mat4& () const { return transform; }*/
		EK_CAST_OPERATOR(glm::mat4, transform);
	};
	struct MeshComponent
	{
		Mesh mesh;

		/*MeshComponent() = default;
		MeshComponent(const MeshComponent& mesh) = default;
		MeshComponent(const Mesh& mesh) : mesh(mesh) {}*/

		EK_COMPONENT_CONSTRUCTS(Mesh, Mesh, mesh);
		EK_CAST_OPERATOR(Mesh, mesh);
	};
	struct CameraComponent
	{
		Camera camera;

		EK_COMPONENT_CONSTRUCTS(Camera, Camera, camera);
		EK_CAST_OPERATOR(Camera, camera);
	};
}