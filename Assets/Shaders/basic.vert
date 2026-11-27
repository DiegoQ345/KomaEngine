#version 460 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_UV;
layout(location = 2) in vec3 a_Normal;
layout(location = 3) in vec4 a_Tangent;

uniform mat4 u_ViewProjection;
uniform mat4 u_Model;
uniform mat4 u_LightSpaceMatrix;

out vec2 v_UV;
out vec3 v_WorldPos;
out vec3 v_Normal;
out vec3 v_Tangent;
out vec3 v_Bitangent;
out vec4 v_PosLightSpace;

void main()
{
    vec4 worldPosition = u_Model * vec4(a_Position, 1.0);

    v_WorldPos = worldPosition.xyz;
    v_UV = a_UV;

    mat3 model3 = mat3(u_Model);
    vec3 normal = normalize(transpose(inverse(model3)) * a_Normal);
    vec3 tangent = normalize(model3 * a_Tangent.xyz);

    tangent = normalize(tangent - dot(tangent, normal) * normal);

    float determinantSign = sign(determinant(model3));
    vec3 bitangent = normalize(cross(normal, tangent) * a_Tangent.w * determinantSign);

    v_Normal = normal;
    v_Tangent = tangent;
    v_Bitangent = bitangent;
    v_PosLightSpace = u_LightSpaceMatrix * worldPosition;

    gl_Position = u_ViewProjection * worldPosition;
}
