# Options:
# CONTROLLER_TYPE: "Gamecube" or "N64" (default N64)
# If Gamecube controller type
# GC_CONTROLLER_TYPE: "oneToOne" or "sms" (default oneToOne)
add_compile_definitions(CONTROLLER_TYPE="Gamecube")
add_subdirectory(otherController)
add_subdirectory(switchController)

add_executable(${PROJECT_NAME} main.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} pico_stdlib tinyusb_device switchController n64 gamecube)

pico_add_extra_outputs(${PROJECT_NAME})