#version 330 core

layout(std140) uniform smol_camera {
    mat4 smol_view;
    mat4 smol_projection;
    vec3 smol_camera_position;
    float padding0;
    vec3 smol_camera_direction;
    float padding1;
};

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord; 

out vec2 frag_texcoord;

uniform mat4 smol_model_mat;

void main() 
{
    frag_texcoord = in_texcoord;

    gl_Position = smol_projection * smol_view * smol_model_mat * vec4(in_position, 1.0);
}
