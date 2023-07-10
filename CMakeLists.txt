# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  #set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("image-to-gradient")

if(WIN32)
  #set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

  #if (NOT DEFINED IMAGE_TO_GRADIENT_ARCH)
      if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
        set(IMAGE_TO_GRADIENT_ARCH "x64")
      else( CMAKE_SIZEOF_VOID_P EQUAL 8 )
        set(IMAGE_TO_GRADIENT_ARCH "x86")
      endif( CMAKE_SIZEOF_VOID_P EQUAL 8 )
  #endif ()

  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${image-to-gradient_SOURCE_DIR}/out/lib/${IMAGE_TO_GRADIENT_ARCH}_$<CONFIG>)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${image-to-gradient_SOURCE_DIR}/out/lib/${IMAGE_TO_GRADIENT_ARCH}_$<CONFIG>)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${image-to-gradient_SOURCE_DIR}/out/bin/${IMAGE_TO_GRADIENT_ARCH}_$<CONFIG>)
else(WIN32)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${image-to-gradient_SOURCE_DIR}/out/lib/${CMAKE_BUILD_TYPE})
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${image-to-gradient_SOURCE_DIR}/out/lib/${CMAKE_BUILD_TYPE})
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${image-to-gradient_SOURCE_DIR}/out/bin/${CMAKE_BUILD_TYPE})
endif()


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include sub-projects.
add_subdirectory ("console_app")
add_subdirectory ("gui_app")
