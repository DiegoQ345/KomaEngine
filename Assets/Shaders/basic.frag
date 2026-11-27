#version 460 core

in vec2 v_UV;
in vec3 v_WorldPos;
in vec3 v_Normal;
in vec3 v_Tangent;
in vec3 v_Bitangent;
in vec4 v_PosLightSpace;

uniform sampler2D u_TextureSampler;
uniform vec3 u_BaseColor;
uniform int u_UseTexture;

uniform vec3 u_CameraPosition;
uniform vec3 u_LightDirection;
uniform vec3 u_AmbientColor;

uniform int u_UsePBR;
uniform float u_Metallic;
uniform float u_Roughness;

uniform int u_UseNormalMap;
uniform sampler2D u_NormalSampler;

uniform sampler2DShadow u_ShadowMap;
uniform mat4 u_LightSpaceMatrix;
uniform int u_ShadowEnabled;
uniform float u_ShadowBias;

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

float ComputeShadow(vec3 N, vec3 L)
{
    if (u_ShadowEnabled == 0) return 1.0;

    vec3 projected = v_PosLightSpace.xyz / v_PosLightSpace.w;
    projected = projected * 0.5 + 0.5;

    if (projected.z < 0.0 || projected.z > 1.0 ||
        projected.x < 0.0 || projected.x > 1.0 ||
        projected.y < 0.0 || projected.y > 1.0)
    {
        return 1.0;
    }

    float bias = max(u_ShadowBias * 0.5, u_ShadowBias * (1.0 - max(dot(N, L), 0.0)));
    return texture(u_ShadowMap, vec3(projected.xy, projected.z - bias));
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

uniform bool u_ToneMappingEnabled;
uniform bool u_UnlitTest;

vec3 reinhardToneMapping(vec3 color)
{
    return color / (vec3(1.0) + color);
}

void main()
{
    vec3 albedo;

    if (u_UnlitTest) {
        albedo = u_BaseColor;
    } else {
        vec3 baseColorLinear = srgbToLinear(u_BaseColor);
        albedo = baseColorLinear;
        if (u_UseTexture == 1) {
            albedo = texture(u_TextureSampler, v_UV).rgb * baseColorLinear;
        }
    }

    if (u_UnlitTest) {
        vec3 color = albedo;
        if (u_ToneMappingEnabled) {
            FragColor = vec4(linearToSrgb(reinhardToneMapping(color)), 1.0);
        } else {
            FragColor = vec4(linearToSrgb(color), 1.0);
        }
        return;
    }

    const float specularStrength = 0.8;
    const float shininess = 16.0;

    vec3 N = normalize(v_Normal);
    if (u_UseNormalMap == 1)
    {
        vec3 tangentSpaceNormal = texture(u_NormalSampler, v_UV).rgb * 2.0 - 1.0;
        tangentSpaceNormal = normalize(tangentSpaceNormal);

        vec3 T = normalize(v_Tangent - v_Normal * dot(v_Normal, v_Tangent));
        vec3 B = normalize(v_Bitangent);
        mat3 TBN = mat3(T, B, v_Normal);

        N = normalize(TBN * tangentSpaceNormal);
    }

    vec3 L = normalize(u_LightDirection);
    float shadow = ComputeShadow(N, L);
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

        finalColor = albedo * u_AmbientColor * (1.0 - metallic);
        finalColor += PBRContribution(
            albedo, N, V, L, vec3(shadow), metallic, roughness
        );

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
        finalColor = albedo * (u_AmbientColor + diffuse * shadow)
                   + vec3(specular * shadow) + pointContribution;
    }

    if (u_ToneMappingEnabled) {
        vec3 mappedColor = reinhardToneMapping(finalColor);
        FragColor = vec4(linearToSrgb(mappedColor), 1.0);
    }
    else
    {
        FragColor = vec4(linearToSrgb(finalColor), 1.0);
    }
}
