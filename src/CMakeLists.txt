cmake_minimum_required(VERSION 3.20)
project(avanti VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-DXPLM200=1 -DXPLM210=1 -DXPLM300=1 -DXPLM301=1 -DXPLM400=1)

if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "Build architectures for Mac OS X" FORCE)
    add_definitions(-DAPL=1 -DIBM=0 -DLIN=0)
    set(ARCH "mac64")
elseif(WIN32)
    add_definitions(-DAPL=0 -DIBM=1 -DLIN=0)
    add_definitions(-D_WIN32_WINNT=0x0600)
    set(ARCH "mingw64")

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static-libgcc")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
    set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "${CMAKE_SHARED_LIBRARY_LINK_C_FLAGS} -static-libgcc -s")
    set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "${CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS}  -static-libgcc -static-libstdc++ -s")
else()
    add_definitions(-DAPL=0 -DIBM=0 -DLIN=1)
    set(ARCH "lin64")
endif()

set(SDK_ROOT "${PROJECT_SOURCE_DIR}/SDK")
include_directories(
        "${SDK_ROOT}/CHeaders/XPLM"
        "${SDK_ROOT}/CHeaders/Wrappers"
        "${SDK_ROOT}/CHeaders/Widgets"
)

add_library(avanti SHARED avanti.cpp units_conv.cpp units_conv.h)
target_compile_options(avanti PUBLIC -Wall -Wextra )

# X-Plane stuff
if(APPLE)
    find_library(XPLM_LIBRARY XPLM ${SDK_ROOT}/Libraries/Mac)
    find_library(XPWIDGETS_LIBRARY XPWidgets ${SDK_ROOT}/Libraries/Mac/)
    find_library(CocoaLib Cocoa)

    target_link_libraries(avanti PUBLIC ${XPLM_LIBRARY})
    target_link_libraries(avanti PUBLIC ${XPWIDGETS_LIBRARY})
    target_link_libraries(avanti PUBLIC ${CocoaLib})
elseif(WIN32)
    set(CMAKE_LIBRARY_PATH ${SDK_ROOT}/Libraries/Win/)
    find_library(XPLM_LIBRARY
            NAMES XPLM_64
            PATHS ${SDK_ROOT}/Libraries/Win/
            NO_DEFAULT_PATH
            NO_CMAKE_FIND_ROOT_PATH
            )
    find_library(XPWIDGETS_LIBRARY
            XPWidgets_64
            ${SDK_ROOT}/Libraries/Win/
            NO_DEFAULT_PATH
            NO_CMAKE_FIND_ROOT_PATH
            )
    target_link_libraries(avanti PUBLIC ${XPLM_LIBRARY} ${XPWIDGETS_LIBRARY})
endif()


set_target_properties(avanti PROPERTIES PREFIX "")
set_target_properties(avanti PROPERTIES SUFFIX "")
set_target_properties(avanti PROPERTIES OUTPUT_NAME "avanti.xpl")

set(PLUGIN_BUILD_DIR "${PROJECT_SOURCE_DIR}/avanti")

if(WIN32)
    set_target_properties(avanti PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PLUGIN_BUILD_DIR}/win_x64")
    set_target_properties(avanti PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${PLUGIN_BUILD_DIR}/win_x64")
elseif(APPLE)
    set_target_properties(avanti PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PLUGIN_BUILD_DIR}/mac_x64")
    set_target_properties(avanti PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${PLUGIN_BUILD_DIR}/mac_x64")
else()
    set_target_properties(avanti PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PLUGIN_BUILD_DIR}/lin_x64")
    set_target_properties(avanti PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${PLUGIN_BUILD_DIR}/lin_x64")
endif()

set_target_properties(avanti PROPERTIES LINK_FLAGS  "${CMAKE_SHARED_LINKER_FLAGS} -fno-stack-protector -fPIC -fvisibility=hidden")