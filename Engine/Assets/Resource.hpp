#pragma once

#include <string>
#include <string_view>
#include <utility>

class Resource
{
public:
    virtual ~Resource() = default;

    virtual std::string_view GetType() const = 0;

    virtual const std::string& GetId() const { return m_Id; }

protected:
    explicit Resource(std::string id)
        : m_Id(std::move(id))
    {
    }

private:
    Resource(const Resource&) = delete;
    Resource& operator=(const Resource&) = delete;

    std::string m_Id;
};
