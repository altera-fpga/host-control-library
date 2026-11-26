add_library(hcl_abr_bridge
    src/abr_bridge.c
)

if(HCL_BUILD_PYTHON)
    hcl_add_python_extension(
        TARGET _abr_bridge
        SOURCES src/abr_bridge_pybind.cpp
        OUTPUT_NAME "abr_bridge"
        PACKAGE_DIR "${CMAKE_BINARY_DIR}/python/altera_host_control"
        LINK_LIBRARIES hcl_core hcl_abr_bridge
        INCLUDE_DIRS
            ${CMAKE_SOURCE_DIR}/include
            ${CMAKE_SOURCE_DIR}/bindings/python/include
            ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
endif()

if(HCL_BUILD_CPP)
    target_sources(hcl_abr_bridge PRIVATE src/abr_bridge.cpp)
endif()

add_library(hcl::drivers::abr_bridge ALIAS hcl_abr_bridge)

target_include_directories(hcl_abr_bridge
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(hcl_abr_bridge
    PUBLIC hcl_core
)

if(HCL_BUILD_CPP)
    target_compile_features(hcl_abr_bridge PUBLIC cxx_std_20)
endif()

target_compile_options(hcl_abr_bridge
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)

install(TARGETS hcl_abr_bridge
    EXPORT hclTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
