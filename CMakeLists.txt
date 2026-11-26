cmake_minimum_required(VERSION 3.16)
project(altera_host_control VERSION 0.3.0 LANGUAGES C)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(HCL_CMAKE_MODULE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

option(HCL_BUILD_CPP "Build C++ wrappers, examples, drivers, sensors, and C++ tests" ON)
option(HCL_BUILD_EXAMPLES "Build examples" ON)
option(HCL_BUILD_TESTS "Build tests" ON)
option(HCL_BUILD_DRIVERS "Build optional higher-level driver libraries" ON)
option(HCL_BUILD_SENSORS "Build optional sensor helpers when drivers are enabled" ON)
option(HCL_BUILD_PYTHON "Build Python bindings" OFF)
option(HCL_BUILD_UDP "Build the UDP register transport" ON)
option(HCL_BUILD_NIOS "Build the NIOS register transport" OFF)
option(HCL_BUILD_HOLOLINK "Build the optional Hololink APB register transport" OFF)

set(HCL_BUILD_UIO_DEFAULT OFF)
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(HCL_BUILD_UIO_DEFAULT ON)
endif()
option(HCL_BUILD_UIO "Build the UIO register transport (Linux only)" ${HCL_BUILD_UIO_DEFAULT})

if (HCL_BUILD_UIO AND NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "HCL_BUILD_UIO is enabled, but UIO transport is only supported on Linux. Disabling HCL_BUILD_UIO.")
    set(HCL_BUILD_UIO OFF CACHE BOOL "Build the UIO register transport (Linux only)" FORCE)
endif()

if (HCL_BUILD_CPP OR HCL_BUILD_PYTHON OR HCL_BUILD_HOLOLINK)
    enable_language(CXX)
endif()

set(HCL_INTERFACE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/src/i2c.c
)

add_library(hcl_core
    src/register_transport.c
    src/register_interface.c
    ${HCL_INTERFACE_SOURCES}
)

if (HCL_BUILD_NIOS)
    target_sources(hcl_core PRIVATE src/nios_register_transport.c)
endif()

if (HCL_BUILD_HOLOLINK)
    find_package(CUDAToolkit REQUIRED)
    find_package(fmt REQUIRED)
    find_package(Hololink CONFIG REQUIRED)
    if (NOT TARGET hololink::core)
        message(FATAL_ERROR "HCL_BUILD_HOLOLINK is enabled, but the Hololink SDK could not be found. Install the Hololink package or disable HCL_BUILD_HOLOLINK.")
    endif()
    target_sources(hcl_core PRIVATE src/hololink_register_transport.cpp)
    target_link_libraries(hcl_core PRIVATE hololink::core)
endif()

if (HCL_BUILD_UDP)
    target_sources(hcl_core PRIVATE src/udp_register_transport.c)
endif()

if (HCL_BUILD_UIO)
    target_sources(hcl_core PRIVATE src/uio_register_transport.c)
endif()

if (HCL_BUILD_CPP)
    target_sources(hcl_core PRIVATE src/register_interface.cpp)

    if (HCL_BUILD_UDP)
        target_sources(hcl_core PRIVATE src/udp_register_transport.cpp)
    endif()

    if (HCL_BUILD_UIO)
        target_sources(hcl_core PRIVATE src/uio_register_transport.cpp)
    endif()
endif()

if (HCL_BUILD_CPP OR HCL_BUILD_PYTHON OR HCL_BUILD_HOLOLINK)
    target_compile_features(hcl_core PUBLIC cxx_std_20)
endif()

add_library(hcl::core ALIAS hcl_core)

target_include_directories(hcl_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/interfaces/include>
        $<INSTALL_INTERFACE:include>
)

set_target_properties(hcl_core PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_compile_options(hcl_core
    PRIVATE
        $<$<COMPILE_LANGUAGE:C>:$<$<C_COMPILER_ID:MSVC>:/W4>>
        $<$<COMPILE_LANGUAGE:C>:$<$<NOT:$<C_COMPILER_ID:MSVC>>:-Wall;-Wextra;-Wpedantic>>
)

if (HCL_BUILD_CPP OR HCL_BUILD_HOLOLINK OR HCL_BUILD_PYTHON)
    target_compile_options(hcl_core
        PRIVATE
            $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
            $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
    )
endif()

if (WIN32)
    target_link_libraries(hcl_core PRIVATE ws2_32)
endif()

if (HCL_BUILD_EXAMPLES)
    if (HCL_BUILD_UDP)
        add_executable(udp_client_example_c examples/udp_client_example.c)
        target_link_libraries(udp_client_example_c PRIVATE hcl_core)
        install(TARGETS udp_client_example_c DESTINATION ${CMAKE_INSTALL_BINDIR})
    endif()

    if (HCL_BUILD_CPP)
        if (HCL_BUILD_UDP)
            add_executable(udp_client_example examples/udp_client_example.cpp)
            target_link_libraries(udp_client_example PRIVATE hcl_core)

            add_executable(udp_server_example examples/udp_server_example.cpp)
            target_link_libraries(udp_server_example PRIVATE hcl_core)

            add_executable(udp_i2c_example examples/udp_i2c_example.cpp)
            target_link_libraries(udp_i2c_example PRIVATE hcl_core hcl_altera_i2c)

            install(TARGETS
                udp_client_example
                udp_server_example
                udp_i2c_example
                DESTINATION ${CMAKE_INSTALL_BINDIR}
            )
        endif()

        if (HCL_BUILD_UIO)
            add_executable(uio_client_example examples/uio_client_example.cpp)
            target_link_libraries(uio_client_example PRIVATE hcl_core)
            install(TARGETS uio_client_example DESTINATION ${CMAKE_INSTALL_BINDIR})
        endif()

        if (HCL_BUILD_HOLOLINK)
            add_executable(hololink_example examples/hololink_example.cpp)
            target_link_libraries(hololink_example PRIVATE hcl_core hololink::core)

            add_executable(hololink_i2c_example examples/hololink_i2c_example.cpp)
            target_link_libraries(hololink_i2c_example PRIVATE hcl_core hcl_hololink_i2c hololink::core)

            install(TARGETS
                hololink_example
                hololink_i2c_example
                DESTINATION ${CMAKE_INSTALL_BINDIR}
            )
        endif()
    endif()

    if (HCL_BUILD_PYTHON)
        if (HCL_BUILD_UDP)
            install(FILES
                examples/python_udp_client_example.py
                examples/python_udp_i2c_example.py
                DESTINATION ${CMAKE_INSTALL_DATADIR}/altera_host_control/examples
            )
        endif()
    endif()
endif()

install(TARGETS hcl_core
    EXPORT hclTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

if (HCL_BUILD_TESTS)
    include(CTest)
    if (BUILD_TESTING)
        add_executable(register_interface_c_tests tests/register_interface_tests.c)
        target_link_libraries(register_interface_c_tests PRIVATE hcl_core)
        add_test(NAME register_interface_c_tests COMMAND register_interface_c_tests)

        if (HCL_BUILD_CPP)
            add_executable(register_interface_cpp_tests tests/register_interface_tests.cpp)
            target_link_libraries(register_interface_cpp_tests PRIVATE hcl_core)
            add_test(NAME register_interface_cpp_tests COMMAND register_interface_cpp_tests)
        endif()
    endif()
endif()

if (HCL_BUILD_DRIVERS)
    add_subdirectory(drivers)

    if (HCL_BUILD_SENSORS)
        add_subdirectory(sensors)
    endif()
endif()

if (HCL_BUILD_PYTHON)
    add_subdirectory(bindings/python)
endif()

set(HCL_CMAKE_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/hcl")
set(HCL_CMAKE_CONFIG_FILE "${CMAKE_CURRENT_BINARY_DIR}/hclConfig.cmake")
set(HCL_CMAKE_CONFIG_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/hclConfigVersion.cmake")

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/hclConfig.cmake.in"
    "${HCL_CMAKE_CONFIG_FILE}"
    INSTALL_DESTINATION "${HCL_CMAKE_INSTALL_DIR}"
)

write_basic_package_version_file(
    "${HCL_CMAKE_CONFIG_VERSION_FILE}"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(EXPORT hclTargets
    FILE hclTargets.cmake
    NAMESPACE hcl::
    DESTINATION ${HCL_CMAKE_INSTALL_DIR}
)

install(FILES
    "${HCL_CMAKE_CONFIG_FILE}"
    "${HCL_CMAKE_CONFIG_VERSION_FILE}"
    DESTINATION "${HCL_CMAKE_INSTALL_DIR}"
)

find_program(STATICK_EXECUTABLE statick)
if (STATICK_EXECUTABLE)
    add_custom_target(statick
        COMMAND ${STATICK_EXECUTABLE} ${CMAKE_SOURCE_DIR}
            --user-paths ${CMAKE_SOURCE_DIR}/.ci/statick
            --config hcl-config.yaml
            --profile hcl-profile.yaml
            --exceptions hcl-exceptions.yaml
            --output-directory ${CMAKE_BINARY_DIR}/statick-output
            --log INFO
            --check
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Run Statick static analysis"
        VERBATIM
    )
else()
    message(STATUS "Statick executable not found. Install via 'pip install statick' to enable the 'statick' target.")
endif()
