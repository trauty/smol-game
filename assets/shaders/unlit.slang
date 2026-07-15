import smol_globals;

[ShaderConfig("MainForwardPass", "Opaque", true, true, true)]
struct UnlitShader
{
    float4 color;
    uint albedo_tex;
    uint albedo_sampler;

    property Texture albedo { get { return {albedo_tex, albedo_sampler}; } }
};

[RenderTargetsConfig]
struct FragmentOut
{
    [RenderTarget("RGBA16_FLOAT")]
    float4 color : SV_Target0;
};

[shader("vertex")]
VertexOut vertexMain(uint vertex_id: SV_VertexID, uint instance_id: SV_InstanceID,
                     uint base_instance: SV_StartInstanceLocation)
{ return defaultVertexTransform(vertex_id, instance_id, base_instance); }

[shader("fragment")]
FragmentOut fragmentMain(VertexOut in)
{
    ObjectData obj = getObjectData(in.draw_id);
    UnlitShader mat = getMaterial<UnlitShader>(obj.material_offset);

    float4 color = mat.albedo.sample(in.uv) * mat.color;

    FragmentOut out;
    out.color = color;

    return out;
}
