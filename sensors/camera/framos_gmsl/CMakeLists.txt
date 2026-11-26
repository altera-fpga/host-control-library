include("${HCL_CMAKE_MODULE_DIR}/hclPython.cmake")

add_library(hcl_framos_gmsl
    src/framos_gmsl.c
)

if(HCL_BUILD_PYTHON)
    hcl_add_python_extension(
        TARGET _framos_gmsl
        SOURCES src/framos_gmsl_pybind.cpp
        OUTPUT_NAME "framos_gmsl"
        PACKAGE_DIR "${CMAKE_BINARY_DIR}/python/altera_host_control"
        LINK_LIBRARIES hcl_core hcl_framos_gmsl
        INCLUDE_DIRS
            ${CMAKE_SOURCE_DIR}/include
            ${CMAKE_SOURCE_DIR}/interfaces/include
            ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
endif()

if(HCL_BUILD_CPP)
    target_sources(hcl_framos_gmsl PRIVATE src/framos_gmsl.cpp)
endif()

add_library(hcl::sensors::framos_gmsl ALIAS hcl_framos_gmsl)

target_include_directories(hcl_framos_gmsl
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(hcl_framos_gmsl
    PUBLIC hcl_altera_i2c
)

if(HCL_BUILD_CPP)
    target_compile_features(hcl_framos_gmsl PUBLIC cxx_std_20)
endif()

target_compile_options(hcl_framos_gmsl
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)

install(TARGETS hcl_framos_gmsl
    EXPORT hclTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
