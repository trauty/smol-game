import smol_globals;
import shader_interface;

[ShaderConfig("MainForwardPass", "Opaque", true, true)]
public struct UnlitShader : IShaderModule
{
    float4 color;
    uint albedo_tex;
    uint albedo_sampler;

    public float3 vertex(float3 local_pos, float3 normal, float time) { return float3(0.0f, 0.0f, 0.0f); }

    public float4 fragment(float2 uv, float3 world_pos, float3 normal)
    {
        Texture2D tex = getTexture(albedo_tex);
        SamplerState sampler = getSampler(albedo_sampler);
        return tex.Sample(sampler, uv) * color;
    }
};
