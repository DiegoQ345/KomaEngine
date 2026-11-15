# KomaEngine

Motor gráfico 3D en C++20 + OpenGL. Primera versión funcional en Windows.

## Requisitos

- Windows con terminal CMD
- MSYS2 UCRT64 en `G:\msys` (compilador en `G:\msys\ucrt64\bin`)
  - GCC/G++ UCRT64
  - CMake y Ninja (incluidos en MSYS2)
  - GLFW y GLM instalados vía MSYS2:
    ```cmd
    pacman -S mingw-w64-ucrt-x86_64-glfw mingw-w64-ucrt-x86_64-glm
    ```
- `G:\msys\ucrt64\bin` debe estar en el `PATH` (necesario para `glfw3.dll` en tiempo de ejecución)

GLAD 2 (gl:core=4.6) está vendorizado en `Dependencies/GLAD/` y no requiere instalación.

## Configuración

```cmd
cmake -S . -B build -G Ninja
```

CMake localiza GLFW (`glfw3 CONFIG`) y GLM (`glm CONFIG`) automáticamente a través del `PATH`. Si no los encuentra, indica el prefijo explícitamente:

```cmd
cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH=G:\msys\ucrt64
```

## Compilación

```cmd
cmake --build build
```

Verificación del entorno:

```cmd
g++ --version
cmake --version
ninja --version
```

## Ejecución

```cmd
build\Sandbox\Sandbox.exe
```

Abre una ventana de 1280 × 720 con un contexto OpenGL 4.6 Core, imprime por consola la versión de OpenGL y el nombre de la GPU, y limpia la pantalla con un color azul oscuro. La ventana permanece abierta hasta cerrarla.

## Estructura

```text
KomaEngine/
├── CMakeLists.txt          # Configuración raíz (C99 para C, C++20 para C++)
├── Engine/                 # Motor
│   ├── Core/
│   ├── Platform/
│   │   └── Window/         # Ventana GLFW (Window.hpp / Window.cpp)
│   └── Renderer/
│       └── OpenGL/
├── Dependencies/
│   └── GLAD/               # GLAD 2 generado para gl:core=4.6
│       ├── include/
│       └── src/gl.c
└── Sandbox/                # Ejecutable de prueba
    └── main.cpp
```

## Arquitectura

- La capa `Platform` (ventana, eventos, contexto) está separada de la capa `Renderer` (OpenGL).
- Los módulos generales del motor no dependen directamente de OpenGL.
- `Window` expone solo GLFW; GLAD se inicializa en el ejecutable una vez el contexto está activo.
