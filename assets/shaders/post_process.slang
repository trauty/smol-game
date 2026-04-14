import smol_globals;

[ShaderConfig("PostProcess", "Opaque", false, false)]
struct PostProcessShader
{
    uint scene_color_tex;
};

StructuredBuffer<PostProcessShader> _reflection_PostProcessShader;

[RenderTargetsConfig]
struct PostProcessOutput
{
    [RenderTarget("Swapchain")]
    float4 color : SV_Target0;
}

struct PPVertexOut
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD0;
};

[shader("vertex")]
PPVertexOut vertexMain(uint vertex_id: SV_VertexID)
{
    PPVertexOut out;

    // generate triangle that covers whole screen with adjusted uv
    out.uv = float2((vertex_id << 1) & 2, vertex_id & 2);
    out.position = float4(out.uv * 2.0f - 1.0f, 0.0f, 1.0f);
    return out;
}

[shader("fragment")]
PostProcessOutput fragmentMain(PPVertexOut input)
{
    PostProcessShader mat = getMaterial<PostProcessShader>(pc.custom_data);

    float4 hdr_color = global_textures[mat.scene_color_tex].Sample(global_samplers[0], input.uv);

    float exposure = 1.12f;
    float3 color = hdr_color.rgb * exposure;

    // aces filmic curve
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    float3 mapped = clamp((color * (a * color + b)) / (color * (c * color + d) + e), 0.0f, 1.0f);

    PostProcessOutput out;
    out.color = float4(mapped, 1.0f);
    return out;
}
