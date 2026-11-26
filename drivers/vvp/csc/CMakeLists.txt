add_library(hcl_altera_vvp_csc
    src/altera_vvp_csc.c
)

if(HCL_BUILD_CPP)
    target_sources(hcl_altera_vvp_csc PRIVATE src/altera_vvp_csc.cpp)
endif()

add_library(hcl::drivers::altera_vvp_csc ALIAS hcl_altera_vvp_csc)

target_include_directories(hcl_altera_vvp_csc
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(hcl_altera_vvp_csc
    PUBLIC hcl_core hcl_altera_vvp_core
)

if(HCL_BUILD_CPP)
    target_compile_features(hcl_altera_vvp_csc PUBLIC cxx_std_20)
endif()

target_compile_options(hcl_altera_vvp_csc
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)

install(TARGETS hcl_altera_vvp_csc
    EXPORT hclTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

if(HCL_BUILD_PYTHON)
    hcl_add_python_extension(
        TARGET _altera_vvp_csc
        SOURCES src/altera_vvp_csc_pybind.cpp
        OUTPUT_NAME "altera_vvp_csc"
        PACKAGE_DIR "${CMAKE_BINARY_DIR}/python/altera_host_control"
        LINK_LIBRARIES hcl_core hcl_altera_vvp_csc
        INCLUDE_DIRS
            ${CMAKE_SOURCE_DIR}/include
            ${CMAKE_SOURCE_DIR}/bindings/python/include
            ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
endif()
