
cmake_minimum_required(VERSION 3.26)

set(CMAKE_USER_MAKE_RULES_OVERRIDE "${CMAKE_CURRENT_SOURCE_DIR}/CMakeOverride.txt")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD 17)

project(hikocsp LANGUAGES CXX)

include(FetchContent)
find_package(fmt REQUIRED)

if(APPLE)
include_directories(/opt/homebrew/include)
endif()


# disable adding CTest build targets like "NightlyStart" (before ctest include)
set_property(GLOBAL PROPERTY CTEST_TARGETS_ADDED 1)
include(CTest)

# if(HIKO_BUILD_TESTING)
    #
    # GoogleTest - non-vcpkg, directly build from externals
    #
    # set(INSTALL_GTEST OFF CACHE INTERNAL "Don't install gtest")
    # set(BUILD_GMOCK OFF CACHE INTERNAL "Don't build gmock")
    # FetchContent_Declare(googletest GIT_REPOSITORY https://github.com/google/googletest.git GIT_TAG v1.13.0)
    # FetchContent_MakeAvailable(googletest)

    # include(GoogleTest)
    # set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    # set_target_properties(gtest      PROPERTIES FOLDER extern)
    # set_target_properties(gtest_main PROPERTIES FOLDER extern)
# endif()

set(HIKOCSP_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/hikocsp")
set(HIKOCSP_EXAMPLES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/examples/")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

add_library(hikocsp-lib INTERFACE)
set_target_properties(hikocsp-lib PROPERTIES DEBUG_POSTFIX "d")
target_compile_features(hikocsp-lib INTERFACE cxx_std_23)

target_sources(hikocsp-lib PUBLIC FILE_SET hikocsp_include_files TYPE HEADERS BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/src/" FILES
    ${HIKOCSP_SOURCE_DIR}/csp_token.hpp
    ${HIKOCSP_SOURCE_DIR}/csp_error.hpp
    ${HIKOCSP_SOURCE_DIR}/csp_parser.hpp
    ${HIKOCSP_SOURCE_DIR}/csp_translator.hpp
    ${HIKOCSP_SOURCE_DIR}/generator.hpp
    ${HIKOCSP_SOURCE_DIR}/option_parser.hpp
)

target_include_directories(hikocsp-lib INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>"
    "$<INSTALL_INTERFACE:include>"
)

add_executable(hikocsp-bin main.cpp)
target_link_libraries(hikocsp-bin hikocsp-lib)
target_link_libraries(hikocsp-bin fmt::fmt)
if(LINUX)
    target_compile_definitions(hikocsp-bin PUBLIC FMT_HEADER_ONLY)
endif()
set_target_properties(hikocsp-bin PROPERTIES OUTPUT_NAME hikocsp)


if(HIKO_BUILD_TESTING AND OFF)
    add_executable(hikocsp_tests)
    target_link_libraries(hikocsp_tests PRIVATE gtest_main hikocsp-lib fmt::fmt)
    target_include_directories(hikocsp_tests PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

    target_sources(hikocsp_tests PRIVATE
        ${HIKOCSP_SOURCE_DIR}/csp_parser_tests.cpp
        ${HIKOCSP_SOURCE_DIR}/generator_tests.cpp
        ${HIKOCSP_SOURCE_DIR}/option_parser_tests.cpp
    )
    gtest_discover_tests(hikocsp_tests DISCOVERY_MODE PRE_TEST)

    add_executable(hikocsp_examples)
    target_link_libraries(hikocsp_examples PRIVATE gtest_main hikocsp-lib fmt::fmt)
    target_include_directories(hikocsp_examples PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

    target_sources(hikocsp_examples PRIVATE
        ${HIKOCSP_EXAMPLES_DIR}/hikocsp_yield_tests.cpp
        ${HIKOCSP_EXAMPLES_DIR}/hikocsp_append_tests.cpp
        ${HIKOCSP_EXAMPLES_DIR}/hikocsp_callback_tests.cpp
        ${HIKOCSP_EXAMPLES_DIR}/hikocsp_no_line_tests.cpp
    )
    gtest_discover_tests(hikocsp_examples DISCOVERY_MODE PRE_TEST)

    add_custom_command(
        OUTPUT "${HIKOCSP_EXAMPLES_DIR}/hikocsp_yield_tests.cpp"
        COMMAND hikocsp "${HIKOCSP_EXAMPLES_DIR}/hikocsp_yield_tests.cpp.csp"
        DEPENDS hikocsp "${HIKOCSP_EXAMPLES_DIR}/hikocsp_yield_tests.cpp.csp"
    )

    add_custom_command(
        OUTPUT "${HIKOCSP_EXAMPLES_DIR}/hikocsp_append_tests.cpp"
        COMMAND hikocsp "--append=out" "${HIKOCSP_EXAMPLES_DIR}/hikocsp_append_tests.cpp.csp"
        DEPENDS hikocsp "${HIKOCSP_EXAMPLES_DIR}/hikocsp_append_tests.cpp.csp"
    )

    add_custom_command(
        OUTPUT "${HIKOCSP_EXAMPLES_DIR}/hikocsp_callback_tests.cpp"
        COMMAND hikocsp "--callback=sink" "${HIKOCSP_EXAMPLES_DIR}/hikocsp_callback_tests.cpp.csp"
        DEPENDS hikocsp "${HIKOCSP_EXAMPLES_DIR}/hikocsp_callback_tests.cpp.csp"
    )

    add_custom_command(
        OUTPUT "${HIKOCSP_EXAMPLES_DIR}/hikocsp_no_line_tests.cpp"
        COMMAND hikocsp "--disable-line" "${HIKOCSP_EXAMPLES_DIR}/hikocsp_no_line_tests.cpp.csp"
        DEPENDS hikocsp "${HIKOCSP_EXAMPLES_DIR}/hikocsp_no_line_tests.cpp.csp"
    )

endif()
