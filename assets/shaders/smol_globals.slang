struct GlobalData
{
    float4x4 view;
    float4x4 projection;
    float4 camera_pos;
    float4 camera_dir;
};

[[vk::binding(0, 0)]] ConstantBuffer<GlobalData> smol;

struct ModelPushConstant
{
    float4x4 model_matrix;
};

[[vk::push_constant]] ModelPushConstant smol_push_constants;