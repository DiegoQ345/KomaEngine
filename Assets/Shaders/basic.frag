#version 460 core

in vec2 v_UV;

uniform sampler2D u_TextureSampler;

out vec4 FragColor;

void main()
{
    FragColor = texture(u_TextureSampler, v_UV);
}
