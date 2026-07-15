# Options:
# SWITCH_BLUETOOTH: Remove for USB
if (PICO_BOARD MATCHES "_w")
    add_compile_definitions(SWITCH_BLUETOOTH)
endif()

add_subdirectory(otherController)
add_subdirectory(switchController)

add_executable(${PROJECT_NAME} main.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} pico_stdlib switchUsb n64 gamecube)

if (PICO_BOARD MATCHES "_w")
    target_link_libraries(${PROJECT_NAME} switchBluetooth)
endif()

pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${PROJECT_NAME} 1)
pico_add_extra_outputs(${PROJECT_NAME})