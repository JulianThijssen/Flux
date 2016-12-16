#include "ModelImporter.h"

#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <Engine/Source/Log.h>
#include <Engine/Source/Path.h>
#include <Engine/Source/Mesh.h>

#include <vector>
#include <cstdio>
#include <iostream>

namespace Flux {
    Model* ModelImporter::loadModel(const Path& path) {
        Assimp::Importer importer;

        unsigned int flags = aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_SortByPType | aiProcess_GenUVCoords;
        const aiScene* scene = importer.ReadFile(path.str(), flags);

        if (!scene) {
            Log::error(importer.GetErrorString());
        }

        Model* model = uploadModel(*scene);
        return model;
    }

    Model* ModelImporter::uploadModel(const aiScene& scene) {
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

            // Store vertices in a buffer
            if (aiMesh->HasPositions()) {
                mesh.vertices.resize(aiMesh->mNumVertices);
                for (unsigned int j = 0; j < aiMesh->mNumVertices; j++) {
                    Vector3f v(aiMesh->mVertices[j].x, aiMesh->mVertices[j].y, aiMesh->mVertices[j].z);
                    mesh.vertices[j] = v;
                }
            }

            // Store texture coordinates in a buffer
            if (aiMesh->HasTextureCoords(0)) {
                mesh.texCoords.resize(aiMesh->mNumVertices);
                for (unsigned int j = 0; j < aiMesh->mNumVertices; j++) {
                    Vector2f t(aiMesh->mTextureCoords[0][j].x, aiMesh->mTextureCoords[0][j].y);
                    mesh.texCoords[j] = t;
                }
            }

            // Store normals in a buffer
            if (aiMesh->HasNormals()) {
                mesh.normals.resize(aiMesh->mNumVertices);
                for (unsigned int j = 0; j < aiMesh->mNumVertices; j++) {
                    Vector3f n(aiMesh->mNormals[j].x, aiMesh->mNormals[j].y, aiMesh->mNormals[j].z);
                    mesh.normals[j] = n;
                }
            }

            // Store tangents in a buffer
            if (aiMesh->HasTangentsAndBitangents()) {
                mesh.tangents.resize(aiMesh->mNumVertices);
                for (unsigned int j = 0; j < aiMesh->mNumVertices; j++) {
                    Vector3f v(aiMesh->mTangents[j].x, aiMesh->mTangents[j].y, aiMesh->mTangents[j].z);
                    mesh.tangents[j] = v;
                }
            }

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
