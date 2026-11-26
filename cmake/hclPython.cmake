include_guard(GLOBAL)

function(hcl_add_python_extension)
    set(options)
    set(oneValueArgs TARGET OUTPUT_NAME PACKAGE_DIR INSTALL_DIR)
    set(multiValueArgs SOURCES LINK_LIBRARIES INCLUDE_DIRS COMPILE_DEFINITIONS)
    cmake_parse_arguments(HCL_PY "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT DEFINED HCL_PY_TARGET OR HCL_PY_TARGET STREQUAL "")
        message(FATAL_ERROR "hcl_add_python_extension requires a TARGET argument.")
    endif()

    if (NOT DEFINED HCL_PY_SOURCES OR HCL_PY_SOURCES STREQUAL "")
        message(FATAL_ERROR "hcl_add_python_extension requires at least one SOURCES argument.")
    endif()

    find_package(Python3 COMPONENTS Interpreter Development.Module QUIET)
    find_package(pybind11 CONFIG QUIET)

    if (NOT Python3_Interpreter_FOUND OR NOT Python3_Development.Module_FOUND)
        message(WARNING "Python bindings are enabled, but Python3 interpreter/development headers were not found for target '${HCL_PY_TARGET}'. Skipping the target.")
        return()
    endif()

    if (NOT pybind11_FOUND)
        message(WARNING "Python bindings are enabled, but pybind11 was not found for target '${HCL_PY_TARGET}'. Skipping the target.")
        return()
    endif()

    pybind11_add_module(${HCL_PY_TARGET} ${HCL_PY_SOURCES})
    target_compile_features(${HCL_PY_TARGET} PRIVATE cxx_std_17)

    if (HCL_PY_LINK_LIBRARIES)
        target_link_libraries(${HCL_PY_TARGET} PRIVATE ${HCL_PY_LINK_LIBRARIES})
    endif()

    target_include_directories(${HCL_PY_TARGET} PRIVATE
        ${CMAKE_SOURCE_DIR}/bindings/python/include
    )

    if (HCL_PY_INCLUDE_DIRS)
        target_include_directories(${HCL_PY_TARGET} PRIVATE ${HCL_PY_INCLUDE_DIRS})
    endif()

    if (HCL_PY_COMPILE_DEFINITIONS)
        target_compile_definitions(${HCL_PY_TARGET} PRIVATE ${HCL_PY_COMPILE_DEFINITIONS})
    endif()

    if (DEFINED HCL_PY_OUTPUT_NAME AND NOT HCL_PY_OUTPUT_NAME STREQUAL "")
        set_target_properties(${HCL_PY_TARGET} PROPERTIES OUTPUT_NAME "${HCL_PY_OUTPUT_NAME}")
    endif()

    if (DEFINED HCL_PY_PACKAGE_DIR AND NOT HCL_PY_PACKAGE_DIR STREQUAL "")
        set(_hcl_python_package_dir "${HCL_PY_PACKAGE_DIR}")
    else()
        set(_hcl_python_package_dir "${CMAKE_BINARY_DIR}/python/altera_host_control")
    endif()

    set(HCL_PYTHON_PACKAGE_DIR "${_hcl_python_package_dir}" PARENT_SCOPE)

    set_target_properties(${HCL_PY_TARGET} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${_hcl_python_package_dir}"
    )

    if (DEFINED HCL_PY_INSTALL_DIR AND NOT HCL_PY_INSTALL_DIR STREQUAL "")
        set(_hcl_python_install_dir "${HCL_PY_INSTALL_DIR}")
    elseif (DEFINED HCL_PYTHON_INSTALL_PACKAGE_DIR AND NOT HCL_PYTHON_INSTALL_PACKAGE_DIR STREQUAL "")
        set(_hcl_python_install_dir "${HCL_PYTHON_INSTALL_PACKAGE_DIR}")
    elseif (DEFINED Python3_SITEARCH AND NOT Python3_SITEARCH STREQUAL "")
        set(_hcl_python_install_dir "${Python3_SITEARCH}/altera_host_control")
    else()
        set(_hcl_python_install_dir "${CMAKE_INSTALL_LIBDIR}/hcl/python/altera_host_control")
        message(WARNING "Python3_SITEARCH is unavailable. Installing Python bindings under ${_hcl_python_install_dir}.")
    endif()

    set(HCL_PYTHON_INSTALL_PACKAGE_DIR "${_hcl_python_install_dir}" PARENT_SCOPE)

    install(TARGETS ${HCL_PY_TARGET}
        LIBRARY DESTINATION "${_hcl_python_install_dir}"
    )
endfunction()
