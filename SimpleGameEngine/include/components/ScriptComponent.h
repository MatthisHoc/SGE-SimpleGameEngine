#pragma once
#include "Component.h"
#include <stdexcept>

namespace sg
{
    class Object;

    class ScriptException : public std::exception
    {
    public:
        ScriptException(const char* message);
        virtual const char* what() const override;
    private:
        std::string m_message;
    };

    class ScriptComponent : public Component
    {
    public:
        ScriptComponent() {};
        virtual ~ScriptComponent() {};
        virtual void begin() {}
        virtual void update(float deltaSeconds) {}
        bool canUpdate = true;
    };
}

