cmake_minimum_required(VERSION 3.25.1)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include($ENV{PICO_SDK_PATH}/tools/CMakeLists.txt)

project(n64_pico_switch C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(PICO_CXX_ENABLE_EXCEPTIONS 1)

pico_sdk_init()

add_executable(n64_pico_switch)
target_sources(n64_pico_switch PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/n64_pico_switch.cpp
    ${CMAKE_CURRENT_LIST_DIR}/N64Controller.cpp
    ${CMAKE_CURRENT_LIST_DIR}/SwitchDescriptors.cpp
    )
target_include_directories(n64_pico_switch PUBLIC ${CMAKE_CURRENT_LIST_DIR})

pico_generate_pio_header(n64_pico_switch ${CMAKE_CURRENT_LIST_DIR}/N64.pio)

target_link_libraries(n64_pico_switch pico_stdlib hardware_pio hardware_clocks tinyusb_device tinyusb_board)

pico_add_extra_outputs(n64_pico_switch)