#include "Camera.h"

using namespace DirectX::SimpleMath;

Camera::Camera()
    : mUp(Vector3(0.0f, 1.0f, 0.0f)),
      mPosition(Vector3(0.0f, 0.0f, -2.0f)),
      mRight(Vector3(1.0f, 0.0f, 0.0f)),
      mLook(Vector3(0.0f, 0.5f, 1.0f)) {}

Camera::~Camera() {}


Vector3 Camera::GetEyePos() {
    return mPosition;
}
