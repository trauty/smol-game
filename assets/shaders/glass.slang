import smol_globals;
import shader_interface;

[ShaderConfig("MainForwardPass", "TransparentAlpha", false, true)]
public struct GlassShader : IShaderModule
{
    public float4 color;

    public float3 vertex(float3 local_pos, float3 normal, float time) { return float3(0.0f); }

    public float4 fragment(float2 uv, float3 world_pos, float3 normal) { return color; }
}
