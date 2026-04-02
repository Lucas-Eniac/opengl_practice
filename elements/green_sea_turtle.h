#pragma once

#include "glad/glad.h"
#include "element.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <vector>

namespace Elements {

    struct Vertex {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
    };

    struct Texture {
        GLuint id;
        std::string type;
        std::string path;
    };

    class GreenSeaTurtle : public Element {
    public:
        GreenSeaTurtle(const std::string& name = "GreenSeaTurtle");
        virtual ~GreenSeaTurtle();

        void initialize() override;
        void update(double delta_time) override;
        void render() override;

    private:
        void processNode(aiNode *node, const aiScene *scene);
        void processMesh(aiMesh *mesh, const aiScene *scene);
        
        struct Mesh {
            std::vector<Vertex> vertices;
            std::vector<unsigned int> indices;
            std::vector<Texture> textures;
            GLuint VAO, VBO, EBO;
            
            void setupMesh();
            void Draw(GLuint shaderProgram);
            void cleanUp();
        };

        std::vector<Mesh> m_meshes;
        std::vector<Texture> m_textures_loaded; // Cache to avoid reloading the same textures

        GLuint m_shaderProgram {0};
        GLuint m_model_loc {0};
        GLuint m_view_loc {0};
        GLuint m_projection_loc {0};
    };
}
