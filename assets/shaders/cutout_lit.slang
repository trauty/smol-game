import smol_globals;
import shader_interface;

import util;
import lighting;

[ShaderConfig("MainForwardPass", "Cutout", true, true)]
public struct CutoutLitShader : IShaderModule
{
    public float4 color;
    public uint albedo_tex;
    public uint albedo_sampler;
    public float smoothness;

    public float alpha_cutoff;

    public float3 vertex(float3 local_pos, float3 normal, float time) { return float3(0.0f, 0.0f, 0.0f); }

    public float4 fragment(float2 uv, float3 world_pos, float3 normal)
    {
        float4 tex_color =
            sampleTriplanar(getTexture(albedo_tex), getSampler(albedo_sampler), world_pos, normal, 10.0f);
        float4 base_color = tex_color * color;

        if (base_color.a < alpha_cutoff) { discard; }

        GlobalData globals = getGlobalData();

        Surface surface;
        surface.albedo = base_color.rgb;
        surface.normal = normalize(normal);
        surface.view_dir = normalize(globals.camera_pos.xyz - world_pos);
        surface.smoothness = smoothness;

        float3 final_color = applyBlinnPhongLighting(surface, world_pos);

        return float4(final_color, base_color.a);
    }
};
