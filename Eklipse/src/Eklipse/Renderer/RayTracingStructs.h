#pragma once
#include <glm/glm.hpp>

namespace Eklipse
{
	struct RayTracingMaterial
	{
		glm::vec3 albedo;
		float smoothness;
		glm::vec3 specularColor;
		float specularProb;
		glm::vec3 emissionColor;
		float emissionStrength;
	};
	struct RayTracingSphereInfo
	{
		glm::vec3 position;
		float radius;
		uint32_t materialIndex;		float __padding0[3];
	};
	struct RayTracingMeshInfo
	{
		uint32_t vertexOffset;
		uint32_t vertexCount;
		uint32_t indexOffset;
		uint32_t indexCount;
		glm::vec3 boundMin;			float __padding0[1];
		glm::vec3 boundMax;
		uint32_t materialIndex;
	};
}