add_library(graphics STATIC
  glad.c
  gl_context/renderer.cpp
)

target_include_directories(graphics
PUBLIC
    include
)

find_package(OpenGL REQUIRED)

target_link_libraries(graphics
PUBLIC
    OpenGL::GL
)

target_compile_features(graphics PUBLIC cxx_std_17)