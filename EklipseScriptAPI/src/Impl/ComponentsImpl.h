#pragma once
#include <ScriptAPI/Components.h>
#include <Eklipse/Scene/Components.h>

namespace EklipseEngine
{
    class Transform::TransformImpl
    {
    public:
        using Comp = typename Eklipse::TransformComponent;
        TransformImpl(Eklipse::TransformComponent& comp);

        glm::vec3& Transform::TransformImpl::GetPosition();
        glm::vec3& Transform::TransformImpl::GetRotation();
        glm::vec3& Transform::TransformImpl::GetScale();

        void SetPosition(glm::vec3& position);
        void SetRotation(glm::vec3& rotation);
        void SetScale(glm::vec3& scale);

        void Translate(glm::vec3& translation);
        void Rotate(glm::vec3& rotation);
        void Scale(glm::vec3& scale);

    private:
        Eklipse::TransformComponent* _comp;
    };
}