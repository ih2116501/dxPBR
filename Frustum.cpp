#include "Frustum.h"

using namespace DirectX::SimpleMath;

Frustum::Frustum() {}
Frustum::~Frustum() {}

void Frustum::Initlaize() {
    Plane p(1.0f, 0.0f, 0.0f, 0.0f);
}
