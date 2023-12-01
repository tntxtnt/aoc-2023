# 2023



## How to configure with CMake

Use CMake presets, list all avaliable presets:
```
cmake --list-presets
```
Then choose one, for example on Windows:
```
cmake --preset vs2022-default
```
On Windows, if you're not using `vs2022-...` presets, you'll need to make sure to have `g++`, `clang++`, `clang-cl`, or `cl` avaliable on your console before calling CMake configure.


## How to use external libraries

### Using built-in libraries
Make sure to install needed libraries before running CMake configure.

### Using vcpkg

#### Clone vcpkg
- From git submodule:
  ```
  git submodule add https://github.com/microsoft/vcpkg vcpkg
  ```
  Then config with a preset that ends with `-vcpkg-submodule`
- From local vcpkg: add path to your local vcpkg folder to environment variable `VCPKG_ROOT`, for example in Powershell: `[Environment]::SetEnvironmentVariable('VCPKG_ROOT', 'D:/vcpkg', 'User')`. Then config with a preset that ends with `-vcpkg-local`

#### Add an external library
- Add the library's names to `vcpkg.json` `"dependencies": [ ... ]"`
- Run `cmake --preset ...` again. `vcpkg` will build and installed those libraries for you. In addition to that, `vcpkg` will tell you how to find and link those libraries with CMake.
- Add `find_package(...)` and `target_link_libraries(...)` to `CMakeList.txt`


## How to debug with VS Code

You only need Microsoft C/C++ extension (cpptools).

- Intenllisense profiles are stored in `.vscode/c_cpp_properties.json`
- Build commands are store in `.vscode/tasks.json`
- Debug profiles are stored in `.vscode/launch.json`
- Open `.vscode/2023.code-workspace` with VS Code

### Linux

It works out of the box, nothing to do.

### Windows

- For MinGW presets: you need to set `MINGW_ROOT` environment var to `/mingw64` folder (or `/mingw32`). For example `D:\msys64\mingw64`

- For MSVC presets: you need to open VS Code with `vcvars64.bat` (or `vcvars32.bat`) enabled. For example from powershell terminal run:

  ```
  cmd
  "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
  code .vscode/2023.code-workspace
  ```
