add_library(engine STATIC
    Application.cpp
    ui/ui_element.cpp
)

target_include_directories(engine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(engine PUBLIC core SDL3::SDL3)

target_compile_features(engine PUBLIC cxx_std_17)
