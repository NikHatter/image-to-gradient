# CMakeList.txt : CMake project for image-to-gradient, include source and define
# project specific logic here.
#

find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)

set(PROJECT_NAME image-to-gradient-console)

# Add source to this project's executable.
add_executable (${PROJECT_NAME}
  "main.cpp"
  "${CMAKE_SOURCE_DIR}/include/gradient.hpp"
  "${CMAKE_SOURCE_DIR}/include/image/boost_pixel.hpp"
  "${CMAKE_SOURCE_DIR}/include/image/boost_image.hpp"
)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "itg")

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_link_libraries(${PROJECT_NAME} ${JPEG_LIBRARIES} PNG::PNG)


# TODO: Add tests and install targets if needed.
