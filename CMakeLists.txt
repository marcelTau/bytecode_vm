cmake_minimum_required(VERSION 3.21)

project(Bytecode-VM-lib)

include(FetchContent)
include(CTest)


if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)
    include(${CMAKE_MODULE_PATH}/CodeCoverage.cmake)
    APPEND_COVERAGE_COMPILER_FLAGS()
    setup_target_for_coverage_gcovr_html(NAME ${PROJECT_NAME}_coverage ${PROJECT_TEST_NAME} coverage)
endif()

set(EXE_NAME Bytecode-VM)

set(SOURCES 
    src/chunk.cpp
    src/compiler.cpp
    src/vm.cpp
    src/scanner.cpp
)
set(HEADERS
    include/chunk.h 
    include/compiler.h  
    include/opcode.h  
    include/scanner.h  
    include/token.h  
    include/vm.h
)
set(MAIN src/main.cpp)

set(CMAKE_CXX_STANDARD 20)


# This is a way to use cmake to download packages for you.
# Install fmt library
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG        92d36e82c437f33562bb32096378746c69d96aac
)

# Install googletest library
FetchContent_Declare(
   googletest
   GIT_REPOSITORY https://github.com/google/googletest.git
   GIT_TAG 58d77fa8070e8cec2dc1ed015d66b454c8d78850
)
FetchContent_MakeAvailable(fmt googletest)

set(TARGET_LIST ${PROJECT_NAME} ${EXE_NAME} fmt gtest gtest_main)

# The Executable
add_executable(${EXE_NAME} ${MAIN} ${SOURCES} ${HEADERS})
target_include_directories(${EXE_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(${EXE_NAME} fmt)

# The library
add_library(${PROJECT_NAME} STATIC ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} fmt)


if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU") 
    message("GCC build")
    target_compile_options(${PROJECT_NAME} PUBLIC -Wall -Wextra -Werror -pedantic -Wconversion)
    target_compile_options(${EXE_NAME} PUBLIC -Wall -Wextra -Werror -pedantic -Wconversion -DDEBUG_TRACE_EXECUTION)
else()
    message("This platform is not supported at the moment.")
endif()

foreach (target ${TARGET_LIST})

    # set binary filenames
    set_target_properties(${target} PROPERTIES OUTPUT_NAME_DEBUG ${target}-debug)
    set_target_properties(${target} PROPERTIES OUTPUT_NAME_RELWITHDEBINFO ${target}-relwithdebinfo)
    set_target_properties(${target} PROPERTIES OUTPUT_NAME_RELEASE ${target}-release)
    set_target_properties(${target} PROPERTIES OUTPUT_NAME_MINSIZEREL ${target}-minsizerel)

    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        message("Enabling LTO for target ${target}")
        set_property(TARGET ${target} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    else ()
        message("Not enabling LTO for target ${target} (not a release build)")
    endif ()

endforeach()

enable_testing()
add_subdirectory(test)
