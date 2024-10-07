cmake_minimum_required(VERSION 3.5)

project(game)

set(SOURCE_FILES src/main.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE
    pspgu
    pspge
    pspdebug
    pspdisplay
    pspgum
	pspctrl
)

# Create an EBOOT.PBP file
create_pbp_file(
    TARGET ${PROJECT_NAME}
	ICON_PATH NULL
	BACKGROUND_PATH NULL
	PREVIEW_PATH NULL
    TITLE ${PROJECT_NAME}
    VERSION 01.00
)


