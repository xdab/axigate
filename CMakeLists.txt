cmake_minimum_required(VERSION 3.10)
project(axigate C)
set(CMAKE_C_STANDARD 11)

# Set compiler flags based on build type
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(DEBUG_FLAGS "-pg -O0 -DDEBUG")
    set(RELEASE_FLAGS "")
    set(RELEASE_LINK_FLAGS "")
else()
    set(DEBUG_FLAGS "")
    set(RELEASE_FLAGS "-O3 -DNDEBUG -flto")
    set(RELEASE_LINK_FLAGS "-Wl,--gc-sections")
endif()

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${DEBUG_FLAGS}")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${RELEASE_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} ${RELEASE_LINK_FLAGS}")

include(GNUInstallDirs)

# Submodules (uncomment as needed)
# add_subdirectory(libs/some_lib)

# Main executable
add_executable(axigate src/main.c src/axigate.c src/options.c src/options_args.c)
target_include_directories(axigate PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

install(TARGETS axigate
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Tests
add_executable(axigate_test test/main_test.c test/test.c src/axigate.c)
target_include_directories(axigate_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/test ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(axigate_test m)

# Strip symbols in Release builds
if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_custom_command(TARGET axigate_test POST_BUILD COMMAND ${CMAKE_STRIP} $<TARGET_FILE:axigate_test>)
endif()

# Systemd service installation
file(GLOB SYSTEMD_SERVICE_FILES "systemd/*.service")
install(FILES ${SYSTEMD_SERVICE_FILES}
    DESTINATION /etc/systemd/system
)
