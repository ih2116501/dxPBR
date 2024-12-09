#pragma once
#include <d3d11.h>
#include <directxtk/SimpleMath.h>
#include <string>
#include <vector>
#include <wrl/client.h>

using namespace Microsoft::WRL;

struct Vertex {

    DirectX::SimpleMath::Vector3 position;
    DirectX::SimpleMath::Vector2 texcoord;
    DirectX::SimpleMath::Vector3 normal;
    DirectX::SimpleMath::Vector3 color;
    DirectX::SimpleMath::Vector3 tangent;
    DirectX::SimpleMath::Vector3 biTangent;
};

struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<uint8_t> colors;
     
    std::string emissiveTextureFilename;
    std::string albedoTextureFilename;
    std::string aoTextureFilename;
    std::string heightTextureFilename;
    std::string normalTextureFilename;
    std::string metallicTextureFilename;
    std::string roughnessTextureFilename;
    std::string metallicRoughnessFilename;
};

struct Mesh {
    ComPtr<ID3D11Buffer> mVertexBuffer;
    ComPtr<ID3D11Buffer> mIndexBuffer;

    ComPtr<ID3D11Texture2D> albedoTexture;
    ComPtr<ID3D11Texture2D> emissiveTexture;
    ComPtr<ID3D11Texture2D> normalTexture;
    ComPtr<ID3D11Texture2D> heightTexture;
    ComPtr<ID3D11Texture2D> aoTexture;
    ComPtr<ID3D11Texture2D> metallicTexture;
    ComPtr<ID3D11Texture2D> roughnessTexture;
    ComPtr<ID3D11Texture2D> metallicRoughnessTexture;

    ComPtr<ID3D11ShaderResourceView> albedoSRV;
    ComPtr<ID3D11ShaderResourceView> emissiveSRV;
    ComPtr<ID3D11ShaderResourceView> normalSRV;
    ComPtr<ID3D11ShaderResourceView> heightSRV;
    ComPtr<ID3D11ShaderResourceView> aoSRV;
    ComPtr<ID3D11ShaderResourceView> metallicSRV;
    ComPtr<ID3D11ShaderResourceView> roughnessSRV;
    ComPtr<ID3D11ShaderResourceView> metallicRoughnessSRV;

    UINT indexCount = 0;
    UINT vertexCount = 0;
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
};