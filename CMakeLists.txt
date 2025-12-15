cmake_minimum_required(VERSION 3.10)
project(MicMuter LANGUAGES C RC)

enable_language(RC)
set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> <DEFINES> <INCLUDES> <FLAGS> <SOURCE> -o <OBJECT>")

add_executable(MicMuter 
    src/main.c 
    src/functions.c
    resources/resources.rc
)

target_include_directories(MicMuter PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/resources")

target_link_libraries(MicMuter 
    ole32 oleaut32 mmdevapi user32 shell32 uuid
)