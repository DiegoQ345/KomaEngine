#include "GameObject.hpp"

#include <utility>

GameObject::GameObject(std::string name)
    : m_Name(std::move(name))
{
}

const std::string& GameObject::GetName() const
{
    return m_Name;
}

Transform& GameObject::GetTransform()
{
    return m_Transform;
}

const Transform& GameObject::GetTransform() const
{
    return m_Transform;
}
