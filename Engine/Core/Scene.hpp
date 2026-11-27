#pragma once

#include "GameObject.hpp"

#include <memory>
#include <string>
#include <vector>
#include <cstddef>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

struct PointLightData
{
    glm::vec3 Position;
    glm::vec3 Color;
};

class Scene
{
public:
    GameObject& AddGameObject(std::string name);

    const std::vector<std::unique_ptr<GameObject>>& GetGameObjects() const;

    std::size_t GetGameObjectCount() const;

    void Clear();

    void AddPointLight(PointLightData light);
    const std::vector<PointLightData>& GetPointLights() const;

    bool SetParent(GameObject& child, GameObject* parent);

    glm::mat4 GetWorldMatrix(const GameObject& object) const;

private:
    bool IsOwned(const GameObject* object) const;

    glm::mat4 ComputeWorldMatrix(const GameObject* object) const;

    std::vector<std::unique_ptr<GameObject>> m_GameObjects;
    std::vector<PointLightData> m_PointLights;
};
