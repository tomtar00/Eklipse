#include "precompiled.h"
#include <Decl/Components.h>
#include <ScriptAPI/Components.h>

namespace EklipseEngine
{
    Transform::TransformImpl::TransformImpl(Eklipse::TransformComponent& comp) : _comp(&comp) {}
    glm::vec3& Transform::TransformImpl::GetPosition()                { return _comp->transform.position; }
    glm::vec3& Transform::TransformImpl::GetRotation()                { return _comp->transform.rotation; }
    glm::vec3& Transform::TransformImpl::GetScale()                   { return _comp->transform.scale; }
    void Transform::TransformImpl::SetPosition(glm::vec3& position)   { _comp->transform.position = position; }
    void Transform::TransformImpl::SetRotation(glm::vec3& rotation)   { _comp->transform.rotation = rotation; }
    void Transform::TransformImpl::SetScale(glm::vec3& scale)         { _comp->transform.scale = scale; }
    void Transform::TransformImpl::Translate(glm::vec3& translation)  { _comp->transform.position += translation; }
    void Transform::TransformImpl::Rotate(glm::vec3& rotation)        { _comp->transform.rotation += rotation; }
    void Transform::TransformImpl::Scale(glm::vec3& scale)            { _comp->transform.scale += scale; }

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