import smol_globals;

struct MaterialData
{
    float4 color;
    uint albedo_tex;
    uint albedo_sampler;
};

ConstantBuffer<MaterialData> material_reflection;

struct VertexOut
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD0;
    uint material_offset : BLENDINDICES0;
};

[shader("vertex")]
VertexOut vertexMain(uint vertex_id: SV_VertexID, uint draw_id: SV_DrawIndex)
{
    VertexOut out;

    GlobalData globals = getGlobalData();
    ObjectData obj = getObjectData(draw_id);
    Vertex vertex = getIndexedVertex(obj.vertex_buffer_id, obj.index_buffer_id, vertex_id);

    float4 world_pos = mul(float4(vertex.position, 1.0), obj.model_matrix);
    out.position = mul(world_pos, globals.view_proj);

    out.uv = vertex.uv;
    out.material_offset = obj.material_offset;

    return out;
}

[shader("fragment")]
float4 fragmentMain(VertexOut input) : SV_Target
{
    MaterialData mat = getMaterial<MaterialData>(pc.material_buffer_id, input.material_offset);

    Texture2D albedo_tex = global_textures[mat.albedo_tex];
    SamplerState sampler = global_samplers[mat.albedo_sampler];

    float4 color = albedo_tex.Sample(sampler, input.uv);
    return color * mat.color;
}
