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

After these installations, you can install the VS Code extensions by opening VS Code, then clicking the four squares on the left side of the screen (the extension manager) and searching for those extensions. After the extensions are installed, you will probably need to reopen vscode.

Once everything is installed, download this repository (mahi-fes) to your computer. You can do from [the main repository page](https://github.com/mahilab/mahi-fes) by clicking the `code` section, then `download zip`. Separately, open Visual Studio Code on your computer, and go to `file -> open folder` and open the mahi-fes-template folder, which is inside `mahi-fes`. You should probably get a popup when opening the folder that says something like "configure folder?" or something. Click on that, and it will ask you what kit (compiler) you want to use. If nothing shows up from the dropdown, click `scan for kits`. If something does show up, click `Visual Studio Community 2019 Release - amd64`. Make sure on the bottm of the screen, one of the boes says `CMake: Release` and not `CMake: Debug`.

Now, click Build on the bottom of the screen (or F7)! A bunch of information should show up in the terminal, and it should say `build finished with exit code 0` when it is done. Find you executable in build/release and try to run it!
