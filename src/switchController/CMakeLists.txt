add_library(switchController SwitchDescriptors.cpp)
target_link_libraries(switchController pico_stdlib tinyusb_device)
target_include_directories(switchController PUBLIC ${PROJECT_SOURCE_DIR}/include)
