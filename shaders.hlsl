

cbuffer Transform : register(b0)
{
    float4x4 World; // ワールド行列
    float4x4 View; // ビュー行列
    float4x4 Proj; // 投影行列
}

struct VSInput
{
    float4 position : POSITION;
    float3 normal : NORMAL; // 法線
    float2 uv : TEXCOORD; // UV
    float3 tangent : TANGENT; // 接空間
    float4 color : COLOR;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

PSInput VSMain(VSInput input)
{
    PSInput result;

    result.position = input.position;
    result.position = mul(Proj, mul(View, mul(World, result.position)));
    result.color = input.color;

    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return input.color;
}
