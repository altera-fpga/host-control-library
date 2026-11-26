add_library(hcl_altera_i2c
    src/altera_i2c.c
)

if(HCL_BUILD_PYTHON)
    hcl_add_python_extension(
        TARGET _altera_i2c
        SOURCES src/altera_i2c_pybind.cpp
        OUTPUT_NAME "altera_i2c"
        PACKAGE_DIR "${CMAKE_BINARY_DIR}/python/altera_host_control"
        LINK_LIBRARIES hcl_core hcl_altera_i2c
        INCLUDE_DIRS
            ${CMAKE_SOURCE_DIR}/include
            ${CMAKE_SOURCE_DIR}/interfaces/include
            ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
endif()

if(HCL_BUILD_CPP)
    target_sources(hcl_altera_i2c PRIVATE src/altera_i2c.cpp)
endif()

add_library(hcl::drivers::altera_i2c ALIAS hcl_altera_i2c)

target_include_directories(hcl_altera_i2c
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/interfaces/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(hcl_altera_i2c
    PUBLIC hcl_core
)

target_compile_features(hcl_altera_i2c PUBLIC cxx_std_20)

target_compile_options(hcl_altera_i2c
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/hcl/drivers/
    DESTINATION include/hcl/drivers
)

install(TARGETS hcl_altera_i2c
    EXPORT hclTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
