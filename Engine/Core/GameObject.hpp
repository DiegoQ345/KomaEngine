#pragma once

#include "Transform.hpp"

#include <string>
#include <vector>

class Scene;

class GameObject
{
public:
    explicit GameObject(std::string name);

    const std::string& GetName() const;

    Transform& GetTransform();
    const Transform& GetTransform() const;

    GameObject(const GameObject&) = delete;
    GameObject& operator=(const GameObject&) = delete;
    GameObject(GameObject&&) = delete;
    GameObject& operator=(GameObject&&) = delete;

private:
    friend class Scene;

    GameObject* m_Parent = nullptr;
    std::vector<GameObject*> m_Children;

    std::string m_Name;
    Transform m_Transform;
};
