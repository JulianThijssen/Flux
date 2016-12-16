#include "ModelLoader.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <glad/glad.h>

#include "../Engine/Source/Log.h"
#include "../Engine/Source/Path.h"
#include "../Engine/Source/Model.h"
#include "../Engine/Source/Mesh.h"

#include <vector>
#include <cstdio>
#include <iostream>

namespace Flux {
    Model* ModelLoader::loadModel(const Path& path) {
        Assimp::Importer importer;

        unsigned int flags = aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_SortByPType | aiProcess_GenUVCoords;
        const aiScene* scene = importer.ReadFile(path.str(), flags);

        if (!scene) {
            Log::error(importer.GetErrorString());
        }

        Model* model = uploadModel(*scene);
        return model;
    }

    Model* ModelLoader::uploadModel(const aiScene& scene) {
        Model* model = new Model();

        for (unsigned int i = 0; i < scene.mNumMeshes; i++) {
            aiMesh* aiMesh = scene.mMeshes[i];
            Mesh mesh;
            
            for (unsigned int j = 0; j < aiMesh->mNumFaces; j++) {
                const aiFace face = aiMesh->mFaces[j];
                mesh.indices.push_back(face.mIndices[0]);
                mesh.indices.push_back(face.mIndices[1]);
                mesh.indices.push_back(face.mIndices[2]);
            }

            // Generate vertex array object
            GLuint vao;
            glGenVertexArrays(1, &vao);
            glBindVertexArray(vao);

            // Store faces in a buffer
            GLuint faceVBO;
            glGenBuffers(1, &faceVBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, faceVBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * aiMesh->mNumFaces * 3, &mesh.indices[0], GL_STATIC_DRAW);

            // Store vertices in a buffer
            if (aiMesh->HasPositions()) {
                mesh.vertices.resize(aiMesh->mNumVertices);
                for (unsigned int j = 0; j < aiMesh->mNumVertices; j++) {
                    Vector3f v(aiMesh->mVertices[j].x, aiMesh->mVertices[j].y, aiMesh->mVertices[j].z);
                    mesh.vertices[j] = v;
                }

                GLuint vertexVBO;
                glGenBuffers(1, &vertexVBO);
                glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(float) * aiMesh->mNumVertices * 3, &mesh.vertices[0], GL_STATIC_DRAW);
                glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
                glEnableVertexAttribArray(0);
            }

            // Store texture coordinates in a buffer
            if (aiMesh->HasTextureCoords(0)) {
                mesh.texCoords.resize(aiMesh->mNumVertices);
                for (unsigned int j = 0; j < aiMesh->mNumVertices; j++) {
                    Vector2f t(aiMesh->mTextureCoords[0][j].x, aiMesh->mTextureCoords[0][j].y);
                    mesh.texCoords[j] = t;
                }

                GLuint texCoordVBO;
                glGenBuffers(1, &texCoordVBO);
                glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(float) * aiMesh->mNumVertices * 2, &mesh.texCoords[0], GL_STATIC_DRAW);
                glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, 0);
                glEnableVertexAttribArray(1);
            }

            // Store normals in a buffer
            if (aiMesh->HasNormals()) {
                mesh.normals.resize(aiMesh->mNumVertices);
                for (unsigned int j = 0; j < aiMesh->mNumVertices; j++) {
                    Vector3f n(aiMesh->mNormals[j].x, aiMesh->mNormals[j].y, aiMesh->mNormals[j].z);
                    mesh.normals[j] = n;
                }

                GLuint normalVBO;
                glGenBuffers(1, &normalVBO);
                glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(float) * aiMesh->mNumVertices * 3, &mesh.normals[0], GL_STATIC_DRAW);
                glVertexAttribPointer(2, 3, GL_FLOAT, false, 0, 0);
                glEnableVertexAttribArray(2);
            }

            // Store tangents in a buffer
            if (aiMesh->HasTangentsAndBitangents()) {
                mesh.tangents.resize(aiMesh->mNumVertices);
                for (unsigned int j = 0; j < aiMesh->mNumVertices; j++) {
                    Vector3f v(aiMesh->mTangents[j].x, aiMesh->mTangents[j].y, aiMesh->mTangents[j].z);
                    mesh.tangents[j] = v;
                }

                GLuint tangentVBO;
                glGenBuffers(1, &tangentVBO);
                glBindBuffer(GL_ARRAY_BUFFER, tangentVBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(float) * aiMesh->mNumVertices * 3, &mesh.tangents[0], GL_STATIC_DRAW);
                glVertexAttribPointer(3, 3, GL_FLOAT, false, 0, 0);
                glEnableVertexAttribArray(3);
            }

            // Unbind the buffers
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);

            // Store relevant data in the new mesh
            mesh.handle = vao;

            aiMaterial* aiMaterial = scene.mMaterials[aiMesh->mMaterialIndex];
            aiString name;
            aiMaterial->Get(AI_MATKEY_NAME, name);
            mesh.materialName = std::string(name.C_Str());
            std::cout << "Material name: " << mesh.materialName << std::endl;

            model->addMesh(mesh);
        }

        return model;
    }
}
