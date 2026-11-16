#pragma once

#include "Transform.hpp"

#include <string>

class GameObject
{
public:
    explicit GameObject(std::string name);

    const std::string& GetName() const;

    Transform& GetTransform();
    const Transform& GetTransform() const;

private:
    std::string m_Name;
    Transform m_Transform;
};
