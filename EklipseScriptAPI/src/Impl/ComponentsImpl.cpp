#include "precompiled.h"
#include "ComponentsImpl.h"
#include <ScriptAPI/Components.h>

namespace EklipseEngine
{
    // Transform
    Transform::TransformImpl::TransformImpl(Eklipse::TransformComponent& comp) : _comp(&comp) {}
    glm::vec3& Transform::TransformImpl::GetPosition()                  { return _comp->transform.position; }
    glm::vec3& Transform::TransformImpl::GetRotation()                  { return _comp->transform.rotation; }
    glm::vec3& Transform::TransformImpl::GetScale()                     { return _comp->transform.scale;    }
    glm::vec3 Transform::TransformImpl::GetForward()                    { return _comp->GetForward();       }
    glm::vec3 Transform::TransformImpl::GetRight()                      { return _comp->GetRight();         }
    glm::vec3 Transform::TransformImpl::GetUp()                         { return _comp->GetUp();            }
    void Transform::TransformImpl::SetPosition(glm::vec3& position)   { _comp->transform.position = position; }
    void Transform::TransformImpl::SetRotation(glm::vec3& rotation)   { _comp->transform.rotation = rotation; }
    void Transform::TransformImpl::SetScale(glm::vec3& scale)         { _comp->transform.scale = scale; }
    void Transform::TransformImpl::Translate(glm::vec3& translation)  { _comp->transform.position += translation; }
    void Transform::TransformImpl::Rotate(glm::vec3& rotation)        { _comp->transform.rotation += rotation; }
    void Transform::TransformImpl::Scale(glm::vec3& scale)            { _comp->transform.scale += scale; }

    Transform::Transform(Ref<TransformImpl> impl) : m_impl(impl) {}
    glm::vec3& Transform::GetPosition()                 { return m_impl->GetPosition(); }
    glm::vec3& Transform::GetRotation()                 { return m_impl->GetRotation(); }
    glm::vec3& Transform::GetScale()                    { return m_impl->GetScale();    }
    glm::vec3 Transform::GetForward()                   { return m_impl->GetForward();  }
    glm::vec3 Transform::GetRight()                     { return m_impl->GetRight();    }
    glm::vec3 Transform::GetUp()                        { return m_impl->GetUp();       }
    void Transform::SetPosition(glm::vec3& position)    { m_impl->SetPosition(position);    }
    void Transform::SetRotation(glm::vec3& rotation)    { m_impl->SetRotation(rotation);    }
    void Transform::SetScale(glm::vec3& scale)          { m_impl->SetScale(scale);          }
    void Transform::Translate(glm::vec3& translation)   { m_impl->Translate(translation);   }
    void Transform::Rotate(glm::vec3& rotation)         { m_impl->Rotate(rotation);         }
    void Transform::Scale(glm::vec3& scale)             { m_impl->Scale(scale);             }

    // ...
}