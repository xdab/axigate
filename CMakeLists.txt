cmake_minimum_required(VERSION 3.10)
project(axigate C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

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

# Apply flags to all targets
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${DEBUG_FLAGS}")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${RELEASE_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} ${RELEASE_LINK_FLAGS}")

# Include GNUInstallDirs for standard installation paths
include(GNUInstallDirs)
include_directories(include)

# Add submodules
add_subdirectory(libs/libtnc)
add_subdirectory(libs/libcomm)

file(GLOB SOURCES "src/*.c")
add_executable(axigate ${SOURCES})
target_link_libraries(axigate PRIVATE tnc comm m)
target_include_directories(axigate PRIVATE libs/libtnc/include libs/libcomm/include)

# axigate_test: unit tests
add_executable(axigate_test test/main_test.c test/test.c src/axigate.c)
target_link_libraries(axigate_test PRIVATE tnc comm m)
target_include_directories(axigate_test PRIVATE libs/libtnc/include libs/libcomm/include)

# Strip symbols in Release builds
if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_custom_command(TARGET axigate POST_BUILD COMMAND ${CMAKE_STRIP} $<TARGET_FILE:axigate>)
endif()

# Installation
install(TARGETS axigate DESTINATION ${CMAKE_INSTALL_BINDIR})

# Install systemd service files
file(GLOB SYSTEMD_SERVICE_FILES "systemd/*.service")
install(FILES ${SYSTEMD_SERVICE_FILES}
    DESTINATION /etc/systemd/system
)

# Reload systemd daemon after installation
install(CODE "execute_process(COMMAND systemctl daemon-reload)")
