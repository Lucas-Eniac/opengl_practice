#include <iostream>

#include "utils/file_utils.h"
#include "triangle.h"

namespace Elements {

    static const float vertices[] = {
        // positions         // colors
         0.0f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // top vertex (red)
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom left vertex (green)
         0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f  // bottom right vertex (blue)
    };

    Triangle::Triangle(const std::string& name) : Element(name) {

    }

    Triangle::~Triangle()
    {
        // Clean up OpenGL resources
        std::cout << "Cleaning up triangle resources... VBO: " << m_VBO << ", VAO: " << m_VAO << std::endl;
        if (m_VBO) {
            glDeleteBuffers(1, &m_VBO);
        }
        if (m_VAO) {
            glDeleteVertexArrays(1, &m_VAO);
        }
    }

    void Triangle::initialize() {
        // Initialize triangle data (e.g., vertex buffers, shaders)
        glCreateBuffers(1, &m_VBO);
        glNamedBufferStorage(m_VBO, sizeof(vertices), vertices, GL_DYNAMIC_STORAGE_BIT);
        glCreateVertexArrays(1, &m_VAO);
        glVertexArrayVertexBuffer(m_VAO, 0, m_VBO, 0, sizeof(float) * 6);
        glVertexArrayAttribFormat(m_VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribFormat(m_VAO, 1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3);
        glVertexArrayAttribBinding(m_VAO, 0, 0);
        glVertexArrayAttribBinding(m_VAO, 1, 0);
        glEnableVertexArrayAttrib(m_VAO, 0);
        glEnableVertexArrayAttrib(m_VAO, 1);

        // Load and compile shaders
        // (Shader loading and compilation code would go here, using FileUtils to read shader files)
        std::string vertexShaderSource = FileUtils::readGLSLFile("glsl/triangle/triangle.vs");
        std::string fragmentShaderSource = FileUtils::readGLSLFile("glsl/triangle/trangle.fs");

        // (Shader compilation and program linking code would go here)
        m_shaderProgram = glCreateProgram();
        // Compile vertex shader
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER); 
        const char* vertexSourceCStr = vertexShaderSource.c_str();
        glShaderSource(vertexShader, 1, &vertexSourceCStr, nullptr);
        glCompileShader(vertexShader);
        // Check for compilation errors (omitted for brevity)
        glAttachShader(m_shaderProgram, vertexShader);
        // Compile fragment shader
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        const char* fragmentSourceCStr = fragmentShaderSource.c_str();
        glShaderSource(fragmentShader, 1, &fragmentSourceCStr, nullptr);
        glCompileShader(fragmentShader);
        // Check for compilation errors (omitted for brevity)
        glAttachShader(m_shaderProgram, fragmentShader);
        // Link shader program
        glLinkProgram(m_shaderProgram);
        // Check for linking errors (omitted for brevity)
        // Clean up shaders as they're linked into the program now and no longer necessary
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    void Triangle::update(double delta_time) {
        // Update triangle state if necessary
    }

    void Triangle::render() {
        // Render the triangle using OpenGL commands
        glUseProgram(m_shaderProgram);
        glBindVertexArray(m_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);
    }
}