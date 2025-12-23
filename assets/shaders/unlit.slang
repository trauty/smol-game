cbuffer smol_camera
{
    float4x4 smol_view;
    float4x4 smol_projection;
    float3   smol_camera_position;
    float    _pad0;
    float3   smol_camera_direction;
    float    _pad1;
};


float4x4 smol_model_mat;
float4   smol_base_color;

Sampler2D smol_albedo_tex;

struct VSInput
{
    [[vk::location(0)]] float3 position : POSITION;
    [[vk::location(1)]] float3 normal   : NORMAL;
    [[vk::location(2)]] float2 uv       : TEXCOORD0;
};

struct VSOutput
{
    float4 position : SV_Position;
    float2 uv       : TEXCOORD0;
};

[shader("vertex")]
VSOutput vertexMain(VSInput input)
{
    VSOutput output;
    
    output.uv = input.uv;
    
    float4 worldPos = mul(smol_model_mat, float4(input.position, 1.0));
    float4 viewPos  = mul(smol_view, worldPos);
    
    output.position = mul(smol_projection, viewPos);

    return output;
}

[shader("fragment")]
float4 fragmentMain(VSOutput input) : SV_Target
{
    return smol_albedo_tex.Sample(input.uv) * smol_base_color;
}