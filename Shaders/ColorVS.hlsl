// 정점 셰이더 : gpu에서 정점에 대한 계산을 하는 곳.
// 주 목적은 공간변환에 있다.

/********************************************************/
// GLOBALS
/********************************************************/

// 전역변수, cpu에서 제공받는다.
cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatix;
};

/********************************************************/
// Structs
/********************************************************/

// 정점 셰이더에 들어오는 구조체
struct VertexInputType
{
    float4 position : POSITION;
    float4 color: COLOR;
};

// 픽셀 셰이더가 받을 구조체
struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

/********************************************************/
/* Main */
/********************************************************/
PixelInputType main(VertexInputType input)
{
    input.position.w = 1.0f;
    
    PixelInputType output;
    // 색상은 들어온 색상 그대로 복사한다.
    output.color = input.color;
    
    // 공간 변환
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatix);
    
    return output;
}