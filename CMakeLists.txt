cmake_minimum_required(VERSION 3.16)

project(FastWriterPro)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)


# Add include directories
include_directories(${CMAKE_SOURCE_DIR}/headers)

set(SOURCES
    src/main.cpp
    src/autocompleteapp.cpp
    src/inputfield.cpp
    src/hoverablebutton.cpp
    src/trie.cpp
    src/trienode.cpp
)

set(DATA_MODEL
    data_model/model.cpp
    data_model/model.h
)

set(HEADERS
    headers/autocompleteapp.h
    headers/inputfield.h
    headers/hoverablebutton.h
    headers/trie.h
    headers/trienode.h
)

add_executable(FastWriterPro ${SOURCES} ${HEADERS} ${DATA_MODEL}
    assets/words_dictionary.json
    assets/Style.css
    assets/json.hpp
)

target_link_libraries(FastWriterPro PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# Set default properties
set_target_properties(FastWriterPro PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Set bundle identifier for Qt versions before 6.1.0
if(Qt6_VERSION VERSION_LESS "6.1.0")
    set_target_properties(FastWriterPro PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER com.example.untitled3
    )
endif()

include(GNUInstallDirs)
install(TARGETS FastWriterPro
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
