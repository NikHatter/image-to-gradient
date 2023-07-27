# image-to-gradient

Demo project.
Creates linear gradient approximation from and input image.

## Dependencies

Vcpkg, Boost::gil, libpng, libjpeg.
For visual application Qt 6.


## Build

### Visual studio

In *CMakePresets.json* set **CMAKE_TOOLCHAIN_FILE** to your vcpkg toolchain file (*vcpkg/scripts/buildsystems/vcpkg.cmake*).

### Cmake

**CMAKE_TOOLCHAIN_FILE** variable needs to be specified during cmake configuration. 
