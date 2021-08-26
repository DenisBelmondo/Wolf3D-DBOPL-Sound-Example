cmake_minimum_required(VERSION 3.0.0)
project(adlibtest VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(adlibtest
    main.cpp
    dbopl/dbopl.cpp
    Nuked-OPL3/opl3.c
)

target_include_directories(
    adlibtest
    PRIVATE
    C:/Users/Mason/.opt/vcpkg/installed/x64-windows/include/SDL2
    D:/Documents/SDL-Mixer-X/include
    dbopl
    Nuked-OPL3
)

target_link_libraries(
    adlibtest
    C:/Users/Mason/.opt/vcpkg/installed/x64-windows/lib/SDL2.lib
    C:/Users/Mason/.opt/vcpkg/installed/x64-windows/lib/manual-link/SDL2main.lib
    D:/Documents/SDL-Mixer-X/build/lib/SDL2_mixer_ext.lib
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
