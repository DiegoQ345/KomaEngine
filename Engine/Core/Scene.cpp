#include "Scene.hpp"

#include <utility>

GameObject& Scene::AddGameObject(std::string name)
{
    m_GameObjects.emplace_back(std::move(name));
    return m_GameObjects.back();
}

std::vector<GameObject>& Scene::GetGameObjects()
{
    return m_GameObjects;
}

const std::vector<GameObject>& Scene::GetGameObjects() const
{
    return m_GameObjects;
}

std::size_t Scene::GetGameObjectCount() const
{
    return m_GameObjects.size();
}

void Scene::Clear()
{
    m_GameObjects.clear();
}
