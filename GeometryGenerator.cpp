#include "GeometryGenerator.h"
#include <directxtk/SimpleMath.h>

using namespace DirectX::SimpleMath;

MeshData GeometryGenerator::CreateTriangle() {
    MeshData meshData;

    std::vector<Vector3> positions;
    std::vector<Vector3> normals;
    std::vector<Vector2> texcoords;

    positions.push_back(Vector3(-0.5f, -0.5f, 0.0f));
    positions.push_back(Vector3(0.0f, 0.5f, 0.0f));
    positions.push_back(Vector3(0.5f, -0.5f, 0.0f));

    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));

    texcoords.push_back(Vector2(0.0f, 1.0f));
    texcoords.push_back(Vector2(1.0f, 1.0f));
    texcoords.push_back(Vector2(0.5f, 0.0f));

    for (int i = 0; i < positions.size(); i++) {
        Vertex v;
        v.position = positions[i];
        v.normal = normals[i];
        v.texcoord = texcoords[i];

        meshData.vertices.push_back(v);
    }

    meshData.indices = {0, 1, 2};

    return meshData;
}

MeshData GeometryGenerator::CreateSquare(const float scale,
                                         const Vector2 texScale) {
    std::vector<Vector3> positions;
    std::vector<Vector2> texcoords;
    std::vector<Vector3> normals;
    std::vector<Vector3> colors;

    positions.push_back(Vector3(-1.0f, 1.0f, 0.0f) * scale);
    positions.push_back(Vector3(1.0f, 1.0f, 0.0f) * scale);
    positions.push_back(Vector3(1.0f, -1.0f, 0.0f) * scale);
    positions.push_back(Vector3(-1.0f, -1.0f, 0.0f) * scale);
    texcoords.push_back(Vector2(0.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 1.0f));
    texcoords.push_back(Vector2(0.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));

    MeshData meshData;

    for (size_t i = 0; i < positions.size(); i++) {
        Vertex v;
        v.position = positions[i];
        v.normal = normals[i];
        v.texcoord = texcoords[i] * texScale;

        meshData.vertices.push_back(v);
    }
    meshData.indices = {
        0, 1, 2, 0, 2, 3,
    };

    return meshData;
}

MeshData GeometryGenerator::CreateSphere(uint32_t numStacks, uint32_t numSlices,
                                         Vector2 texScale) {

    MeshData meshData;

    const float radius = 1.0f;
    const float dTheta = -DirectX::XM_2PI / float(numSlices);
    const float dPhi = -DirectX::XM_PI / float(numStacks);

    for (size_t j = 0; j <= numStacks; j++) {
        Vector3 startPosition = Vector3::Transform(
            Vector3{0.0f, -radius, 0.0f}, Matrix::CreateRotationZ(dPhi * j));

        for (size_t i = 0; i <= numSlices; i++) {
            Vertex v;
            v.position = Vector3::Transform(
                startPosition, Matrix::CreateRotationY(dTheta * float(i)));
            v.normal = v.position;
            v.normal.Normalize();
            v.color = Vector3{0.0f, 0.0f, 1.0f};
            v.texcoord =
                Vector2(float(i) / numSlices, 1.0f - float(j) / numStacks) *
                texScale;
            // v.texcoord =
            // Vector2(float(i) / numSlices, 1 - (v.position.y +
            // radius)/(2*radius)) * texScale;

            Vector3 biTangent = Vector3(0.0f, -1.0f, 0.0f);

            biTangent = biTangent - biTangent.Dot(v.normal) * v.normal;
            biTangent.Normalize();

            v.tangent = biTangent.Cross(v.normal);
            v.tangent.Normalize();
            meshData.vertices.push_back(v);
        }
    }

    std::vector<uint32_t> indices;
    for (int j = 0; j < numStacks; j++) {

        const int offset = (numSlices + 1) * j;

        for (int i = 0; i < numSlices; i++) {

            indices.push_back(offset + i);
            indices.push_back(offset + i + numSlices + 1);
            indices.push_back(offset + i + 1 + numSlices + 1);

            indices.push_back(offset + i);
            indices.push_back(offset + i + 1 + numSlices + 1);
            indices.push_back(offset + i + 1);
        }
    }
    meshData.indices = indices;

    return meshData;
}

