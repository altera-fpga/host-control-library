include("${HCL_CMAKE_MODULE_DIR}/hclPython.cmake")

add_library(hcl_imx678
    src/imx678.c
)

set_target_properties(hcl_imx678 PROPERTIES POSITION_INDEPENDENT_CODE ON)

if(HCL_BUILD_PYTHON)
    hcl_add_python_extension(
        TARGET _imx678
        SOURCES src/imx678_pybind.cpp
        OUTPUT_NAME "imx678"
        PACKAGE_DIR "${CMAKE_BINARY_DIR}/python/altera_host_control"
        LINK_LIBRARIES hcl_core hcl_imx678
        INCLUDE_DIRS
            ${CMAKE_SOURCE_DIR}/include
            ${CMAKE_SOURCE_DIR}/interfaces/include
            ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
endif()

if(HCL_BUILD_CPP)
    target_sources(hcl_imx678 PRIVATE src/imx678.cpp)
endif()

add_library(hcl::sensors::imx678 ALIAS hcl_imx678)

target_include_directories(hcl_imx678
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(hcl_imx678
    PUBLIC hcl_altera_i2c
)

if(HCL_BUILD_CPP)
    target_compile_features(hcl_imx678 PUBLIC cxx_std_20)
endif()

target_compile_options(hcl_imx678
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)

install(TARGETS hcl_imx678
    EXPORT hclTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(FILES
    include/hcl/sensors/imx678_modes.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/hcl/sensors
)
