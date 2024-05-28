#pragma once
#include <directxmath.h>
using namespace DirectX;

struct VertexType
{
	XMFLOAT3 position;
	XMFLOAT2 texture;
	XMFLOAT3 normal;
};

struct VertexColor
{
	XMFLOAT3 position;
	XMFLOAT4 color;
};