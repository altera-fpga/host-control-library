add_library(hcl_hololink_i2c
    src/hololink_i2c.cpp
)

if(HCL_BUILD_PYTHON)
    hcl_add_python_extension(
        TARGET _hololink_i2c
        SOURCES src/hololink_i2c_pybind.cpp
        OUTPUT_NAME "hololink_i2c"
        PACKAGE_DIR "${CMAKE_BINARY_DIR}/python/altera_host_control"
        LINK_LIBRARIES hcl_core hcl_hololink_i2c
        INCLUDE_DIRS
            ${CMAKE_SOURCE_DIR}/include
            ${CMAKE_SOURCE_DIR}/interfaces/include
            ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
endif()

add_library(hcl::drivers::hololink_i2c ALIAS hcl_hololink_i2c)

target_include_directories(hcl_hololink_i2c
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/interfaces/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(hcl_hololink_i2c
    PUBLIC hcl_core hololink::core
)

target_compile_features(hcl_hololink_i2c PUBLIC cxx_std_20)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/hcl/drivers/
    DESTINATION include/hcl/drivers/
)

install(TARGETS hcl_hololink_i2c
    EXPORT hclTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
