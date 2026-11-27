# Anime Skybox & Living Environment Pack

## Contenido del paquete
1. **`anime_skybox_demo.html`** — Demo interactiva autocontenida (Three.js + GLSL). Ábrela directamente en cualquier navegador (Chrome/Edge/Firefox) sin instalar nada para probar el entorno vivo:
   - Skybox procedural estilo anime con sol dinámico, halo Mie y 2 capas de nubes animadas.
   - 3 cordilleras en el horizonte con perspectiva atmosférica y sombras móviles de nubes.
   - Pradera con 14,000 briznas de hierba instanciadas movidas por viento en GPU.
   - Lago reflectante en tiempo real para validar los reflejos del Skybox.
   - Partículas de polen flotando + controles en pantalla (elevación del sol, velocidad de nubes y viento).
2. **`shaders/anime_skybox.vert` y `shaders/anime_skybox.frag`** — Shaders GLSL 4.30 (`#version 430 core`) listos para integrar directamente en tu cubo unitario o domo en **OpenGL / C++**.

## Uniforms requeridos en OpenGL 4.3+
- `uProjection` (`mat4`): Matriz de proyección.
- `uView` (`mat4`): Matriz de vista sin traslación (`mat4(mat3(camera.GetViewMatrix()))`).
- `uTime` (`float`): Tiempo acumulado en segundos.
- `uSunDir` (`vec3`): Vector normalizado hacia el sol (ej. `normalize(vec3(0.4, 0.32, -0.85))`).
- `uCloudSpeed` (`float`): Multiplicador de velocidad de nubes (default `1.0`).
