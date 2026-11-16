#include "Scene.hpp"

#include "Core/Logger.hpp"
#include "Transform.hpp"

#include <algorithm>
#include <stdexcept>
#include <string>
#include <utility>

GameObject& Scene::AddGameObject(std::string name)
{
    m_GameObjects.emplace_back(std::make_unique<GameObject>(std::move(name)));
    return *m_GameObjects.back();
}

const std::vector<std::unique_ptr<GameObject>>& Scene::GetGameObjects() const
{
    return m_GameObjects;
}

std::size_t Scene::GetGameObjectCount() const
{
    return m_GameObjects.size();
}

void Scene::Clear()
{
    for (const auto& object : m_GameObjects)
    {
        object->m_Parent = nullptr;
        object->m_Children.clear();
    }

    m_GameObjects.clear();
}

bool Scene::SetParent(GameObject& child, GameObject* parent)
{
    if (parent == &child) return false;

    if (!IsOwned(&child)) return false;
    if (parent != nullptr && !IsOwned(parent)) return false;

    for (const GameObject* walker = parent; walker != nullptr; walker = walker->m_Parent)
    {
        if (walker == &child) return false;
    }

    GameObject* const oldParent = child.m_Parent;

    if (oldParent)
    {
        auto& children = oldParent->m_Children;
        const auto it = std::find(children.begin(), children.end(), &child);
        if (it != children.end()) children.erase(it);
    }

    child.m_Parent = parent;
    if (parent)
    {
        parent->m_Children.push_back(&child);
    }

    return true;
}

glm::mat4 Scene::GetWorldMatrix(const GameObject& object) const
{
    if (!IsOwned(&object))
    {
        Logger::Error("Scene::GetWorldMatrix received an object that does not belong to this scene");
        throw std::runtime_error("Scene::GetWorldMatrix received an object that does not belong to this scene");
    }

    return ComputeWorldMatrix(&object);
}

bool Scene::IsOwned(const GameObject* object) const
{
    return std::any_of(m_GameObjects.begin(), m_GameObjects.end(),
        [object](const std::unique_ptr<GameObject>& owned) { return owned.get() == object; });
}

glm::mat4 Scene::ComputeWorldMatrix(const GameObject* object) const
{
    const glm::mat4 local = object->GetTransform().GetModelMatrix();

    if (object->m_Parent == nullptr)
    {
        return local;
    }

    return ComputeWorldMatrix(object->m_Parent) * local;
}
