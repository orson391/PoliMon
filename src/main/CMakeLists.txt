add_executable(PoliMon main.cpp)

target_link_libraries(PoliMon PRIVATE
    game
    core
    engine
    SDL3::SDL3
    SDL3_image::SDL3_image
)

# Deploy the runtime libraries associated with the configured CMake packages.
add_custom_command(TARGET PoliMon POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3::SDL3>
            $<TARGET_FILE_DIR:PoliMon>
    COMMENT "Copying SDL3.dll to build directory"
)

add_custom_command(TARGET PoliMon POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3_image::SDL3_image>
            $<TARGET_FILE_DIR:PoliMon>
    COMMENT "Copying SDL3_image.dll to build directory"
)
