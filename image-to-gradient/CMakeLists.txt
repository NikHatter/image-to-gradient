# CMakeList.txt : CMake project for image-to-gradient, include source and define
# project specific logic here.
#

find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)

# Add source to this project's executable.
add_executable (image-to-gradient
  "image-to-gradient.cpp"
  "gradient.hpp"
  "color.hpp"
  "pixel.hpp"
  "image.hpp"
)

target_link_libraries(image-to-gradient ${JPEG_LIBRARIES} PNG::PNG)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET image-to-gradient PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
