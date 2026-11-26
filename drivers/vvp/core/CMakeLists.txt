add_library(hcl_altera_vvp_core INTERFACE)

target_include_directories(hcl_altera_vvp_core
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(hcl_altera_vvp_core
    INTERFACE hcl_core
)

if(HCL_BUILD_CPP)
    target_compile_features(hcl_altera_vvp_core INTERFACE cxx_std_20)
endif()

install(TARGETS hcl_altera_vvp_core
    EXPORT hclTargets
)

install(FILES
    include/hcl/drivers/altera_vvp_core.h
    include/hcl/drivers/altera_vvp_core.hpp
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/hcl/drivers
)
