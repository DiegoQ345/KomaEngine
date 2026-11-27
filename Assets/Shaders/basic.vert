#version 460 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_UV;
layout(location = 2) in vec3 a_Normal;

uniform mat4 u_ViewProjection;
uniform mat4 u_Model;

out vec2 v_UV;
out vec3 v_WorldPos;
out vec3 v_Normal;

void main()
{
    vec4 worldPosition = u_Model * vec4(a_Position, 1.0);

    v_WorldPos = worldPosition.xyz;
    v_Normal = normalize(transpose(inverse(mat3(u_Model))) * a_Normal);
    v_UV = a_UV;

    gl_Position = u_ViewProjection * worldPosition;
}
