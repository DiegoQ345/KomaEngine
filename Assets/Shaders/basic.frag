#version 460 core

in vec2 v_UV;
in vec3 v_WorldPos;
in vec3 v_Normal;

uniform sampler2D u_TextureSampler;
uniform vec3 u_BaseColor;
uniform int u_UseTexture;

uniform vec3 u_CameraPosition;
uniform vec3 u_LightDirection;

uniform int u_UsePBR;
uniform float u_Metallic;
uniform float u_Roughness;

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

float DistributionGGX(float NdotH, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float denom = NdotH * NdotH * (a2 - 1.0) + 1.0;
    return a2 / max(3.14159265 * denom * denom, 1e-5);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    return NdotV / max(NdotV * (1.0 - k) + k, 1e-5);
}

float GeometrySmith(float NdotV, float NdotL, float roughness)
{
    return GeometrySchlickGGX(NdotV, roughness) * GeometrySchlickGGX(NdotL, roughness);
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 PBRContribution(vec3 albedo, vec3 N, vec3 V, vec3 L, vec3 radiance,
                     float metallic, float roughness)
{
    vec3 H = normalize(L + V);
    float NdotL = max(dot(N, L), 0.0);
    float NdotV = max(dot(N, V), 1e-4);
    float NdotH = max(dot(N, H), 0.0);
    float HdotV = max(dot(H, V), 0.0);

    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    vec3 F = FresnelSchlick(HdotV, F0);
    float D = DistributionGGX(NdotH, roughness);
    float G = GeometrySmith(NdotV, NdotL, roughness);
    vec3 specular = (D * G * F) / max(4.0 * NdotV * NdotL, 1e-4);

    vec3 kD = (vec3(1.0) - F) * (1.0 - metallic);

    return (kD * albedo / 3.14159265 + specular) * radiance * NdotL;
}

void main()
{
    vec3 baseColorLinear = srgbToLinear(u_BaseColor);
    vec3 albedo = baseColorLinear;

    if (u_UseTexture == 1)
    {
        albedo = texture(u_TextureSampler, v_UV).rgb * baseColorLinear;
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

    vec3 finalColor;

    if (u_UsePBR == 1)
    {
        float metallic = clamp(u_Metallic, 0.0, 1.0);
        float roughness = clamp(u_Roughness, 0.04, 1.0);

        finalColor = albedo * ambientStrength * (1.0 - metallic);
        finalColor += PBRContribution(albedo, N, V, L, vec3(1.0), metallic, roughness);

        for (int i = 0; i < pointLightCount; i++)
        {
            const vec3 toLight = u_PointLightPosition[i] - v_WorldPos;
            const float lightDistance = length(toLight);
            if (lightDistance < 1e-4) continue;

            const vec3 pointL = toLight / lightDistance;
            const float attenuation =
                1.0 / (1.0 + 0.09 * lightDistance + 0.032 * lightDistance * lightDistance);

            finalColor += PBRContribution(
                albedo, N, V, pointL,
                srgbToLinear(u_PointLightColor[i]) * attenuation,
                metallic, roughness
            );
        }
    }
    else
    {
        finalColor = albedo * (ambientStrength + diffuse) + vec3(specular) + pointContribution;
    }

    FragColor = vec4(linearToSrgb(finalColor), 1.0);
}
