#pragma once

#include "GameObject.hpp"

#include <memory>
#include <string>
#include <vector>
#include <cstddef>

#include <glm/mat4x4.hpp>

class Scene
{
public:
    GameObject& AddGameObject(std::string name);

    const std::vector<std::unique_ptr<GameObject>>& GetGameObjects() const;

    std::size_t GetGameObjectCount() const;

    void Clear();

    bool SetParent(GameObject& child, GameObject* parent);

    glm::mat4 GetWorldMatrix(const GameObject& object) const;

private:
    bool IsOwned(const GameObject* object) const;

    glm::mat4 ComputeWorldMatrix(const GameObject* object) const;

    std::vector<std::unique_ptr<GameObject>> m_GameObjects;
};
