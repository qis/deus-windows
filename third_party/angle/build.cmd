@echo off

rem Check the environment variables.
if NOT "%GYP_GENERATORS%" == "msvs" (
  echo Set the GYP_GENERATORS environment variable to "msvs".
  pause
  exit 1
)

if NOT "%GYP_MSVS_VERSION%" == "2015" (
  echo Set the GYP_MSVS_VERSION environment variable to "2015".
  pause
  exit 1
)

if NOT "%GYP_DEFINES%" == "windows_sdk_path=C:\Program Files (x86)\Windows Kits\10" (
  echo Set the GYP_DEFINES environment variable to "windows_sdk_path=C:\Program Files (x86)\Windows Kits\10".
  pause
  exit 1
)

rem Check the dependencies.
where gclient 2>NUL >NUL
if not %errorlevel% == 0 (
  echo Install depot_tools from Google and add the directory to the Path environment variable.
  echo https://dev.chromium.org/developers/how-tos/install-depot-tools
  pause
  exit 1
)

rem Initialize the VS2015 command line utilities.
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" amd64

rem Enter the script directory.
pushd %~dp0

rem Retrieve the source code and create the solution.
if not exist src (
  call git clone -b chromium/2757 https://github.com/google/angle src
  pushd src
  call python scripts/bootstrap.py
  call gclient sync
  popd
)

rem Build the debug libraries.
call devenv "src\build\ANGLE.sln" /build "Debug|x64" /project libEGL

if not exist "src\build\Debug_x64\libEGL.dll" (
  echo Could not build libEGL in debug mode.
  pause
  exit 1
)

rem Build the release libraries.
call devenv "src\build\ANGLE.sln" /build "Release|x64" /project libEGL

if not exist "src\build\Debug_x64\libEGL.dll" (
  echo Could not build libEGL in release mode.
  pause
  exit 1
)

rem Install the files and directories.
rmdir /q /s include lib

xcopy src\include include /s /e /i

xcopy src\build\Debug_x64\libGLESv2.dll lib\debug\
xcopy src\build\Debug_x64\libEGL.dll lib\debug\

xcopy src\build\Debug_x64\lib\libANGLE.lib lib\debug\
xcopy src\build\Debug_x64\lib\libGLESv2.lib lib\debug\
xcopy src\build\Debug_x64\lib\libEGL.lib lib\debug\

xcopy src\build\Release_x64\libGLESv2.dll lib\release\
xcopy src\build\Release_x64\libEGL.dll lib\release\

xcopy src\build\Release_x64\lib\libANGLE.lib lib\release\
xcopy src\build\Release_x64\lib\libGLESv2.lib lib\release\
xcopy src\build\Release_x64\lib\libEGL.lib lib\release\

rem Leave the script directory.
popd
