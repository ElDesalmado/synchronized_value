cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

project(synchronized_value LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)

add_library(synchronized_value INTERFACE)
target_include_directories(synchronized_value INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

enable_testing()

# tests

# deploy dlls if MinGW
if(CMAKE_CXX_COMPILER_ID)
    find_file(libgcc NAMES libgcc_s_dw2-1.dll libgcc_s_seh-1.dll)
    find_file(libstdc++-6 libstdc++-6.dll)
    find_file(libwinpthread-1 libwinpthread-1.dll)

    add_custom_target(COPY_MINGW_DEPS
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${libgcc} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${libstdc++-6} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${libwinpthread-1} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            COMMENT "Copying MinGW32 dependent libraries"
            )
else()
    add_custom_target(COPY_MINGW_DEPS)
endif()

add_executable(test_unique_lock
        test/run_check.h
        test/test_unique_lock.cpp
        )
target_link_libraries(test_unique_lock PRIVATE synchronized_value)
target_include_directories(test_unique_lock PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(test_unique_lock COPY_MINGW_DEPS)

add_test(NAME test_unique_lock
        COMMAND test_unique_lock)

add_executable(test_lock_guard
        test/run_check.h
        test/test_lock_guard.cpp
        )
target_link_libraries(test_lock_guard PRIVATE synchronized_value)
target_include_directories(test_lock_guard PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(test_lock_guard COPY_MINGW_DEPS)

add_test(NAME test_lock_guard
        COMMAND test_lock_guard)

add_executable(test_synch_value
        test/run_check.h
        test/test_synch_value.cpp
        )
target_link_libraries(test_synch_value PRIVATE synchronized_value)
target_include_directories(test_synch_value PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(test_synch_value COPY_MINGW_DEPS)

add_test(NAME test_synch_value
        COMMAND test_synch_value)



