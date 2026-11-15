# AGENTS.md — KomaEngine

## Reglas generales

- Trabaja en tareas pequeñas y delimitadas; no hagas más de lo pedido.
- Inspecciona la estructura y los archivos relevantes antes de modificar nada.
- Evita cambios no solicitados; si algo está fuera de alcance, repórtalo y espera.
- Explica brevemente las decisiones técnicas (qué, por qué).
- No introduzcas dependencias nuevas sin autorización explícita.

## Arquitectura

- Respeta la separación de módulos: `Platform` (ventana, contexto, eventos), `Renderer` (backend gráfico; OpenGL hoy, Vulkan posible a futuro), `Core`, `App` (ejecutable de prueba), `Tests`.
- Evita dependencias circulares entre módulos.
- No mezcles lógica de aplicación con responsabilidades del motor.
- No acoples sistemas independientes directamente a OpenGL sin justificación (p. ej. matemáticas con GLM o conceptos de escena).
- Los módulos generales no deben incluir cabeceras de OpenGL/GLAD; Platform no expone GLAD en sus interfaces.

## Compilación y validación

- Usa los CMake Presets existentes: `windows-debug` y `windows-release` (`cmake --preset …` / `cmake --build --preset …`).
- Compila Debug y Release cuando los cambios puedan afectar a ambos.
- No des una tarea por terminada solo porque compila: ejecuta lo que corresponda (App, y las pruebas cuando existan).
- Informa errores y advertencias sin ocultarlos ni silenciarlos.

## Límites de actuación

- No cambies versiones de C++ (C++20), OpenGL (4.6 Core), GLFW, GLAD o GLM sin autorización.
- No reorganices carpetas ni renombres módulos por iniciativa propia.
- No elimines archivos o funcionalidades existentes sin justificación y autorización.
- No crees sistemas futuros (Renderer completo, Scene, Physics, etc.) antes de que una tarea los requiera.
- No hagas commits salvo que se pida explícitamente.

## Informe final

Al cerrar cada tarea entrega:

1. Archivos modificados.
2. Cambios realizados.
3. Motivos técnicos.
4. Compilaciones y pruebas ejecutadas (Debug/Release, App).
5. Problemas encontrados (errores y advertencias).
6. Pendientes.

Entorno de referencia: MSYS2 UCRT64 en `G:\msys` (`G:\msys\ucrt64\bin` en el `PATH`, necesario para `glfw3.dll`), Ninja, presets `windows-debug` / `windows-release`.
