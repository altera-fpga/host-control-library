option(HCL_VVP_BLC "Build the VVP BLC driver library" ON)
option(HCL_VVP_SWITCH "Build the VVP Switch driver library" ON)
option(HCL_VVP_CRS "Build the VVP CRS driver library" ON)
option(HCL_VVP_SCALER "Build the VVP Scaler driver library" ON)
option(HCL_VVP_CSC "Build the VVP CSC driver library" ON)
option(HCL_VVP_DEMOSAIC "Build the VVP Demosaic driver library" ON)
option(HCL_VVP_WBC "Build the VVP WBC driver library" ON)
option(HCL_VVP_TPG "Build the VVP TPG driver library" ON)
option(HCL_VVP_PIP_CONV "Build the VVP PIP converter driver library" ON)

add_subdirectory(core)

if(HCL_VVP_BLC)
    add_subdirectory(blc)
endif()

if(HCL_VVP_SWITCH)
    add_subdirectory(switch)
endif()

if(HCL_VVP_CRS)
    add_subdirectory(crs)
endif()

if(HCL_VVP_SCALER)
    add_subdirectory(scaler)
endif()

if(HCL_VVP_CSC)
    add_subdirectory(csc)
endif()

if(HCL_VVP_DEMOSAIC)
    add_subdirectory(demosaic)
endif()

if(HCL_VVP_WBC)
    add_subdirectory(wbc)
endif()

if(HCL_VVP_TPG)
    add_subdirectory(tpg)
endif()

if(HCL_VVP_PIP_CONV)
    add_subdirectory(pip_conv)
endif()
