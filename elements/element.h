#pragma once

#include <string>

namespace Elements {
    class Element {
    public:
        Element(const std::string& name) : name(name) {}
        virtual ~Element() = default;

        virtual void initialize() = 0;
        virtual void update() = 0;
        virtual void render() = 0;

        const std::string& getName() const { return name; }

    protected:
        std::string name;
    };
}