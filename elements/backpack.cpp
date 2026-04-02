#include "backpack.h"
#include "../utils/file_utils.h"

#include <iostream>
#include <stb/stb_image.h>

namespace Elements {

    Backpack::Backpack(const std::string& name) : Element(name) {}

    Backpack::~Backpack() {
        for (auto& mesh : meshes) {
            mesh.cleanUp();
        }
        if (m_shaderProgram) glDeleteProgram(m_shaderProgram);
    }

    void Backpack::initialize() {
        loadModel("res/models/backpack/backpack.obj");

        // Shaders
        std::string vertexShaderSource = FileUtils::readGLSLFile("glsl/backpack/backpack.vs");
        std::string fragmentShaderSource = FileUtils::readGLSLFile("glsl/backpack/backpack.fs");

        m_shaderProgram = glCreateProgram();
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER); 
        const char* vertexSourceCStr = vertexShaderSource.c_str();
        glShaderSource(vertexShader, 1, &vertexSourceCStr, nullptr);
        glCompileShader(vertexShader);
        
        glAttachShader(m_shaderProgram, vertexShader);
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
        glProgramUniform1i(m_shaderProgram, glGetUniformLocation(m_shaderProgram, "material.texture_diffuse1"), 0);
        glProgramUniform1i(m_shaderProgram, glGetUniformLocation(m_shaderProgram, "material.texture_specular1"), 1);
    }

    void Backpack::loadModel(const std::string& path) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cerr << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
            return;
        }

        directory = path.substr(0, path.find_last_of('/'));
        processNode(scene->mRootNode, scene);
    }

    void Backpack::processNode(aiNode *node, const aiScene *scene) {
        for(unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            processMesh(mesh, scene);
        }
        for(unsigned int i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene);
        }
    }

    void Backpack::processMesh(aiMesh *mesh, const aiScene *scene) {
        Mesh finalMesh;
        for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
            BackpackVertex vertex;
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
        
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        
        std::vector<BackpackTexture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        finalMesh.textures.insert(finalMesh.textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        
        std::vector<BackpackTexture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        finalMesh.textures.insert(finalMesh.textures.end(), specularMaps.begin(), specularMaps.end());

        finalMesh.setupMesh();
        meshes.push_back(finalMesh);
    }

    std::vector<BackpackTexture> Backpack::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName) {
        std::vector<BackpackTexture> textures;
        for(unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
            aiString str;
            mat->GetTexture(type, i, &str);
            bool skip = false;
            for(unsigned int j = 0; j < textures_loaded.size(); j++) {
                if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
                    textures.push_back(textures_loaded[j]);
                    skip = true; break;
                }
            }
            if(!skip) {
                BackpackTexture texture;
                texture.id = TextureFromFile(str.C_Str(), this->directory);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture);
            }
        }
        return textures;
    }

    unsigned int Backpack::TextureFromFile(const char *path, const std::string &directory) {
        std::string filename = std::string(path);
        filename = directory + '/' + filename;

        unsigned int textureID;
        glCreateTextures(GL_TEXTURE_2D, 1, &textureID);

        int width, height, nrComponents;
        stbi_set_flip_vertically_on_load(true); // 通常OpenGL需要翻转
        unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
        if (data) {
            GLenum format = GL_RGB;
            GLenum internalFormat = GL_RGB8;
            if (nrComponents == 1) {
                format = GL_RED; internalFormat = GL_R8;
            } else if (nrComponents == 4) {
                format = GL_RGBA; internalFormat = GL_RGBA8;
            }

            glTextureStorage2D(textureID, 1, internalFormat, width, height);
            glTextureSubImage2D(textureID, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, data);
            
            glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glGenerateTextureMipmap(textureID);

            stbi_image_free(data);
        } else {
            std::cerr << "Texture failed to load at path: " << filename << std::endl;
            stbi_image_free(data);
        }
        return textureID;
    }

    void Backpack::update(double delta_time) {
        glm::mat4 model = glm::mat4(1.0f);
        // Slightly rotate model
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); 
        model = glm::scale(model, glm::vec3(1.0f));	// it's a bit too big for our scene, so scale it down
        model = glm::rotate(model, (float)delta_time * glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)800 / (float)600, 0.1f, 100.0f);

        glUseProgram(m_shaderProgram);
        glUniformMatrix4fv(m_model_loc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(m_view_loc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(m_projection_loc, 1, GL_FALSE, glm::value_ptr(projection));
    }

    void Backpack::render() {
        glUseProgram(m_shaderProgram);
        for(unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(m_shaderProgram);
    }

    void Backpack::Mesh::setupMesh() {
        glCreateBuffers(1, &VBO);
        glNamedBufferData(VBO, vertices.size() * sizeof(BackpackVertex), &vertices[0], GL_STATIC_DRAW);
        
        glCreateBuffers(1, &EBO);
        glNamedBufferData(EBO, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        
        glCreateVertexArrays(1, &VAO);
        glVertexArrayVertexBuffer(VAO, 0, VBO, 0, sizeof(BackpackVertex));
        glVertexArrayElementBuffer(VAO, EBO);
        
        glEnableVertexArrayAttrib(VAO, 0);
        glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, offsetof(BackpackVertex, Position));
        glVertexArrayAttribBinding(VAO, 0, 0);
        
        glEnableVertexArrayAttrib(VAO, 1);
        glVertexArrayAttribFormat(VAO, 1, 3, GL_FLOAT, GL_FALSE, offsetof(BackpackVertex, Normal));
        glVertexArrayAttribBinding(VAO, 1, 0);
        
        glEnableVertexArrayAttrib(VAO, 2);
        glVertexArrayAttribFormat(VAO, 2, 2, GL_FLOAT, GL_FALSE, offsetof(BackpackVertex, TexCoords));
        glVertexArrayAttribBinding(VAO, 2, 0);
    }
    
    void Backpack::Mesh::Draw(GLuint shaderProgram) {
        unsigned int diffuseNr  = 1;
        unsigned int specularNr = 1;
        for(unsigned int i = 0; i < textures.size(); i++) {
            std::string number;
            std::string name = textures[i].type;
            if(name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if(name == "texture_specular")
                number = std::to_string(specularNr++);

            glProgramUniform1i(shaderProgram, glGetUniformLocation(shaderProgram, ("material." + name + number).c_str()), i);
            glBindTextureUnit(i, textures[i].id);
        }

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void Backpack::Mesh::cleanUp() {
        if(VBO) glDeleteBuffers(1, &VBO);
        if(EBO) glDeleteBuffers(1, &EBO);
        if(VAO) glDeleteVertexArrays(1, &VAO);
    }
}