#version 460 core

in vec2 v_UV;

uniform sampler2D u_TextureSampler;
uniform vec3 u_BaseColor;
uniform int u_UseTexture;

out vec4 FragColor;

void main()
{
    vec3 color = u_BaseColor;

    if (u_UseTexture == 1)
    {
        color = texture(u_TextureSampler, v_UV).rgb * u_BaseColor;
    }

    FragColor = vec4(color, 1.0);
}
