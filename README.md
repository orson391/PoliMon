# PoliMon

PoliMon is a small C++17 game built on SDL3 and SDL3_image. Its current
rendering backend is SDL_Renderer; OpenGL is not part of the runtime
architecture.

## Build

From the project folder:

```powershell
cmake -S . -B build
cmake --build build --config Debug
```

The `PoliMon` executable will be produced beneath `build/src/main` in the
selected build configuration directory.
