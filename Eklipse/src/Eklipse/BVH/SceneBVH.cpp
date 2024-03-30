#include "precompiled.h"
#include "SceneBVH.h"

#include <Eklipse/Scene/Components.h>

#define MAX_BVH_NUM_TRI 10
#define MAX_BVH_DEPTH 10

namespace Eklipse
{
    void SceneBVH::Build()
    {
        EK_CORE_PROFILE();
        EK_CORE_TRACE("Building BVH...");

        AABB sceneAABB(glm::vec3(FLT_MAX), glm::vec3(-FLT_MAX));
        std::vector<Triangle> allTriangles;
        m_scene->GetRegistry().view<RayTracingMeshComponent>().each([&](auto entityID, RayTracingMeshComponent& rtMesh) {
            Entity entity = { entityID, m_scene };
            auto mesh = entity.GetComponent<MeshComponent>().mesh;
            auto& transformMat = entity.GetComponent<TransformComponent>().GetTransformMatrix();
            auto& triangles = mesh->GetTriangles();
            for (auto& triangle : triangles) {
                triangle.a = glm::vec3(transformMat * glm::vec4(triangle.a, 1.0f));
                triangle.b = glm::vec3(transformMat * glm::vec4(triangle.b, 1.0f));
                triangle.c = glm::vec3(transformMat * glm::vec4(triangle.c, 1.0f));
            }
            allTriangles.insert(allTriangles.end(), triangles.begin(), triangles.end());
            for (const auto& triangle : triangles) {
                sceneAABB.Expand(triangle.a);
                sceneAABB.Expand(triangle.b);
                sceneAABB.Expand(triangle.c);
            }
        });

        m_nodes.clear();
        BuildRecursive(allTriangles, 0, sceneAABB, 0);

        EK_CORE_DBG("BVH built with {0} nodes", m_nodes.size());
    }
    Vec<SceneBVH::Node>& SceneBVH::GetNodes()
    {
        return m_nodes;
    }

    void SceneBVH::BuildRecursive(const Vec<Triangle>& triangles, uint32_t nodeIndex, const AABB& parentAABB, uint32_t depth)
    {
        EK_CORE_PROFILE();

        if (triangles.size() <= MAX_BVH_NUM_TRI || depth >= MAX_BVH_DEPTH) 
        {
            m_nodes.push_back(Node{ parentAABB.GetMin(), parentAABB.GetMax() });
            return;
        }

        int axis = FindSplitAxis(triangles, parentAABB);
        float splitPos = FindSplitPosition(triangles, axis, parentAABB);

        std::vector<Triangle> leftTriangles, rightTriangles;
        AABB leftAABB(glm::vec3(FLT_MAX), glm::vec3(-FLT_MAX));
        AABB rightAABB(glm::vec3(FLT_MAX), glm::vec3(-FLT_MAX));
        for (const auto& triangle : triangles) 
        {
            AABB triangleAABB(glm::vec3(FLT_MAX), glm::vec3(-FLT_MAX));
            triangleAABB.Expand(triangle.a);
            triangleAABB.Expand(triangle.b);
            triangleAABB.Expand(triangle.c);
            if (triangleAABB.GetMax()[axis] < splitPos) 
            {
                leftTriangles.push_back(triangle);
                leftAABB.Expand(triangle.a);
                leftAABB.Expand(triangle.b);
                leftAABB.Expand(triangle.c);
            }
            else 
            {
                rightTriangles.push_back(triangle);
                rightAABB.Expand(triangle.a);
                rightAABB.Expand(triangle.b);
                rightAABB.Expand(triangle.c);
            }
        }

        m_nodes.push_back(Node{ parentAABB.GetMin(), parentAABB.GetMax() });
        BuildRecursive(leftTriangles, nodeIndex * 2 + 1, leftAABB, depth + 1);
        BuildRecursive(rightTriangles, nodeIndex * 2 + 2, rightAABB, depth + 1);
    }

    int SceneBVH::FindSplitAxis(const Vec<Triangle>& triangles, const AABB& aabb)
    {
        EK_CORE_PROFILE();

        return aabb.MaxDim();
    }

    float SceneBVH::FindSplitPosition(const Vec<Triangle>& triangles, int axis, const AABB& aabb) 
    {
        EK_CORE_PROFILE();

        std::vector<float> centerPoints;
        for (const auto& triangle : triangles) {
            centerPoints.push_back((triangle.a[axis] + triangle.b[axis] + triangle.c[axis]) / 3.0f);
        }
        std::sort(centerPoints.begin(), centerPoints.end());
        return centerPoints[centerPoints.size() / 2];
    }
}
