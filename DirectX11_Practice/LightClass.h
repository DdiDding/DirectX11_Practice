#pragma once
#include <directxmath.h>
using namespace DirectX;

class LightClass
{
public:
    LightClass() = default;
    LightClass(const LightClass&) = default;
    ~LightClass() = default;

    void SetDiffuseColor(float, float, float, float);
    void SetDirection(float, float, float);
    void SetDirection(XMFLOAT3);

    XMFLOAT4 GetDiffuseColor();
    XMFLOAT3 GetDirection();

private:
    XMFLOAT4 m_diffuseColor;
    XMFLOAT3 m_direction;
};
