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

    // ���� ������ ������ŵ�ϴ�.
    lightDir = -lightDirection;

    // �� �ȼ��� ���� ���� ����մϴ�.
    lightIntensity = saturate(dot(input.normal, lightDir));

    // ���� ��� �ȼ��� ���� ������ ���Ͽ� ������ ������ ����� ���ϴ�.
    color = saturate(diffuseColor * lightIntensity);

    // ������ ����� �ؽ����� ������ ���Ͽ� �ؽ��ĸ� �����ŵ�ϴ�.
    color = color * textureColor;

    return color;
}
