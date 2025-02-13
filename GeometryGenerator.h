#pragma once

#include "D3DUtils.h"
#include "Mesh.h"
#include <memory>
#include <vector>
#include <wrl/client.h>

using namespace Microsoft::WRL;

namespace GeometryGenerator {
MeshData CreateTriangle();
MeshData CreateSquare(const float scale = 1.0f,
                      const DirectX::SimpleMath::Vector2 texScale =
                          DirectX::SimpleMath::Vector2(1.0f));
MeshData CreateSphere(
    uint32_t numStack, uint32_t numSlice,
    DirectX::SimpleMath::Vector2 texScale = DirectX::SimpleMath::Vector2(1.0f));
MeshData CreateBox(float scale);
void NormalizeMesh(std::vector<MeshData> &meshes, float scale);

//void ComputeFaceNormal(
//    MeshData &meshData,
//    DirectX::SimpleMath::Vector2 texScale = DirectX::SimpleMath::Vector2(1.0f));

} // namespace GeometryGenerator
