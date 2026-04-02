#pragma once

#include "glad/glad.h"

#include "element.h"

namespace Elements {
    class Cube : public Element {
    public:
        Cube(const std::string& name = "Cube");
        virtual ~Cube();

        void initialize() override;
        void update(double delta_time) override;
        void render() override;

    private:
        GLuint m_VAO {0};
        GLuint m_VBO {0};
        GLuint m_EBO {0};
        GLuint m_model_loc {0};
        GLuint m_view_loc {0};
        GLuint m_projection_loc {0};
        GLuint m_shaderProgram {0};
    };
}