MeshData GeometryGenerator::CreateBox(float scale = 1.0f) {
    MeshData meshData;
    std::vector<Vector3> positions;
    std::vector<Vector3> normals;
    std::vector<Vector2> texcoords;
    std::vector<Vector3> colors;

    // À­¸é
    positions.push_back(Vector3(-1.0f, 1.0f, -1.0f) * scale);
    positions.push_back(Vector3(-1.0f, 1.0f, 1.0f) * scale);
    positions.push_back(Vector3(1.0f, 1.0f, 1.0f) * scale);
    positions.push_back(Vector3(1.0f, 1.0f, -1.0f) * scale);
    normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
    texcoords.push_back(Vector2(0.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 1.0f));
    texcoords.push_back(Vector2(0.0f, 1.0f));
    colors.push_back(Vector3(0.9f, 0.2f, 0.8f));
    colors.push_back(Vector3(0.9f, 0.2f, 0.8f));
    colors.push_back(Vector3(0.9f, 0.2f, 0.8f));
    colors.push_back(Vector3(0.9f, 0.2f, 0.8f));

    // ¾Æ·§¸é
    positions.push_back(Vector3(-1.0f, -1.0f, -1.0f) * scale);
    positions.push_back(Vector3(1.0f, -1.0f, -1.0f) * scale);
    positions.push_back(Vector3(1.0f, -1.0f, 1.0f) * scale);
    positions.push_back(Vector3(-1.0f, -1.0f, 1.0f) * scale);
    normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
    texcoords.push_back(Vector2(0.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 1.0f));
    texcoords.push_back(Vector2(0.0f, 1.0f));
    colors.push_back(Vector3(0.9f, 0.2f, 0.8f));
    colors.push_back(Vector3(0.9f, 0.2f, 0.8f));
    colors.push_back(Vector3(0.9f, 0.2f, 0.8f));
    colors.push_back(Vector3(0.9f, 0.2f, 0.8f));

    // ¾Õ¸é
    positions.push_back(Vector3(-1.0f, -1.0f, -1.0f) * scale);
    positions.push_back(Vector3(-1.0f, 1.0f, -1.0f) * scale);
    positions.push_back(Vector3(1.0f, 1.0f, -1.0f) * scale);
    positions.push_back(Vector3(1.0f, -1.0f, -1.0f) * scale);
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    texcoords.push_back(Vector2(0.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 1.0f));
    texcoords.push_back(Vector2(0.0f, 1.0f));
    colors.push_back(Vector3(0.2f, 0.9f, 0.1f));
    colors.push_back(Vector3(0.2f, 0.9f, 0.1f));
    colors.push_back(Vector3(0.2f, 0.9f, 0.1f));
    colors.push_back(Vector3(0.2f, 0.9f, 0.1f));

    // µÞ¸é
    positions.push_back(Vector3(-1.0f, -1.0f, 1.0f) * scale);
    positions.push_back(Vector3(1.0f, -1.0f, 1.0f) * scale);
    positions.push_back(Vector3(1.0f, 1.0f, 1.0f) * scale);
    positions.push_back(Vector3(-1.0f, 1.0f, 1.0f) * scale);
    normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
    texcoords.push_back(Vector2(0.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 1.0f));
    texcoords.push_back(Vector2(0.0f, 1.0f));
    colors.push_back(Vector3(0.2f, 0.9f, 0.1f));
    colors.push_back(Vector3(0.2f, 0.9f, 0.1f));
    colors.push_back(Vector3(0.2f, 0.9f, 0.1f));
    colors.push_back(Vector3(0.2f, 0.9f, 0.1f));

    // ¿ÞÂÊ
    positions.push_back(Vector3(-1.0f, -1.0f, 1.0f) * scale);
    positions.push_back(Vector3(-1.0f, 1.0f, 1.0f) * scale);
    positions.push_back(Vector3(-1.0f, 1.0f, -1.0f) * scale);
    positions.push_back(Vector3(-1.0f, -1.0f, -1.0f) * scale);
    normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
    normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
    normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
    normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
    texcoords.push_back(Vector2(0.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 1.0f));
    texcoords.push_back(Vector2(0.0f, 1.0f));
    colors.push_back(Vector3(0.2f, 0.1f, 0.9f));
    colors.push_back(Vector3(0.2f, 0.1f, 0.9f));
    colors.push_back(Vector3(0.2f, 0.1f, 0.9f));
    colors.push_back(Vector3(0.2f, 0.1f, 0.9f));

    // ¿À¸¥ÂÊ
    positions.push_back(Vector3(1.0f, -1.0f, 1.0f) * scale);
    positions.push_back(Vector3(1.0f, -1.0f, -1.0f) * scale);
    positions.push_back(Vector3(1.0f, 1.0f, -1.0f) * scale);
    positions.push_back(Vector3(1.0f, 1.0f, 1.0f) * scale);
    normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
    normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
    normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
    normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
    texcoords.push_back(Vector2(0.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 1.0f));
    texcoords.push_back(Vector2(0.0f, 1.0f));
    colors.push_back(Vector3(0.9f, 0.2f, 0.1f));
    colors.push_back(Vector3(0.9f, 0.2f, 0.1f));
    colors.push_back(Vector3(0.9f, 0.2f, 0.1f));
    colors.push_back(Vector3(0.9f, 0.2f, 0.1f));

    for (size_t i = 0; i < positions.size(); i++) {
        Vertex v;
        v.position = positions[i];
        v.normal = normals[i];
        v.texcoord = texcoords[i];
        v.color = colors[i];
        meshData.vertices.push_back(v);
    }

    meshData.indices = {
        0,  1,  2,  0,  2,  3,  // À­¸é
        4,  5,  6,  4,  6,  7,  // ¾Æ·§¸é
        8,  9,  10, 8,  10, 11, // ¾Õ¸é
        12, 13, 14, 12, 14, 15, // µÞ¸é
        16, 17, 18, 16, 18, 19, // ¿ÞÂÊ
        20, 21, 22, 20, 22, 23  // ¿À¸¥ÂÊ
    };

    return meshData;
}

