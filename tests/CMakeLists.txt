add_executable(config_test ConfigTest.cpp)
target_link_libraries(config_test PRIVATE core xml_reader)

add_custom_command(TARGET config_test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:xml_reader>
            $<TARGET_FILE_DIR:config_test>
    COMMENT "Copying xml_reader DLL to test output directory"
)

add_test(NAME config_test COMMAND config_test)
