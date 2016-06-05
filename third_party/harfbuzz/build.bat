@echo off
rem Initialize the VS2015 command line utilities.
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" amd64

rem Enter the script directory.
pushd %~dp0

rem Determine the project name.
for /f "tokens=2 delims=( " %%i in ('findstr /c:"project(" CMakeLists.txt') do (
  set project=%%i
)

rem Create and enter the build directory.
if not exist build (
  mkdir build
)
pushd build

rem Generate and build the solution.
cmake -G "Visual Studio 14 2015 Win64" -DCMAKE_INSTALL_PREFIX:PATH=.. ..
if %errorlevel% == 0 (
  call devenv "%project%.sln" /build "Debug|x64" /project INSTALL
  call devenv "%project%.sln" /build "Release|x64" /project INSTALL
) else (
  pause
)

rem Leave the build directory.
popd

rem Leave the script directory.
popd
