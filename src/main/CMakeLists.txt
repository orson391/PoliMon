add_executable(SDL3Hello main.cpp)

target_link_libraries(SDL3Hello PRIVATE
    game
    core
    engine
    SDL3::SDL3
    SDL3_image::SDL3_image
)

# Copy SDL3 DLL files to build directory
set(SDL_DLL_DIR "C:/Devlopment/SDL/SDL3-3.4.8/lib/x64")
set(SDL_IMAGE_DLL_DIR "C:/Devlopment/SDL/SDL3_image-3.4.4/lib/x64")

add_custom_command(TARGET SDL3Hello POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SDL_DLL_DIR}/SDL3.dll"
            $<TARGET_FILE_DIR:SDL3Hello>
    COMMENT "Copying SDL3.dll to build directory"
)

add_custom_command(TARGET SDL3Hello POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SDL_IMAGE_DLL_DIR}/SDL3_image.dll"
            $<TARGET_FILE_DIR:SDL3Hello>
    COMMENT "Copying SDL3_image.dll to build directory"
)

add_custom_command(TARGET SDL3Hello POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:xml_reader>
            $<TARGET_FILE_DIR:SDL3Hello>
    COMMENT "Copying xml_reader DLL to build directory"
)
