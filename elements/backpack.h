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

    struct BackpackVertex {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
    };

    struct BackpackTexture {
        GLuint id;
        std::string type;
        std::string path;
    };

    class Backpack : public Element {
    public:
        Backpack(const std::string& name = "Backpack");
        virtual ~Backpack();

        void initialize() override;
        void update(double delta_time) override;
        void render() override;

    private:
        void loadModel(const std::string& path);
        void processNode(aiNode *node, const aiScene *scene);
        void processMesh(aiMesh *mesh, const aiScene *scene);
        std::vector<BackpackTexture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
        unsigned int TextureFromFile(const char *path, const std::string &directory);

        struct Mesh {
            std::vector<BackpackVertex> vertices;
            std::vector<unsigned int> indices;
            std::vector<BackpackTexture> textures;
            GLuint VAO, VBO, EBO;
            
            void setupMesh();
            void Draw(GLuint shaderProgram);
            void cleanUp();
        };

        std::vector<Mesh> meshes;
        std::vector<BackpackTexture> textures_loaded;
        std::string directory;

        GLuint m_shaderProgram {0};
        GLuint m_model_loc {0};
        GLuint m_view_loc {0};
        GLuint m_projection_loc {0};
    };
}
