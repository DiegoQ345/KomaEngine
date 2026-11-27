#version 460 core

in vec2 v_UV;
in vec3 v_WorldPos;
in vec3 v_Normal;

uniform sampler2D u_TextureSampler;
uniform vec3 u_BaseColor;
uniform int u_UseTexture;

uniform vec3 u_CameraPosition;
uniform vec3 u_LightDirection;

out vec4 FragColor;

void main()
{
    vec3 surfaceColor = u_BaseColor;

    if (u_UseTexture == 1)
    {
        surfaceColor = texture(u_TextureSampler, v_UV).rgb * u_BaseColor;
    }

    const float ambientStrength = 0.25;
    const float specularStrength = 0.8;
    const float shininess = 16.0;

    vec3 N = normalize(v_Normal);
    vec3 L = normalize(u_LightDirection);
    float diffuse = max(dot(N, L), 0.0);

    vec3 V = normalize(u_CameraPosition - v_WorldPos);
    vec3 H = L + V;

    float specular = 0.0;
    if (diffuse > 0.0 && dot(H, H) > 1e-6)
    {
        H = normalize(H);
        specular = pow(max(dot(N, H), 0.0), shininess) * specularStrength;
    }

    FragColor = vec4(surfaceColor * (ambientStrength + diffuse) + vec3(specular), 1.0);
}
