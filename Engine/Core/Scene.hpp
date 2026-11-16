#pragma once

#include "GameObject.hpp"

#include <string>
#include <vector>
#include <cstddef>

class Scene
{
public:
    GameObject& AddGameObject(std::string name);

    std::vector<GameObject>& GetGameObjects();
    const std::vector<GameObject>& GetGameObjects() const;

    std::size_t GetGameObjectCount() const;

    void Clear();

private:
    std::vector<GameObject> m_GameObjects;
};
