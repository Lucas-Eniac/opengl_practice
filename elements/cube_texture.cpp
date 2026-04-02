#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "utils/file_utils.h"
#include "cube_texture.h"

namespace Elements {

    static const float vertices[] = {
        // 前面                 // 纹理坐标
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 1.0f,
        // 后面
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,
        // 顶部
        -0.5f,  0.5f, -0.5f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 1.0f,
        // 底部
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
         0.5f, -0.5f,  0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 1.0f,
        // 右侧面
         0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
         0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
         0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
        // 左侧面
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 1.0f
    };

    static const unsigned int indices[] = {
        // 前面
        0, 1, 2,
        2, 3, 0,
        // 后面
        4, 5, 6,
        6, 7, 4,
        // 顶部
        8, 9, 10,
        10, 11, 8,
        // 底部
        12, 13, 14,
        14, 15, 12,
        // 右侧面
        16, 17, 18,
        18, 19, 16,
        // 左侧面
        20, 21, 22,
        22, 23, 20
    };

    CubeTexture::CubeTexture(const std::string& name) : Element(name) {

    }

    CubeTexture::~CubeTexture()
    {
        // Clean up OpenGL resources
        std::cout << "Cleaning up cube resources... VBO: " << m_VBO << ", VAO: " << m_VAO << std::endl;
        if (m_VBO) {
            glDeleteBuffers(1, &m_VBO);
        }
        if (m_VAO) {
            glDeleteVertexArrays(1, &m_VAO);
        }
        if (m_EBO) {
            glDeleteBuffers(1, &m_EBO);
        }
        if (m_texture) {
            glDeleteTextures(1, &m_texture);
        }
    }

    void CubeTexture::initialize() {
        // Initialize cube data (e.g., vertex buffers, shaders)
        int width, height, nrChannels;
        glCreateTextures(GL_TEXTURE_2D, 1, &m_texture);
        glTextureParameterf(m_texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameterf(m_texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameterf(m_texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameterf(m_texture, GL_TEXTURE_WRAP_T, GL_REPEAT);
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load("res/textures/face.png", &width, &height, &nrChannels, 0);
        if (data) {
            GLenum format = GL_RGB;
            GLenum internalFormat = GL_RGB8;
            if (nrChannels == 4) {
                format = GL_RGBA;
                internalFormat = GL_RGBA8;
            }
            glTextureStorage2D(m_texture, 1, internalFormat, width, height);
            glTextureSubImage2D(m_texture, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, data);
        } else {
            std::cerr << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);

        glCreateBuffers(1, &m_VBO);
        glNamedBufferData(m_VBO, sizeof(vertices), vertices, GL_STATIC_DRAW);
        
        glCreateBuffers(1, &m_EBO);
        glNamedBufferData(m_EBO, sizeof(indices), indices, GL_STATIC_DRAW);

        glCreateVertexArrays(1, &m_VAO);
        glVertexArrayVertexBuffer(m_VAO, 0, m_VBO, 0, sizeof(float) * 5);
        glVertexArrayElementBuffer(m_VAO, m_EBO);
        glVertexArrayAttribFormat(m_VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribFormat(m_VAO, 2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 3);
        glVertexArrayAttribBinding(m_VAO, 0, 0);
        glVertexArrayAttribBinding(m_VAO, 2, 0);
        glEnableVertexArrayAttrib(m_VAO, 0);
        glEnableVertexArrayAttrib(m_VAO, 2);

        // Load and compile shaders
        // (Shader loading and compilation code would go here, using FileUtils to read shader files)
        std::string vertexShaderSource = FileUtils::readGLSLFile("glsl/cube_texture/cube_texture.vs");
        std::string fragmentShaderSource = FileUtils::readGLSLFile("glsl/cube_texture/cube_texture.fs");

        // (Shader compilation and program linking code would go here)
        m_shaderProgram = glCreateProgram();
        // Compile vertex shader
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER); 
        const char* vertexSourceCStr = vertexShaderSource.c_str();
        glShaderSource(vertexShader, 1, &vertexSourceCStr, nullptr);
        glCompileShader(vertexShader);
        // Check for compilation errors
        GLint success;
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
            std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }


        glAttachShader(m_shaderProgram, vertexShader);
        // Compile fragment shader
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        const char* fragmentSourceCStr = fragmentShaderSource.c_str();
        glShaderSource(fragmentShader, 1, &fragmentSourceCStr, nullptr);
        glCompileShader(fragmentShader);
        // Check for compilation errors
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
            std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
        glAttachShader(m_shaderProgram, fragmentShader);
        // Link shader program
        glLinkProgram(m_shaderProgram);
        // Check for linking errors
        glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetProgramInfoLog(m_shaderProgram, 512, nullptr, infoLog);
            std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }
        // Clean up shaders as they're linked into the program now and no longer necessary
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        m_model_loc = glGetUniformLocation(m_shaderProgram, "model");
        m_view_loc = glGetUniformLocation(m_shaderProgram, "view");
        m_projection_loc = glGetUniformLocation(m_shaderProgram, "projection");
        m_texture_loc = glGetUniformLocation(m_shaderProgram, "ourTexture");
        
        // 绑定纹理到着色器的纹理单元 0
        glProgramUniform1i(m_shaderProgram, m_texture_loc, 0); 
        // 将 m_texture 绑定到现代 OpenGL 的纹理槽 0
        glBindTextureUnit(0, m_texture);
    }

    void CubeTexture::update(double delta_time) {
        // Update triangle state if necessary
        glm::mat4 model = glm::rotate(glm::mat4(1.0f), (float)delta_time * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)800 / (float)600, 0.1f, 100.0f);

        glUseProgram(m_shaderProgram);
        glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(m_view_loc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(m_projection_loc, 1, GL_FALSE, glm::value_ptr(projection));
    }

    void CubeTexture::render() {
        // Render the triangle using OpenGL commands
        glUseProgram(m_shaderProgram);
        glBindVertexArray(m_VAO);
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}