#pragma once
#include "Mesh.h"
#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include <assimp\scene.h>
#include <iostream>
#include <string>
#include <vector>

class ModelLoader {
  public:
    void Load(std::string basePath, std::string filename, bool revertNormals);

    void ProcessNode(aiNode *node, const aiScene *scene,
                     DirectX::SimpleMath::Matrix tr);

    MeshData ProcessMesh(aiMesh *mesh, const aiScene *scene);

    std::string ReadFilename(aiMaterial *material, aiTextureType type);

    void UpdateTangents();
    std::vector<MeshData> GetMeshes() const;
  public:
    std::string basePath;
    std::vector<MeshData> mMeshes;
    bool m_isGLTF = false;
    bool m_revertNormals = false;
};