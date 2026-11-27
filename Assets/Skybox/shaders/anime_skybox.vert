#version 430 core
layout (location = 0) in vec3 aPos;

out vec3 vWorldDir;

uniform mat4 uProjection;
uniform mat4 uView; // Pass mat4(mat3(view)) so skybox stays centered on camera

void main() {
    vWorldDir = aPos;
    vec4 pos = uProjection * uView * vec4(aPos, 1.0);
    gl_Position = pos.xyww; // Force depth = 1.0 (far plane)
}
