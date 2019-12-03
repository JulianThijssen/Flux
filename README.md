# Flux

Welcome to the Flux Rendering Engine repository. This application is meant to be an exploration into modern rendering techniques used in the game industry. One of the major improvements over its progenitor [Radiant](https://github.com/JulianThijssen/Radiant) are its Physically Based Rendering capabilities.

![alt text](https://user-images.githubusercontent.com/2978176/40970690-9e450978-68bb-11e8-8d3d-60779c756651.png "Latest screenshot")

## Technology
### Implemented
 - Cook-Torrance BRDF
 - Irradiance mapping
 - Prefiltered environment maps + BRDFintegration map
 - Normal mapping
 - HDR Skybox
 - FXAA
 - Gamma Correction
 - HDR + Tone mapping
 - HDR Bloom
 - SSAO
 - Shadow mapping
 - Deferred Rendering
 - Color Grading
 - Fog
 - Crepuscular rays

### Planned
 - Automatic exposure adjustment
 - Spherical Harmonics
 - Omni-directional shadow mapping
 - Light probes
 - PBR Lens flares

## Building
### Requirements
* Git (https://git-scm.com/)
* CMake 3.1+ (https://cmake.org/)
* Visual Studio 2017 or higher (Windows https://visualstudio.microsoft.com/downloads/)

### Setting up
The first step is to pull the repository from GitHub. You can do this in any way you like, but here is one possible way:

1. Create a local folder for the project on your computer
2. Go into the folder and open GitBash or a terminal with git enabled
3. Execute the command `git init` to initialize the Git repository
4. Execute the command `git remote add origin git@github.com:JulianThijssen/Flux.git` to link your local repository to the remote GitHub one.
5. Fetch all branches from the repository using `git fetch -a`
5. Now you can pull in the source files from the `master` branch by executing `git pull origin master`
6. If you get a permission denied message on the last step be sure that you have an SSH-key pair and have added the public one to your GitHub account. See: https://help.github.com/en/github/authenticating-to-github/connecting-to-github-with-ssh

### Building
1. Launch CMake (available here: https://cmake.org/)
2. In the source code field browse to the local folder from the last section (which contains CMakeLists.txt).
3. In the build field copy the source code field but append `/Build` at the end. This will put all files necessary for building in the `Build` folder (it will create it if it doesn't exist).
4. Press `Configure` and select the generator for your IDE of choice with the platform of `x64`. Press Finish to configure the project.
5. If no errors appear in the CMake log on the bottom, press `Generate` to generate the solution for your given IDE.
6. Press `Open Project` to launch the IDE and the project.

# Compilation

## Windows
1. At the top of Visual Studio set the build mode (where it says `Debug`) to `Release`.
2. Right click the project `TestProject` in the Solution Explorer and select Set as StartUp Project.
3. Right click the project `Flux` and press `Build`, if that succeeds, right click the `TestProject` project and press `Build`.
4. If all is well both builds should succeed and if you run the project with Ctrl+F5 it will launch `TestProject.exe`.

## Linux | Mac
This depends on your IDE / compiler. Follow their instructions for compiling source code. The code is untested on these platforms.

# Dependencies
Editor
 - Assimp 3.3.1

Engine
 - GLFW 3.2.1

## License
The source code and auxiliary files fall under a GPL License, which you can read about in LICENSE.md.
