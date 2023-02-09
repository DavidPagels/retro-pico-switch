cmake_minimum_required(VERSION 3.25.1)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include($ENV{PICO_SDK_PATH}/tools/CMakeLists.txt)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(PICO_CXX_ENABLE_EXCEPTIONS 1)
pico_sdk_init()

project(retro_pico_switch C CXX ASM)

add_compile_definitions(CFG_TUSB_CONFIG_FILE="${CMAKE_CURRENT_LIST_DIR}/include/tusb_config.h")
add_subdirectory(src)
