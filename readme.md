# DEUS
This is a template for a Google ANGLE OpenGL ES 3.2 (Windows) application.

Currently, only the rendering part is implemented. In future, this template will include convenient
C++ wrappers for commonly used C libraries, examples on how to use them in OpenGL ES 3.2 and input
handling (mouse, keyboard, touch).

A port to Android, OS X and Linux will be provided in later stages of the development.

### Requirements
* 64-bit Windows 10 (should work on Windows 7 and later)
* Visual Studio 2015 Update 2
* CMake >= 3.2
* Pandoc
* 7-zip

If you want to support Windows 7 and 8, you will require the `d3dcompiler_47.dll` file from Microsoft
either installed on the target system or placed into the application directory.

### Customizations
Please change the project name, the `PRODUCT_NAME` and the `COPYING_TEXT` variables at the top of
the [CMakeLists.txt](CMakeLists.txt) file before using this template.

It is also a good idea to update the license in [doc/license.md](doc/license.md) and the release
notes in [doc/release.md](doc/release.md) before publishing.

### Dependencies
The Google ANGLE library must be built from source code.

1. Install [depot_tools](https://dev.chromium.org/developers/how-tos/install-depot-tools) from Google.
2. Set the required environment variables.
3. Execute the script **third_party/angle/build.bat** (or run the commands manually).

This environment variables were used during testing:

```
GYP_GENERATORS=msvs
GYP_MSVS_VERSION=2015
GYP_DEFINES=windows_sdk_path=C:\Program Files (x86)\Windows Kits\10
```

Other libraries are provided in binary form for convenience. You can rebuild them with the `build.bat`
scripts in their directories.

### Build the Template
Execute the script **solution.bat** (or run the commands manually).

### Copyright & License
Copyright (c) 2016 Alexej Harm. All rights reserved.

This template is released under the [Boost Software License](http://www.boost.org/LICENSE_1_0.txt).

### Third Party Licenses
For libjpeg-turbo licenses, see [third_party/jpeg/license.md](third_party/jpeg/license.md).
For all other third party licenses, see [doc/license.md](doc/license.md).
