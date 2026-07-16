<p align="center">
<a href="https://github.com/pavelc4/klein-bottle-viz"><img src="https://img.shields.io/badge/C%2B%2B-00599C?style=for-the-badge&logo=cplusplus&logoColor=white" alt="C++17"></a>
<a href="LICENSE"><img src="https://img.shields.io/badge/MIT-white?style=for-the-badge&logo=opensourceinitiative&logoColor=white&label=License&labelColor=222" alt="License"></a>
</p>

## About klein-bottle-viz

**klein-bottle-viz** is an interactive 3D Klein bottle visualizer built with C++ , SDL2, and legacy OpenGL. Explore the topology of a non-orientable surface — rotate, zoom, slice through it, and watch objects travel along its surface to see what "no inside or outside" really means.


## Install

### Build from source

```bash
git clone https://github.com/pavelc4/klein-bottle-viz.git
cd klein-bottle-viz
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/klein-bottle-viz
```

Requires: C++ compiler, SDL2, OpenGL

```bash
# Debian/Ubuntu
sudo apt install libsdl2-dev libgl-dev libglu1-mesa-dev

# Arch
sudo pacman -S sdl2 glu

# Fedora
sudo dnf install SDL2-devel mesa-libGL-devel mesa-libGLU-devel
```


## License

klein-bottle-viz is open-sourced software licensed under the [MIT License](LICENSE).
