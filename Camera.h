#pragma once
#include <d3d11.h>
#include <directxtk/SimpleMath.h>
#include <vector>

class Camera {
  public:
    Camera();
    ~Camera();

    DirectX::SimpleMath::Vector3 GetEyePos();
    DirectX::SimpleMath::Vector3 mPosition;
    DirectX::SimpleMath::Vector3 mRight;
    DirectX::SimpleMath::Vector3 mUp;
    DirectX::SimpleMath::Vector3 mLook;

  private:
};
