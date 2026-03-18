import smol_globals;

[MaterialReflection]
struct PostProcessData
{
    uint scene_color_tex;
};

ConstantBuffer<PostProcessData> material_reflection;

[RenderConfig("PostProcess", "Opaque", false, false)]
struct PostProcessOutput
{
    [RenderTarget("Swapchain")]
    float4 color : SV_Target0;
}

struct VertexOut
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD0;
};

[shader("vertex")]
VertexOut vertexMain(uint vertex_id: SV_VertexID)
{
    VertexOut out;

    // generate triangle that covers whole screen with adjusted uv
    out.uv = float2((vertex_id << 1) & 2, vertex_id & 2);
    out.position = float4(out.uv * 2.0f - 1.0f, 0.0f, 1.0f);
    return out;
}

[shader("fragment")]
PostProcessOutput fragmentMain(VertexOut input)
{
    PostProcessData mat = getMaterial<PostProcessData>(pc.material_buffer_id, pc.custom_data);

    float4 hdr_color = global_textures[mat.scene_color_tex].Sample(global_samplers[0], input.uv);

    // reinhard tonemapping
    float3 mapped = hdr_color.rgb / (hdr_color.rgb + float3(1.0f));

    // gamma correction
    mapped = pow(mapped, float3(1.0f / 2.2f));

    PostProcessOutput out;
    out.color = float4(mapped, 1.0f);
    return out;
}
