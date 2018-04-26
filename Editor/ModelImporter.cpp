#include "ModelImporter.h"

#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>

#include "Util/Log.h"
#include "Util/Path.h"
#include "Mesh.h"

#include <vector>
#include <iostream>
#include <algorithm>

namespace Flux {
    namespace Editor {
        Model ModelImporter::ImportFromFile(const Path& path) {
            Assimp::Importer importer;

            unsigned int flags = aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_SortByPType | aiProcess_GenUVCoords | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices;
            const aiScene* scene = importer.ReadFile(path.str(), flags);

            if (!scene) {
                Log::error(importer.GetErrorString());
            }

            Model model = ReadModelFromScene(*scene);

            return model;
        }

        Model ModelImporter::ReadModelFromScene(const aiScene& scene) {
            Model model;

            for (unsigned int i = 0; i < scene.mNumMeshes; i++) {
                aiMesh* aiMesh = scene.mMeshes[i];

                Mesh* mesh = new Mesh();

                mesh->indices.resize(aiMesh->mNumFaces * 3);
                for (unsigned int j = 0; j < aiMesh->mNumFaces; j++) {
                    const aiFace face = aiMesh->mFaces[j];
                    mesh->indices[j * 3 + 0] = face.mIndices[0];
                    mesh->indices[j * 3 + 1] = face.mIndices[1];
                    mesh->indices[j * 3 + 2] = face.mIndices[2];
                }

                // Store vertices in a buffer
                if (aiMesh->HasPositions()) {
                    mesh->vertices.resize(aiMesh->mNumVertices);
                    memcpy(&mesh->vertices[0], aiMesh->mVertices, aiMesh->mNumVertices * sizeof(aiVector3D));
                }

                // Store texture coordinates in a buffer
                if (aiMesh->HasTextureCoords(0)) {
                    mesh->texCoords.resize(aiMesh->mNumVertices);
                    // We want to copy the 0th element from every vertex, since this is strided memory we can't use memcpy
                    for (unsigned int j = 0; j < aiMesh->mNumVertices; j++) {
                        Vector2f t(aiMesh->mTextureCoords[0][j].x, 1 - aiMesh->mTextureCoords[0][j].y);
                        mesh->texCoords[j] = t;
                    }
                }
                else {
                    mesh->texCoords.resize(aiMesh->mNumVertices, Vector2f(0, 0));
                }

                // Store normals in a buffer
                if (aiMesh->HasNormals()) {
                    mesh->normals.resize(aiMesh->mNumVertices);
                    memcpy(&mesh->normals[0], aiMesh->mNormals, aiMesh->mNumVertices * sizeof(aiVector3D));
                }

                // Store tangents in a buffer
                if (aiMesh->HasTangentsAndBitangents()) {
                    mesh->tangents.resize(aiMesh->mNumVertices);
                    memcpy(&mesh->tangents[0], aiMesh->mTangents, aiMesh->mNumVertices * sizeof(aiVector3D));
                }
                else {
                    mesh->tangents.resize(aiMesh->mNumVertices, Vector3f(0, 0, 0));
                }

                aiMaterial* aiMaterial = scene.mMaterials[aiMesh->mMaterialIndex];
                aiString name;
                aiMaterial->Get(AI_MATKEY_NAME, name);
                mesh->materialName = std::string(name.C_Str());
                std::cout << "Material name: " << mesh->materialName << std::endl;

                model.addMesh(mesh);
            }

            return model;
        }
    } // namespace Editor
} // namespace Flux
