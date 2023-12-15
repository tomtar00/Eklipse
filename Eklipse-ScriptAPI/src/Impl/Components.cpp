#include "precompiled.h"
#include <ScriptAPI/Components.h>
#include <Eklipse/Scene/Components.h>

namespace EklipseEngine
{
    class Transform::TransformImpl
    {
    public:
        TransformImpl(Eklipse::TransformComponent& comp) : _comp(&comp) {}

        glm::vec3& GetPosition()                { return _comp->transform.position; }
        glm::vec3& GetRotation()                { return _comp->transform.rotation; }
        glm::vec3& GetScale()                   { return _comp->transform.scale; }

        void SetPosition(glm::vec3& position)   { _comp->transform.position = position; }
        void SetRotation(glm::vec3& rotation)   { _comp->transform.rotation = rotation; }
        void SetScale(glm::vec3& scale)         { _comp->transform.scale = scale; }

        void Translate(glm::vec3& translation)  { _comp->transform.position += translation; }
        void Rotate(glm::vec3& rotation)        { _comp->transform.rotation += rotation; }
        void Scale(glm::vec3& scale)            { _comp->transform.scale += scale; }

    private:
        Eklipse::TransformComponent* _comp;
    };
    Transform::Transform(Ref<TransformImpl> impl) : _impl(impl) {}
    glm::vec3& Transform::GetPosition()                 { return _impl->GetPosition(); }
    glm::vec3& Transform::GetRotation()                 { return _impl->GetRotation(); }
    glm::vec3& Transform::GetScale()                    { return _impl->GetScale(); }
    void Transform::SetPosition(glm::vec3& position)    { _impl->SetPosition(position); }
    void Transform::SetRotation(glm::vec3& rotation)    { _impl->SetRotation(rotation); }
    void Transform::SetScale(glm::vec3& scale)          { _impl->SetScale(scale); }
    void Transform::Translate(glm::vec3& translation)   { _impl->Translate(translation); }
    void Transform::Rotate(glm::vec3& rotation)         { _impl->Rotate(rotation); }
    void Transform::Scale(glm::vec3& scale)             { _impl->Scale(scale); }
}