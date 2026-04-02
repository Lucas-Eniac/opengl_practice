#include "green_sea_turtle.h"
#include "../utils/file_utils.h"

#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stb/stb_image.h>

namespace Elements {

    GreenSeaTurtle::GreenSeaTurtle(const std::string& name) : Element(name) {}

    GreenSeaTurtle::~GreenSeaTurtle() {
        for (auto& mesh : m_meshes) {
            mesh.cleanUp();
        }
        if (m_shaderProgram) glDeleteProgram(m_shaderProgram);
    }

    void GreenSeaTurtle::initialize() {
        std::string path = "res/models/Green_Sea_Turtle.fbx";
        
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cerr << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
            return;
        }

        processNode(scene->mRootNode, scene);

        // Shaders
        std::string vertexShaderSource = FileUtils::readGLSLFile("glsl/sea_turtle/sea_turtle.vs");
        std::string fragmentShaderSource = FileUtils::readGLSLFile("glsl/sea_turtle/sea_turtle.fs");

        m_shaderProgram = glCreateProgram();
        // Compile vertex shader
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER); 
        const char* vertexSourceCStr = vertexShaderSource.c_str();
        glShaderSource(vertexShader, 1, &vertexSourceCStr, nullptr);
        glCompileShader(vertexShader);
        
        glAttachShader(m_shaderProgram, vertexShader);
        // Compile fragment shader
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        const char* fragmentSourceCStr = fragmentShaderSource.c_str();
        glShaderSource(fragmentShader, 1, &fragmentSourceCStr, nullptr);
        glCompileShader(fragmentShader);
        glAttachShader(m_shaderProgram, fragmentShader);
        
        glLinkProgram(m_shaderProgram);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        m_model_loc = glGetUniformLocation(m_shaderProgram, "model");
        m_view_loc = glGetUniformLocation(m_shaderProgram, "view");
        m_projection_loc = glGetUniformLocation(m_shaderProgram, "projection");
    }

    void GreenSeaTurtle::processNode(aiNode *node, const aiScene *scene) {
        for(unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            processMesh(mesh, scene);
        }
        for(unsigned int i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene);
        }
    }

    void GreenSeaTurtle::processMesh(aiMesh *mesh, const aiScene *scene) {
        Mesh finalMesh;
        for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex;
            glm::vec3 vector;
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;
            
            if (mesh->HasNormals()) {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.Normal = vector;
            }
            
            if(mesh->mTextureCoords[0]) {
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
            } else {
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            }
            finalMesh.vertices.push_back(vertex);
        }
        
        for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for(unsigned int j = 0; j < face.mNumIndices; j++)
                finalMesh.indices.push_back(face.mIndices[j]);
        }
        
        finalMesh.setupMesh();
        m_meshes.push_back(finalMesh);
    }

    void GreenSeaTurtle::update(double delta_time) {
        glm::mat4 model = glm::rotate(glm::mat4(1.0f), (float)delta_time * glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        // Scale it down usually FBX models are huge
        model = glm::scale(model, glm::vec3(0.04f)); 
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)800 / (float)600, 0.1f, 100.0f);

        glUseProgram(m_shaderProgram);
        glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(m_view_loc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(m_projection_loc, 1, GL_FALSE, glm::value_ptr(projection));
    }

    void GreenSeaTurtle::render() {
        glUseProgram(m_shaderProgram);
        for(unsigned int i = 0; i < m_meshes.size(); i++)
            m_meshes[i].Draw(m_shaderProgram);
    }

    // --- Mesh Methods ---
    void GreenSeaTurtle::Mesh::setupMesh() {
        glCreateBuffers(1, &VBO);
        glNamedBufferData(VBO, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
        
        glCreateBuffers(1, &EBO);
        glNamedBufferData(EBO, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        
        glCreateVertexArrays(1, &VAO);
        glVertexArrayVertexBuffer(VAO, 0, VBO, 0, sizeof(Vertex));
        glVertexArrayElementBuffer(VAO, EBO);
        
        // Position
        glEnableVertexArrayAttrib(VAO, 0);
        glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, Position));
        glVertexArrayAttribBinding(VAO, 0, 0);
        
        // Normal
        glEnableVertexArrayAttrib(VAO, 1);
        glVertexArrayAttribFormat(VAO, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, Normal));
        glVertexArrayAttribBinding(VAO, 1, 0);
        
        // TexCoords
        glEnableVertexArrayAttrib(VAO, 2);
        glVertexArrayAttribFormat(VAO, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, TexCoords));
        glVertexArrayAttribBinding(VAO, 2, 0);
    }
    
    void GreenSeaTurtle::Mesh::Draw(GLuint shaderProgram) {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void GreenSeaTurtle::Mesh::cleanUp() {
        if(VBO) glDeleteBuffers(1, &VBO);
        if(EBO) glDeleteBuffers(1, &EBO);
        if(VAO) glDeleteVertexArrays(1, &VAO);
    }
}
