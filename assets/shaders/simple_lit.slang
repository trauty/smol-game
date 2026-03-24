import smol_globals;
import shader_interface;

struct Surface
{
    float3 albedo;
    float3 normal;
    float3 view_dir;
    float smoothness;
};

float3 evaluateBlinnPhong(Surface surface, IncidentLight light)
{
    if (length(light.radiance) <= 0.001) { return float3(0.0f); }

    float n_dot_l = max(dot(surface.normal, light.direction), 0.0f);
    float3 diffuse = surface.albedo * light.radiance * n_dot_l;

    float3 halfway_vec = normalize(light.direction + surface.view_dir);
    float3 specular = pow(max(dot(surface.normal, halfway_vec), 0.0f), surface.smoothness) * light.radiance;

    return diffuse + specular;
}

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
        surface.albedo = global_textures[albedo_tex].Sample(global_samplers[albedo_sampler], uv).rgb * color.rgb;
        surface.normal = normalize(normal);
        surface.view_dir = normalize(globals.camera_pos.xyz - world_pos);
        surface.smoothness = smoothness;

        float3 final_color = float3(0.02f);

        for (uint d = 0; d < globals.dir_light_count; d++)
        {
            DirectionalLight dl =
                global_buffers[globals.dir_light_buffer_id].Load<DirectionalLight>(d * sizeof(DirectionalLight));
            final_color += evaluateBlinnPhong(surface, dl.evaluate(world_pos));
        }

        for (uint p = 0; p < globals.point_light_count; p++)
        {
            PointLight pl = global_buffers[globals.point_light_buffer_id].Load<PointLight>(p * sizeof(PointLight));
            final_color += evaluateBlinnPhong(surface, pl.evaluate(world_pos));
        }

        for (uint s = 0; s < globals.point_light_count; s++)
        {
            SpotLight sl = global_buffers[globals.point_light_buffer_id].Load<SpotLight>(s * sizeof(SpotLight));
            final_color += evaluateBlinnPhong(surface, sl.evaluate(world_pos));
        }

        return float4(final_color, 1.0);
    }
};
