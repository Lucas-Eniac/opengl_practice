#pragma once

#include "glad/glad.h"

#include "element.h"

namespace Elements {
    class CubeTexture : public Element {
    public:
        CubeTexture(const std::string& name = "CubeTexture");
        virtual ~CubeTexture();

        void initialize() override;
        void update(double delta_time) override;
        void render() override;

    private:
        GLuint m_VAO {0};
        GLuint m_VBO {0};
        GLuint m_EBO {0};
        GLuint m_texture {0};
        GLuint m_model_loc {0};
        GLuint m_view_loc {0};
        GLuint m_projection_loc {0};
        GLuint m_shaderProgram {0};
        GLuint m_texture_loc {0};
    };
}