#version 460 core

in vec3 v_Direction;

uniform samplerCube u_Skybox;

out vec4 FragColor;

vec3 linearToSrgb(vec3 color)
{
    vec3 value = clamp(color, 0.0, 1.0);
    return mix(value * 12.92,
               1.055 * pow(value, vec3(1.0 / 2.4)) - 0.055,
               step(vec3(0.0031308), value));
}

void main()
{
    vec3 color = texture(u_Skybox, normalize(v_Direction)).rgb;
    FragColor = vec4(linearToSrgb(color), 1.0);
}
