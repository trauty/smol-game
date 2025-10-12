#version 330 core

in vec2 frag_texcoord;

out vec4 out_color;

uniform vec4 smol_base_color;
uniform sampler2D smol_albedo_tex;

void main()
{
    out_color = texture(smol_albedo_tex, frag_texcoord) * smol_base_color;
}
