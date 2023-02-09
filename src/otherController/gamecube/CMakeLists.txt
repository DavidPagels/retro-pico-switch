add_library(gamecube GamecubeController.cpp)
target_link_libraries(gamecube pico_stdlib controller)
target_include_directories(gamecube PUBLIC ${PROJECT_SOURCE_DIR}/include)
