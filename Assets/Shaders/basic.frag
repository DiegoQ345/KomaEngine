#version 460 core

in vec2 v_UV;
in vec3 v_WorldPos;
in vec3 v_Normal;

uniform sampler2D u_TextureSampler;
uniform vec3 u_BaseColor;
uniform int u_UseTexture;

uniform vec3 u_CameraPosition;
uniform vec3 u_LightDirection;

#define MAX_POINT_LIGHTS 4
uniform int u_PointLightCount;
uniform vec3 u_PointLightPosition[MAX_POINT_LIGHTS];
uniform vec3 u_PointLightColor[MAX_POINT_LIGHTS];

out vec4 FragColor;

vec3 srgbToLinear(vec3 color)
{
    vec3 value = clamp(color, 0.0, 1.0);
    return mix(value / 12.92,
               pow((value + 0.055) / 1.055, vec3(2.4)),
               step(vec3(0.04045), value));
}

vec3 linearToSrgb(vec3 color)
{
    vec3 value = clamp(color, 0.0, 1.0);
    return mix(value * 12.92,
               1.055 * pow(value, vec3(1.0 / 2.4)) - 0.055,
               step(vec3(0.0031308), value));
}

void main()
{
    vec3 baseColorLinear = srgbToLinear(u_BaseColor);
    vec3 surfaceColor = baseColorLinear;

    if (u_UseTexture == 1)
    {
        surfaceColor = texture(u_TextureSampler, v_UV).rgb * baseColorLinear;
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

    vec3 pointContribution = vec3(0.0);
    const int pointLightCount = min(u_PointLightCount, MAX_POINT_LIGHTS);

    for (int i = 0; i < pointLightCount; i++)
    {
        const vec3 toLight = u_PointLightPosition[i] - v_WorldPos;
        const float lightDistance = length(toLight);
        if (lightDistance < 1e-4) continue;

        const vec3 pointL = toLight / lightDistance;
        const float attenuation =
            1.0 / (1.0 + 0.09 * lightDistance + 0.032 * lightDistance * lightDistance);

        const float pointDiffuse = max(dot(N, pointL), 0.0) * attenuation;

        vec3 pointH = pointL + V;
        float pointSpecular = 0.0;
        if (pointDiffuse > 0.0 && dot(pointH, pointH) > 1e-6)
        {
            pointH = normalize(pointH);
            pointSpecular =
                pow(max(dot(N, pointH), 0.0), shininess) * specularStrength * attenuation;
        }

        pointContribution += srgbToLinear(u_PointLightColor[i]) * (pointDiffuse + pointSpecular);
    }

    vec3 linearColor =
        surfaceColor * (ambientStrength + diffuse) + vec3(specular) + pointContribution;

    FragColor = vec4(linearToSrgb(linearColor), 1.0);
}
