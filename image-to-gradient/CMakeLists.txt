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
  "boost_pixel.hpp"
  "boost_image.hpp"
)

target_link_libraries(image-to-gradient ${JPEG_LIBRARIES} PNG::PNG)


# TODO: Add tests and install targets if needed.
