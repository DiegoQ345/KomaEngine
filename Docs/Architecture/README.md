# Architecture

Decisiones de arquitectura actuales de KomaEngine (sin sistemas aún implementados).

## Capas

- **Platform/** — ventanas, eventos y gestión del contexto (hoy `Platform/Window`, sobre GLFW). Independiente del pipeline de renderizado.
- **Renderer/** — (pendiente) backends gráficos. Preparado para OpenGL en `Renderer/OpenGL` y un futuro backend Vulkan; los sistemas generales no dependerán directamente de OpenGL.
- **Core/** — (pendiente) núcleo del motor.
- **App/** — ejecutable de prueba que usa el motor.

## Dependencias gráficas

- **GLFW** en Platform solamente: crea la ventana y el contexto OpenGL 4.6 Core, eventos y buffers (`SwapBuffers`).
- **GLAD 2** en la frontera Renderer/App: carga los punteros de función de OpenGL; requiere contexto activo. Solo se inicializa tras `glfwMakeContextCurrent`.
- **GLM** para el código que trabaje con matemáticas 3D (Renderer/App en el futuro): tipos `glm::vec*` y `glm::mat*`.

Flujo de arranque: GLFW init → crear ventana/contexto 4.6 Core → activar contexto → GLAD → bucle principal (clear + SwapBuffers).

El motor enlaza OpenGL solo a través del backend; Platform no incluye GLAD en sus cabeceras.
