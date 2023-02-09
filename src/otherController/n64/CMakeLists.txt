add_library(n64 N64Controller.cpp)
target_link_libraries(n64 pico_stdlib controller)
target_include_directories(n64 PUBLIC ${PROJECT_SOURCE_DIR}/include)
