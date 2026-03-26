import smol_globals;
import shader_interface;

import lighting;

[ShaderConfig("MainForwardPass", "Opaque", true, true)]
public struct SimpleLitShader : IShaderModule
{
    float4 color;
    uint albedo_tex;
    uint albedo_sampler;
    float smoothness;

    public float3 vertex(float3 local_pos, float3 normal, float time) { return float3(0.0f, 0.0f, 0.0f); }

    public float4 fragment(float2 uv, float3 world_pos, float3 normal)
    {
        GlobalData globals = getGlobalData();

        Surface surface;
        surface.albedo = getTexture(albedo_tex).Sample(getSampler(albedo_sampler), uv).rgb * color.rgb;
        surface.normal = normalize(normal);
        surface.view_dir = normalize(globals.camera_pos.xyz - world_pos);
        surface.smoothness = smoothness;

        float3 final_color = applyBlinnPhongLighting(surface, world_pos);

        return float4(final_color, 1.0f);
    }
};
