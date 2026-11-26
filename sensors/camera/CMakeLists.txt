option(HCL_SENSOR_IMX678 "Build the IMX678 camera sensor driver" ON)
option(HCL_SENSOR_FRAMOS_GMSL "Build the FRAMOS GMSL camera bridge driver" ON)

if(HCL_SENSOR_IMX678)
    add_subdirectory(imx678)
endif()

if(HCL_SENSOR_FRAMOS_GMSL)
    add_subdirectory(framos_gmsl)
endif()
