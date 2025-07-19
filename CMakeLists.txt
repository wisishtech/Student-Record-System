cmake_minimum_required(VERSION 3.31)
project(SchoolRecordSystem VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Set build type to Debug if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Compiler flags
set(CMAKE_C_FLAGS_DEBUG "-g -Wall -Wextra -O0")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")

# Find required packages
find_package(PkgConfig REQUIRED)

# Find GTK3
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${GTK3_INCLUDE_DIRS})

# Link directories
link_directories(${GTK3_LIBRARY_DIRS})

# Add definitions
add_definitions(${GTK3_CFLAGS_OTHER})

# Create directories
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/data)

# Source files - add all your implementation files
set(SOURCES
        src/main.c
        src/student.c
        src/gui.c
        src/file_operations.c
        src/calculations.c
        src/memory_manager.c
)

# Header files
set(HEADERS
        include/student.h
        include/gui.h
        include/file_operations.h
        include/calculations.h
        include/memory_manager.h
)

# Create executable with all source files
add_executable(SchoolRecordSystem ${SOURCES} ${HEADERS})

# Link GTK3 libraries
target_link_libraries(SchoolRecordSystem ${GTK3_LIBRARIES})

# Compiler-specific options for better code quality
if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
    target_compile_options(SchoolRecordSystem PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Wformat=2
            -Wno-unused-parameter
            -Wshadow
            -Wwrite-strings
            -Wstrict-prototypes
            -Wold-style-definition
            -Wredundant-decls
            -Wnested-externs
            -Wmissing-include-dirs
    )
endif()

# macOS specific settings (for your MacBook Pro 2017)
if(APPLE)
    # Find Homebrew GTK3
    execute_process(
            COMMAND brew --prefix gtk+3
            OUTPUT_VARIABLE GTK3_PREFIX
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
    )

    if(GTK3_PREFIX)
        message(STATUS "Found Homebrew GTK3 at: ${GTK3_PREFIX}")

        # Add Homebrew GTK3 include directories
        include_directories(${GTK3_PREFIX}/include/gtk-3.0)
        include_directories(${GTK3_PREFIX}/include/glib-2.0)
        include_directories(${GTK3_PREFIX}/lib/glib-2.0/include)
        include_directories(${GTK3_PREFIX}/include/pango-1.0)
        include_directories(${GTK3_PREFIX}/include/cairo)
        include_directories(${GTK3_PREFIX}/include/gdk-pixbuf-2.0)
        include_directories(${GTK3_PREFIX}/include/atk-1.0)

        # Add library directories
        link_directories(${GTK3_PREFIX}/lib)

        # Set PKG_CONFIG_PATH for GTK3
        set(ENV{PKG_CONFIG_PATH} "${GTK3_PREFIX}/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")
    else()
        message(WARNING "GTK3 not found via Homebrew. Please install with: brew install gtk+3")
    endif()

    # macOS frameworks
    find_library(COCOA_LIBRARY Cocoa)
    if(COCOA_LIBRARY)
        target_link_libraries(SchoolRecordSystem ${COCOA_LIBRARY})
    endif()

    # Set RPATH for macOS
    set_target_properties(SchoolRecordSystem PROPERTIES
            INSTALL_RPATH_USE_LINK_PATH TRUE
            MACOSX_RPATH TRUE
    )
endif()

# Installation rules
install(TARGETS SchoolRecordSystem
        RUNTIME DESTINATION bin
)

# Install data directory (only if it exists)
if(EXISTS ${CMAKE_SOURCE_DIR}/data)
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/data/
            DESTINATION share/SchoolRecordSystem/data
            FILES_MATCHING PATTERN "*.txt"
    )
endif()

# Custom targets for development

# Run the application
add_custom_target(run
        COMMAND ${CMAKE_BINARY_DIR}/SchoolRecordSystem
        DEPENDS SchoolRecordSystem
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Running School Record System"
)

