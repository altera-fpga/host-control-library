add_library(hcl_driveonchip
    src/driveonchip.c
)

if (HCL_BUILD_PYTHON)
    hcl_add_python_extension(
        TARGET _driveonchip
        SOURCES src/driveonchip_pybind.cpp
        OUTPUT_NAME "driveonchip"
        PACKAGE_DIR "${CMAKE_BINARY_DIR}/python/altera_host_control"
        LINK_LIBRARIES hcl_core hcl_driveonchip
        INCLUDE_DIRS
            ${CMAKE_SOURCE_DIR}/include
            ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
endif()

if (HCL_BUILD_CPP)
    target_sources(hcl_driveonchip PRIVATE src/driveonchip.cpp)
endif()

add_library(hcl::drivers::driveonchip ALIAS hcl_driveonchip)

target_include_directories(hcl_driveonchip
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(hcl_driveonchip
    PUBLIC hcl_core
)

if (HCL_BUILD_CPP)
    target_compile_features(hcl_driveonchip PUBLIC cxx_std_20)

    target_compile_options(hcl_driveonchip
        PRIVATE
            $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
            $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
    )
endif()

install(TARGETS hcl_driveonchip
    EXPORT hclTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(FILES
    include/hcl/drivers/driveonchip.h
    include/hcl/drivers/driveonchip_regs.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/hcl/drivers
)

if (HCL_BUILD_CPP)
    install(FILES
        include/hcl/drivers/driveonchip.hpp
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/hcl/drivers
    )
endif()