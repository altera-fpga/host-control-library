include("${HCL_CMAKE_MODULE_DIR}/hclPython.cmake")

add_subdirectory(altera_i2c)

if(HCL_BUILD_HOLOLINK)
    add_subdirectory(hololink_i2c)
endif()
