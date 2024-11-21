#pragma once
#include <d3d11.h>
#include <directxtk/SimpleMath.h>
#include <vector>

class Frustum {
  public:
    Frustum();
    ~Frustum();

    void Initlaize();

  private:
    DirectX::SimpleMath::Plane mUp;
    DirectX::SimpleMath::Plane mDown;
    DirectX::SimpleMath::Plane mLeft;
    DirectX::SimpleMath::Plane mRight;
    DirectX::SimpleMath::Plane mNear;
    DirectX::SimpleMath::Plane mFar;
};

