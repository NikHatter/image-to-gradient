# CMakeList.txt : CMake project for image-to-gradient, include source and define
# project specific logic here.
#

set(Qt6_USE_STATIC_LIBS ON)
set(Qt6_USE_STATIC_RUNTIME ON)

find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)
find_package(Qt6 CONFIG COMPONENTS Widgets)

if (Qt6_FOUND)

  set(CMAKE_AUTOUIC ON)
  set(CMAKE_AUTOMOC ON)
  set(CMAKE_AUTORCC ON)

  set(PROJECT_NAME image-to-gradient-gui)
  
  set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    
    "${CMAKE_SOURCE_DIR}/include/gradient.hpp"
    "${CMAKE_SOURCE_DIR}/include/image/boost_pixel.hpp"
    "${CMAKE_SOURCE_DIR}/include/image/boost_image.hpp"
    "${CMAKE_SOURCE_DIR}/include/image/qt_image.hpp"
  ) 
  
  qt_add_executable(${PROJECT_NAME}
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
  )

    set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "image-to-gradient")

  target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/include")

  target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets ${JPEG_LIBRARIES} PNG::PNG)

  set_target_properties(${PROJECT_NAME} PROPERTIES
  #  MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
  #  MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
  #  MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
  #  MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
  )

  install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  )
  
  qt_finalize_executable(${PROJECT_NAME})

endif()