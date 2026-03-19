import smol_globals;

[MaterialReflection]
struct LitMaterialData
{
    float4 color;
    uint albedo_tex;
    uint albedo_sampler;
    float smoothness;
};

ConstantBuffer<LitMaterialData> material_reflection;

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

struct VertexOut
{
    float4 position : SV_Position;
    float3 world_pos : POSITION;
    float3 normal : NORMAL;
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

    float4 world_pos = mul(float4(vertex.position, 1.0f), obj.model_matrix);
    out.position = mul(world_pos, globals.view_proj);

    out.world_pos = world_pos.xyz;
    out.normal = normalize(mul(vertex.normal, (float3x3)obj.normal_matrix));
    out.uv = vertex.uv;
    out.material_offset = obj.material_offset;

    return out;
}

[RenderConfig("MainForwardPass", "Opaque", true, true)]
struct FragmentOut
{
    [RenderTarget("RGBA16_FLOAT")]
    float4 color;
};

[shader("fragment")]
FragmentOut fragmentMain(VertexOut in)
{
    FragmentOut out;
    GlobalData globals = getGlobalData();
    LitMaterialData mat = getMaterial<LitMaterialData>(pc.material_buffer_id, in.material_offset);

    Surface surface;
    surface.albedo =
        global_textures[mat.albedo_tex].Sample(global_samplers[mat.albedo_sampler], in.uv).rgb * mat.color.rgb;
    surface.normal = normalize(in.normal);
    surface.view_dir = normalize(globals.camera_pos.xyz - in.world_pos);
    surface.smoothness = mat.smoothness;

    float3 final_color = float3(0.02f);

    for (uint d = 0; d < globals.dir_light_count; d++)
    {
        DirectionalLight dl =
            global_buffers[globals.dir_light_buffer_id].Load<DirectionalLight>(d * sizeof(DirectionalLight));
        final_color += evaluateBlinnPhong(surface, dl.evaluate(in.world_pos));
    }

    for (uint p = 0; p < globals.point_light_count; p++)
    {
        PointLight pl = global_buffers[globals.point_light_buffer_id].Load<PointLight>(p * sizeof(PointLight));
        final_color += evaluateBlinnPhong(surface, pl.evaluate(in.world_pos));
    }

    for (uint s = 0; s < globals.point_light_count; s++)
    {
        SpotLight sl = global_buffers[globals.point_light_buffer_id].Load<SpotLight>(s * sizeof(SpotLight));
        final_color += evaluateBlinnPhong(surface, sl.evaluate(in.world_pos));
    }

    out.color = float4(final_color, 1.0);
    return out;
}
