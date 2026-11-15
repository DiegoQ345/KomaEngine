# KomaEngine

Motor gráfico 3D en C++20 + OpenGL. Primera versión funcional en Windows.

## Tecnologías

- C++20 / C99 (GLAD)
- CMake + Ninja
- GCC/G++ de MSYS2 UCRT64 (`G:\msys`)
- OpenGL 4.6 Core — GLAD 2 (gl:core=4.6), vendorizado en `Dependencies/GLAD/`
- GLFW (ventanas y contexto) y GLM (matemáticas), vía MSYS2 UCRT64

## Requisitos

- Windows con terminal CMD
- MSYS2 UCRT64 en `G:\msys` (`G:\msys\ucrt64\bin` en el `PATH`, necesario para `glfw3.dll`)
- Herramientas MSYS2 UCRT64: GCC, CMake, Ninja
- GLFW y GLM:
  ```cmd
  pacman -S mingw-w64-ucrt-x86_64-glfw mingw-w64-ucrt-x86_64-glm
  ```

## Configuración y compilación

```cmd
cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH=G:/msys/ucrt64
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
build\App\App.exe
```

Abre una ventana de 1280 × 720 con contexto OpenGL 4.6 Core, imprime por consola la versión de OpenGL y la GPU, y limpia la pantalla con azul oscuro. La ventana permanece abierta hasta cerrarla.

## Estructura

```text
KomaEngine/
├── CMakeLists.txt          # Config raíz (C99 / C++20, Ninja)
├── Engine/                 # Motor (biblioteca estática)
│   ├── Core/               # Nucleo (pendiente)
│   ├── Platform/
│   │   └── Window/         # Ventana GLFW (Window.hpp / Window.cpp)
│   └── Renderer/
│       └── OpenGL/         # Backend OpenGL (pendiente)
├── App/                    # Ejecutable de prueba (KomaEngine App)
│   └── main.cpp
├── Assets/                 # Models, Textures, Shaders, Materials
├── Tests/                  # Tests (aun sin frameworks configurados)
├── Docs/                   # Architecture, Development
└── Dependencies/
    └── GLAD/               # GLAD 2, gl:core=4.6
```

## Arquitectura

- La capa `Platform` (ventana, eventos, contexto) está separada de la capa `Renderer`.
- El motor no depende directamente de OpenGL fuera del backend; GLAD se inicializa solo cuando el contexto está activo.
- Estructura preparada para un futuro backend Vulkan en `Renderer/`.

## Desarrollo

Ver `Docs/Development/` y `Docs/Architecture/`. Las pruebas irán en `Tests/` (activables con `-DKOMA_BUILD_TESTS=ON`).
