# Development

Guía breve del entorno y las dependencias de KomaEngine.

## Entorno

- Windows + terminal CMD.
- MSYS2 UCRT64 en `G:\msys`; toolchain en `G:\msys\ucrt64\bin` (GCC/G++, CMake, Ninja).
- `G:\msys\ucrt64\bin` debe estar en el `PATH`: `App.exe` carga **`glfw3.dll`** en tiempo de ejecución; Windows debe poder encontrarla (ruta de MSYS2 o copia junto al ejecutable).

## Compilación y ejecución

```cmd
cmake --preset windows-debug
cmake --build --preset windows-debug
build\debug\App\App.exe

cmake --preset windows-release
cmake --build --preset windows-release
build\release\App\App.exe
```

## Dependencias

- **GLFW** — gestión de ventana y contexto OpenGL. Se enlaza como `glfw` (DLL de MSYS2, `GLFW_DLL` definido por el target importado).
- **GLAD 2** — carga de las funciones OpenGL. Cargador propio (`gladLoadGL`), generado para `gl:core=4.6`, compilado desde `Dependencies/GLAD/src/gl.c` (C99) como `libglad.a`.
- **GLM** — vectores, matrices y transformaciones 3D. Header-only (`glm::glm`).

### Orden de inicialización

1. `glfwInit()`
2. Crear la ventana GLFW (contexto OpenGL 4.6 Core)
3. `glfwMakeContextCurrent(ventana)` — activar el contexto
4. `gladLoadGL(glfwGetProcAddress)` — solo entonces GLAD puede cargar las funciones OpenGL

### Integración con CMake

- `Engine/CMakeLists.txt`: `find_package(glfw3 CONFIG REQUIRED)`, `find_package(glm CONFIG REQUIRED)`; target `glad` propio enlazado PRIVATE; `Engine` propaga `glfw` y `glm::glm` PUBLIC (cabeceras de Window.hpp exponen tipos GLFW).
- `App/CMakeLists.txt`: `App` enlaza PRIVATE `Engine`, `glad` y `glfw` (usa GLAD y GLFW directamente en `main.cpp`).
- Presets en `CMakePresets.json` (windows-debug / windows-release, Ninja + GCC UCRT64, `CMAKE_PREFIX_PATH=G:/msys/ucrt64`).
