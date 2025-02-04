# dxPBR
This is a personal project to learn and test D3D11.


## Preview


https://github.com/user-attachments/assets/39384fb4-d579-4647-85ff-e73ab52e847a


## Features
- Physically Based Rendering(IBL only)
- Image-Based Lighting
  - HDRI cubemap
- GUI panel based on imgui
- Image loading using stb-image
- Model loading using assimp
## Getting Started

Codes are written via Visual Studio 2022.
1. Install Windows SDK. (this project set Windows SDK 10.0)
1. Install vcpkg:
```
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg; .\bootstrap-vcpkg.bat
```
2. Install Dependencies using vcpkg:
```
vcpkg install imgui[win32-binding,dx11-binding]:x64-windows
vcpkg install assimp:x64-windows
vcpkg install stb:x64-windows
vcpkg install directxtk:x64-windows
vcpkg integrate install
```

### Building the Projects
1. clone this repository:
`git clone https://github.com/ih2116501/dxPBR.git`
or download ZIP file.

2. open `dxPBR.sln` to build and run the project.

### Dependencies
- [assimp](https://github.com/assimp/assimp)
- [imgui](https://github.com/ocornut/imgui)
- [stb_image](https://github.com/nothings/stb)
- [DirectXTK](https://github.com/microsoft/DirectXTK)

Install all dependencies through vcpkg.

## License
This code is licensed under the MIT License.


## Acknowledgments

- metal texture by [CC0-textures.com](https://cc0-textures.com/t/cc0t-metal-004)
- cubemap texture by [polyhaven.com](https://polyhaven.com/a/empty_play_room)
- damaged helmet 3D model by [leonardo-carrion](https://sketchfab.com/3d-models/battle-damaged-sci-fi-helmet-pbr-b81008d513954189a063ff901f7abfe4)
