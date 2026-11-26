option(HCL_BUILD_SENSORS "Build optional sensor drivers" ON)

if(HCL_BUILD_SENSORS)
    add_subdirectory(camera)
endif()
