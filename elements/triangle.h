#pragma once

#include "glad/glad.h"

#include "element.h"

namespace Elements {
    class Triangle : public Element {
    public:
        Triangle(const std::string& name = "Triangle");
        virtual ~Triangle();

        void initialize() override;
        void update(double delta_time) override;
        void render() override;

    private:
        GLuint m_VAO {0};
        GLuint m_VBO {0};
        GLuint m_shaderProgram {0};
    };
}