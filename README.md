# Engine 12

A 3D game engine that aims to be convenient and lightweight, using OpenGL for rendering. We are also aiming for an OS-agnostic development experience. Meaning, regardless if you are on Windows, Linux, ~~or even MacOS~~, you can contribute to the project.

## Getting Started
GLM and GLFW is already taken care of by the CMake script `CMakeLists.txt`, so
there is no need to download and install manually. OpenGL is automatically
installed in most (if not all) modern machines.

Currently, this project uses the following versions:
- GLM Git Tag 0.9.9.8
- GLFW Git Tag 3.3.8

### Windows
The preferred IDEs to develop in Windows would be CLion. We will
discuss how to get you started in both.

#### CLion
When you open up CLion, click on "Get from VCS", pull the repository located
in GitHub. It should automatically open the project. Afterwards, find the
`CMakeLists.txt` file in the root directory and a notification should pop up on
the top right corner prompting you to press "fix". Press it and select "Load
Project". Doing so will immediately begin the CMake script and build the
project. Once it's finished, the green "play" button towards the top right
should be clickable. Click on it, and the main executable `TBDEngine` should
start.

### Unix/Linux
Clone the repository and `cd` into the root directory of the project.
If you are using WSL with Ubuntu/Debian, run the following bash commands:
```bash
sudo apt install libxrandr-dev
sudo apt install libxinerama-dev
sudo apt install libxcursor-dev
sudo apt install libxi-dev
```
Afterwards, run the following bash commands:
```bash
mkdir build
cd build
cmake ..
```
Doing this configures CMake to build your project according to the
specifications in the `CMakeLists.txt` located in the root directory. Once CMake
is finished with its configurations, run the following command:
```bash
make -j8
```
This will compile all the files into binaries and finally generate the
main executable `TBDEngine`. To start the executable, run the following
command:
```bash
./TBDEngine
```

## Contributors

**Current**
- Ilter Ulutas
- Arkan A
- Roei Ben Porat
- Ethan Nguyen
- Calvin Fun
- Owen House
- Camryn Graham
- Josh Eury
- Raniv Gupta
- Kate Serem
- abhilaasya
- ana_kaur
- bangzhe
- aryav123
- dillil
- naomismith3328
- ana_kaur
- bangzhe
- aryav123
- dillil
- naomismith3328
- Orlando Haye
- priyanka_27916
- daddyvi


**Previous**
- kickdapie
- alejandro-d-garza
- Pooji20-dot
- abzf227
- uzairak12
