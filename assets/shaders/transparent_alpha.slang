import smol_globals;
import lighting;

[ShaderConfig("MainForwardPass", "TransparentAlpha", false, true)]
struct TransparentAlphaShader
{
    float4 color;
    uint albedo_tex;
    uint albedo_sampler;
    float smoothness;
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
    GlobalData globals = getGlobalData();
    ObjectData obj = getObjectData(in.draw_id);

    TransparentAlphaShader mat = getMaterial<TransparentAlphaShader>(obj.material_offset);

    Surface surface;
    surface.albedo = getTexture(mat.albedo_tex).Sample(getSampler(mat.albedo_sampler), in.uv).rgb * mat.color.rgb;
    surface.normal = normalize(in.normal);
    surface.view_dir = normalize(globals.camera_pos.xyz - in.world_pos);
    surface.smoothness = mat.smoothness;

    float3 final_color = applyBlinnPhongLighting(surface, in.world_pos);

    FragmentOut out;
    out.color = float4(final_color, 1.0f);
    return out;
}

