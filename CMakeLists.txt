cmake_minimum_required(VERSION 3.20)
project(llvm LANGUAGES C CXX)

include(FetchContent)
FetchContent_Declare(lexy URL https://lexy.foonathan.net/download/lexy-src.zip)
FetchContent_MakeAvailable(lexy)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(APPLE AND NOT DEFINED LLVM_DIR)
    execute_process(
        COMMAND brew --prefix llvm
        OUTPUT_VARIABLE HOMEBREW_LLVM_PREFIX
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    if(EXISTS "${HOMEBREW_LLVM_PREFIX}/lib/cmake/llvm")
        set(LLVM_DIR "${HOMEBREW_LLVM_PREFIX}/lib/cmake/llvm")
    endif()
endif()

find_package(LLVM REQUIRED CONFIG)
find_package(SDL2 REQUIRED)

add_subdirectory(application)
add_subdirectory(asm/asm_to_ir)
