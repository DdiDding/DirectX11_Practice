Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer LightBuffer
{
    float4 diffuseColor;
    float3 lightDirection;
    float padding;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
float4 LightPixelShader(PixelInputType input) : SV_TARGET
{
    float4 textureColor;
    float3 lightDir;
    float lightIntensity;
    float4 color;

    textureColor = shaderTexture.Sample(SampleType, input.tex);

    // 빛의 방향을 반전시킵니다.
    lightDir = -lightDirection;

    // 이 픽셀의 빛의 양을 계산합니다.
    lightIntensity = saturate(dot(input.normal, lightDir));

    // 빛의 양과 픽셀의 최종 색상을 곱하여 최최종 색상을 만들어 냅니다.
    color = saturate(diffuseColor * lightIntensity);

    // 최최종 색상과 텍스쳐의 색상을 곱하여 텍스쳐를 적용시킵니다.
    color = color * textureColor;

    return color;
}
