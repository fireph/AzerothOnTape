cmake_minimum_required(VERSION 3.14)
project(AzerothOnTape VERSION 1.0
        LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Network REQUIRED)
find_package(Qt5 COMPONENTS Multimedia REQUIRED)

if (APPLE)
    set(GUI_TYPE MACOSX_BUNDLE)
    find_library(APP_SERVICES_LIBRARY ApplicationServices)
    mark_as_advanced(
            APP_SERVICES_LIBRARY)
    set(EXTRA_LIBS ${APP_SERVICES_LIBRARY})

    set(ICON images/AzerothOnTape.icns)
    set_source_files_properties(images/AzerothOnTape.icns PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources")

endif (APPLE)


include(FetchContent)
FetchContent_Declare(
        qhotkey
        GIT_REPOSITORY https://github.com/Skycoder42/QHotkey.git
        GIT_TAG master
)
FetchContent_MakeAvailable(qhotkey)

FetchContent_Declare(
        input_lite

        GIT_REPOSITORY "https://github.com/smasherprog/input_lite.git"
        GIT_TAG master
)
FetchContent_MakeAvailable(input_lite)


if (WIN32)
    set(UTILS_PLATFORM_SRC
            src/utils/win/Utils.cpp
            )

elseif (APPLE)

    set(UTILS_PLATFORM_SRC
            src/utils/mac/Utils.cpp
            )
else ()
    #TODO
endif ()


add_executable(AzerothOnTape MACOSX_BUNDLE
        src/main.cpp
        src/AzerothOnTapeSettingsDialog.hpp
        src/AzerothOnTapeSettingsDialog.cpp
        src/FileDownloader.hpp
        src/FileDownloader.cpp
        ${UTILS_PLATFORM_SRC}
        azerothontape.qrc
        ${ICON})
target_compile_features(AzerothOnTape PUBLIC cxx_std_14)


target_link_libraries(AzerothOnTape Qt5::Core)
target_link_libraries(AzerothOnTape Qt5::Widgets)
target_link_libraries(AzerothOnTape Qt5::Network)
target_link_libraries(AzerothOnTape Qt5::Multimedia)

target_link_libraries(AzerothOnTape QHotkey::QHotkey)
target_include_directories(AzerothOnTape PUBLIC ${qhotkey_SOURCE_DIR}/QHotkey)

target_link_libraries(AzerothOnTape ${EXTRA_LIBS})

target_link_libraries(AzerothOnTape input_lite)
target_include_directories(AzerothOnTape PUBLIC ${input_lite_SOURCE_DIR}/include)


if (APPLE)
    set_target_properties(AzerothOnTape PROPERTIES
            RESOURCE AzerothOnTape.icns
            MACOSX_BUNDLE_ICON_FILE AzerothOnTape
            MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/cmake/macos/Info.plist")
endif ()



