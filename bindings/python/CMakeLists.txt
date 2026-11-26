include("${HCL_CMAKE_MODULE_DIR}/hclPython.cmake")

set(_hcl_python_compile_definitions)
if (HCL_BUILD_HOLOLINK)
    list(APPEND _hcl_python_compile_definitions HCL_PYTHON_HAS_HOLOLINK=1)
endif()

if (HCL_BUILD_UDP)
    list(APPEND _hcl_python_compile_definitions HCL_PYTHON_HAS_UDP=1)
endif()

if (HCL_BUILD_UIO)
    list(APPEND _hcl_python_compile_definitions HCL_PYTHON_HAS_UIO=1)
endif()

hcl_add_python_extension(
    TARGET _core
    SOURCES src/hcl_pybind.cpp
    OUTPUT_NAME "core"
    PACKAGE_DIR "${CMAKE_BINARY_DIR}/python/altera_host_control"
    INSTALL_DIR "${HCL_PYTHON_INSTALL_PACKAGE_DIR}"
    LINK_LIBRARIES hcl_core
    INCLUDE_DIRS
        ${CMAKE_SOURCE_DIR}/include
    COMPILE_DEFINITIONS ${_hcl_python_compile_definitions}
)

set(HCL_PYTHON_PACKAGE_DIR "${CMAKE_BINARY_DIR}/python/altera_host_control")
file(MAKE_DIRECTORY "${HCL_PYTHON_PACKAGE_DIR}")
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/altera_host_control/__init__.py"
    "${HCL_PYTHON_PACKAGE_DIR}/__init__.py"
    COPYONLY
)

file(COPY
    "${CMAKE_CURRENT_SOURCE_DIR}/altera_host_control/drivers"
    "${CMAKE_CURRENT_SOURCE_DIR}/altera_host_control/sensors"
    DESTINATION "${HCL_PYTHON_PACKAGE_DIR}"
)

install(FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/altera_host_control/__init__.py"
    DESTINATION "${HCL_PYTHON_INSTALL_PACKAGE_DIR}"
)

install(DIRECTORY
    "${CMAKE_CURRENT_SOURCE_DIR}/altera_host_control/drivers"
    "${CMAKE_CURRENT_SOURCE_DIR}/altera_host_control/sensors"
    DESTINATION "${HCL_PYTHON_INSTALL_PACKAGE_DIR}"
)
