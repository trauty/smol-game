import "smol_globals";

[[vk::binding(0, 1)]] Texture2D albedo_tex;
[[vk::binding(1, 1)]] SamplerState albedo_sampler;

struct VertexIn
{
    [[vk::location(0)]] float3 pos : POSITION;
    [[vk::location(1)]] float3 normal : NORMAL;
    [[vk::location(2)]] float2 uv : TEXCOORD0;
};

struct VertexOut
{
    float4 pos : SV_Position;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
};

[shader("vertex")]
VertexOut vertexMain(VertexIn input)
{
    VertexOut out;

    float4 world_pos = mul(smol_push_constants.model_matrix, float4(input.pos, 1.0));

    out.pos = mul(smol.projection, mul(smol.view, world_pos));
    out.normal = mul((float3x3)smol_push_constants.model_matrix, input.normal);
    out.uv = input.uv;

    return out;
}

[shader("fragment")]
float4 fragmentMain(VertexOut input) : SV_Target
{
    float4 color = albedo_tex.Sample(albedo_sampler, input.uv);
    return color;
}