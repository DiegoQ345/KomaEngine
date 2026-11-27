#version 460 core

in vec3 vWorldDir;
out vec4 FragColor;

uniform float uTime;
uniform vec3 uSunDir;
uniform float uCloudSpeed;

float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453123);
}

float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(
        mix(hash(i + vec2(0.0, 0.0)), hash(i + vec2(1.0, 0.0)), u.x),
        mix(hash(i + vec2(0.0, 1.0)), hash(i + vec2(1.0, 1.0)), u.x),
        u.y
    );
}

float fbm(vec2 p) {
    float v = 0.0;
    float a = 0.5;
    mat2 rot = mat2(0.8, 0.6, -0.6, 0.8);
    for (int i = 0; i < 5; i++) {
        v += a * noise(p);
        p = rot * p * 2.03;
        a *= 0.5;
    }
    return v;
}

float mountainRidge(float x, float freq, float seed) {
    float n = sin(x * freq + seed) * 0.5 + 0.5;
    n += (noise(vec2(x * freq * 1.7, seed)) - 0.5) * 0.65;
    n += (noise(vec2(x * freq * 4.2, seed + 3.1)) - 0.5) * 0.22;
    return clamp(n, 0.0, 1.5);
}

vec3 linearToSrgb(vec3 color) {
    return mix(color * 12.92,
               1.055 * pow(color, vec3(1.0 / 2.4)) - 0.055,
               step(vec3(0.0031308), color));
}

uniform bool u_ToneMappingEnabled;

vec3 reinhardToneMapping(vec3 color)
{
    return color / (vec3(1.0) + color);
}

void main() {
    vec3 dir = normalize(vWorldDir);
    float y = dir.y;
    vec3 sunDir = normalize(uSunDir);

    // 1. Anime Sky Gradient (Zenith -> Mid -> Horizon)
    vec3 zenithCol  = vec3(0.09, 0.36, 0.82);
    vec3 midSkyCol  = vec3(0.26, 0.65, 0.96);
    vec3 horizonCol = vec3(0.72, 0.93, 1.00);

    float t1 = smoothstep(0.0, 0.35, max(y, 0.0));
    float t2 = smoothstep(0.25, 0.92, max(y, 0.0));
    vec3 col = mix(horizonCol, midSkyCol, t1);
    col = mix(col, zenithCol, t2);

    // 2. Sun Disc & Mie Halo
    float sunCos = max(dot(dir, sunDir), 0.0);
    float sunHalo = pow(sunCos, 10.0) * 0.35 + pow(sunCos, 90.0) * 0.65;
    float sunDisc = smoothstep(0.9982, 0.9993, sunCos);
    col += vec3(1.0, 0.93, 0.78) * sunHalo;
    col = mix(col, vec3(1.45, 1.40, 1.25), sunDisc);

    // 3. Layered Animated Cumulus & Cirrus Clouds
    if (y > 0.015) {
        vec2 uv = dir.xz / (y + 0.16);
        float t = uTime * 0.035 * uCloudSpeed;

        float cirrus = fbm(uv * 1.4 + vec2(t * 0.6, t * 0.2));
        float cirrusMask = smoothstep(0.56, 0.82, cirrus) * smoothstep(0.02, 0.25, y);
        col = mix(col, vec3(0.95, 0.98, 1.0), cirrusMask * 0.35);

        vec2 cuv = uv * 0.85 + vec2(t, t * 0.35);
        float cBase = fbm(cuv);
        float cDetail = fbm(cuv * 2.4 - vec2(t * 0.5, 0.0));
        float cloudDensity = cBase * 0.78 + cDetail * 0.28;

        float cloudAlpha = smoothstep(0.53, 0.59, cloudDensity) * smoothstep(0.02, 0.16, y);
        float cSunSample = fbm(cuv + sunDir.xz * 0.08);
        float litFactor = smoothstep(0.46, 0.64, cSunSample);

        vec3 cloudShadow = mix(vec3(0.56, 0.68, 0.88), vec3(0.75, 0.84, 0.96), litFactor);
        vec3 cloudLight  = vec3(1.02, 1.01, 0.99) + vec3(0.18, 0.14, 0.06) * pow(sunCos, 4.0);
        vec3 cloudCol = mix(cloudShadow, cloudLight, smoothstep(0.35, 0.72, litFactor));

        col = mix(col, cloudCol, cloudAlpha * 0.94);
    }

    // 4. Three Mountain Depth Layers with Atmospheric Perspective
    float azimuth = atan(dir.z, dir.x);

    float m1 = mountainRidge(azimuth, 3.0, 1.2) * 0.11 + 0.01;
    if (y < m1 && y > -0.05) {
        float snow = smoothstep(m1 - 0.025, m1, y);
        vec3 m1Col = mix(vec3(0.50, 0.74, 0.95), vec3(0.84, 0.94, 1.0), snow * 0.65);
        col = mix(col, m1Col, 0.88);
    }

    float m2 = mountainRidge(azimuth, 4.5, 4.7) * 0.075 + 0.002;
    if (y < m2 && y > -0.05) {
        float cloudShadow = smoothstep(0.42, 0.65, noise(vec2(azimuth * 5.0 + uTime * 0.05 * uCloudSpeed, 2.0)));
        col = mix(vec3(0.28, 0.52, 0.78), vec3(0.38, 0.64, 0.86), cloudShadow);
    }

    float m3 = mountainRidge(azimuth, 7.0, 8.4) * 0.042 - 0.004;
    if (y < m3 && y > -0.05) {
        col = vec3(0.18, 0.44, 0.56);
    }

    // 5. Horizon Mist
    float mist = 1.0 - smoothstep(-0.01, 0.045, abs(y));
    col = mix(col, horizonCol, mist * 0.45);

    if (u_ToneMappingEnabled) {
        vec3 mappedColor = reinhardToneMapping(col);
        FragColor = vec4(linearToSrgb(mappedColor), 1.0);
    }
    else
    {
        FragColor = vec4(linearToSrgb(col), 1.0);
    }
}