void GeometryGenerator::NormalizeMesh(std::vector<MeshData> &meshes,
                                      float scale) {
    Vector3 vmin(1000);
    Vector3 vmax(-1000);
    for (auto &mesh : meshes) {
        for (auto &v : mesh.vertices) {
            vmin.x = v.position.x < vmin.x ? v.position.x : vmin.x;
            vmin.y = v.position.y < vmin.y ? v.position.y : vmin.y;
            vmin.z = v.position.z < vmin.z ? v.position.z : vmin.z;

            vmax.x = v.position.x > vmax.x ? v.position.x : vmax.x;
            vmax.y = v.position.y > vmax.y ? v.position.y : vmax.y;
            vmax.z = v.position.z > vmax.z ? v.position.z : vmax.z;
        }
    }
    float dx = vmax.x - vmin.x;
    float dy = vmax.y - vmin.y;
    float dz = vmax.z - vmin.z;

    float maxD = dx > dy ? (dx > dz ? dx : dz) : (dy > dz ? dy : dz);
    Vector3 center = Vector3((vmax.x + vmin.x) * 0.5, (vmax.y + vmin.y) * 0.5,
                             (vmax.z + vmin.z) * 0.5);

    for (auto &mesh : meshes) {
        for (auto &v : mesh.vertices) {
            v.position = ((v.position - center) / maxD) * scale;
        }
    }
}