# Run in console mode
add_custom_target(run-console
        COMMAND ${CMAKE_BINARY_DIR}/SchoolRecordSystem --console
        DEPENDS SchoolRecordSystem
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Running School Record System in console mode"
)

# Debug target (if GDB is available)
find_program(GDB_EXECUTABLE gdb)
if(GDB_EXECUTABLE)
    add_custom_target(debug
            COMMAND ${GDB_EXECUTABLE} ${CMAKE_BINARY_DIR}/SchoolRecordSystem
            DEPENDS SchoolRecordSystem
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Debugging School Record System with GDB"
    )
endif()

# Valgrind target for memory checking (if available)
find_program(VALGRIND_EXECUTABLE valgrind)
if(VALGRIND_EXECUTABLE)
    add_custom_target(memcheck
            COMMAND ${VALGRIND_EXECUTABLE} --leak-check=full --show-leak-kinds=all
            --track-origins=yes --verbose ${CMAKE_BINARY_DIR}/SchoolRecordSystem --console
            DEPENDS SchoolRecordSystem
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Running memory check with Valgrind"
    )
endif()

# Clean all generated files
add_custom_target(clean-all
        COMMAND ${CMAKE_BUILD_TOOL} clean
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/data
        COMMENT "Cleaning all generated files"
)

# Create macOS application bundle
if(APPLE)
    add_custom_target(app-bundle
            COMMAND ${CMAKE_COMMAND} -E make_directory SchoolRecordSystem.app/Contents/MacOS
            COMMAND ${CMAKE_COMMAND} -E make_directory SchoolRecordSystem.app/Contents/Resources
            COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/SchoolRecordSystem SchoolRecordSystem.app/Contents/MacOS/
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/data SchoolRecordSystem.app/Contents/Resources/data
            DEPENDS SchoolRecordSystem
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Creating macOS application bundle"
    )
endif()

# Print configuration summary
message(STATUS "")
message(STATUS "=== School Record Management System Configuration ===")
message(STATUS "Project: ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C Standard: C${CMAKE_C_STANDARD}")
message(STATUS "C Compiler: ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}")
message(STATUS "Source Directory: ${CMAKE_SOURCE_DIR}")
message(STATUS "Binary Directory: ${CMAKE_BINARY_DIR}")
message(STATUS "Install Prefix: ${CMAKE_INSTALL_PREFIX}")

# GTK3 information
if(GTK3_FOUND)
    message(STATUS "GTK3: Found")
    message(STATUS "  Version: ${GTK3_VERSION}")
    message(STATUS "  Include Dirs: ${GTK3_INCLUDE_DIRS}")
    message(STATUS "  Libraries: ${GTK3_LIBRARIES}")
else()
    message(WARNING "GTK3: Not found - GUI features will not work")
endif()

if(APPLE AND GTK3_PREFIX)
    message(STATUS "Homebrew GTK3 Prefix: ${GTK3_PREFIX}")
endif()

message(STATUS "")
message(STATUS "=== Available Build Targets ===")
message(STATUS "make               - Build the application")
message(STATUS "make run           - Build and run (GUI mode)")
message(STATUS "make run-console   - Build and run (console mode)")

if(GDB_EXECUTABLE)
    message(STATUS "make debug         - Run with GDB debugger")
endif()

if(VALGRIND_EXECUTABLE)
    message(STATUS "make memcheck      - Run memory check with Valgrind")
endif()

message(STATUS "make install       - Install the application")
message(STATUS "make clean-all     - Clean all generated files")

if(APPLE)
    message(STATUS "make app-bundle    - Create macOS app bundle")
endif()

message(STATUS "")
message(STATUS "=== Setup Instructions ===")
message(STATUS "1. Install GTK3: brew install gtk+3 cmake pkg-config")
message(STATUS "2. Build: mkdir build && cd build && cmake .. && make")
message(STATUS "3. Run: make run")
message(STATUS "")