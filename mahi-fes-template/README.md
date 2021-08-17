Things you need to run this:
 - cmake
 - [Build Tools for Visual Studio 2019](https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2019)
 - ninja
 - vscode
 - vscode extensions:
   - C/C++
   - CMake
   - CMake Tools

[These can be installed using the chocolatey installer](https://chocolatey.org/)
``` shell
> choco install cmake --installargs 'ADD_CMAKE_TO_PATH=System'
> choco install visualstudio2019buildtools
> choco install vscode
> choco install ninja
> choco install git
```