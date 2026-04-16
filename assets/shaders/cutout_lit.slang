import smol_globals;
import util;
import lighting;

[ShaderConfig("MainForwardPass", "Cutout", true, true)]
struct CutoutLitShader
{
    float4 color;
    uint albedo_tex;
    uint albedo_sampler;
    float smoothness;
    float alpha_cutoff;
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
    CutoutLitShader mat = getMaterial<CutoutLitShader>(obj.material_offset);

    float4 tex_color =
        sampleTriplanar(getTexture(mat.albedo_tex), getSampler(mat.albedo_sampler), in.world_pos, in.normal, 10.0f);
    float4 base_color = tex_color * mat.color;

    if (base_color.a < mat.alpha_cutoff) { discard; }

    Surface surface;
    surface.albedo = base_color.rgb;
    surface.normal = normalize(in.normal);
    surface.view_dir = normalize(globals.camera_pos.xyz - in.world_pos);
    surface.smoothness = mat.smoothness;

    float3 final_color = applyBlinnPhongLighting(surface, in.world_pos);

    return FragmentOut(float4(final_color, base_color.a));
}
