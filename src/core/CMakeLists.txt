find_package(tinyxml2 CONFIG REQUIRED)

add_library(core STATIC
    Logger.cpp
    Config.cpp
)

add_library(xml_reader SHARED
    xml_reader.cpp
)

target_include_directories(core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(xml_reader PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(core PUBLIC tinyxml2::tinyxml2)
target_link_libraries(xml_reader PUBLIC core tinyxml2::tinyxml2)

target_compile_definitions(xml_reader PRIVATE XML_READER_EXPORTS)

set_target_properties(xml_reader PROPERTIES
    WINDOWS_EXPORT_ALL_SYMBOLS ON
)