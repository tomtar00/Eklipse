#pragma once
#include <Eklipse/Renderer/Vertex.h>
#include <Eklipse/Renderer/Texture.h>

namespace Eklipse
{
	class Component
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
	};
}