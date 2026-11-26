option(HCL_I2C "Build the I2C driver library" ON)
option(HCL_RDMA "Build the RDMA driver libraries" ON)
option(HCL_PIO "Build the Altera Avalon PIO driver library" ON)
option(HCL_VVP "Build the VVP driver libraries" ON)
option(HCL_DRIVEONCHIP "Build the DriveOnChip driver library" ON)

if(HCL_I2C)
    add_subdirectory(i2c)
endif()

if(HCL_RDMA)
    add_subdirectory(rdma)
endif()

if(HCL_PIO)
    add_subdirectory(pio)
endif()

if(HCL_VVP)
    add_subdirectory(vvp)
endif()

if(HCL_DRIVEONCHIP)
    add_subdirectory(driveonchip)
endif()
