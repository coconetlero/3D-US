#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 2.8.0)

#-----------------------------------------------------------------------------

if(NOT Slicer_SOURCE_DIR)
  set(EXTENSION_NAME 3DFreehandUS)
  set(EXTENSION_HOMEPAGE "www.ccadet.unam.mx")
  set(EXTENSION_CATEGORY "Ultrasound")
  set(EXTENSION_CONTRIBUTORS "Zian Fanti (UNAM)")
  set(EXTENSION_DESCRIPTION "3D Freehand Ultrasound capabilities extension")
  set(EXTENSION_ICONURL "")
  set(EXTENSION_SCREENSHOTURLS "http://wiki.slicer.org/slicerWiki/images/4/42/Slicer-r19441-USProbeCalibration_Slicer-screenshot.png")
endif()

#-----------------------------------------------------------------------------

project(3DFreehandUS)

#-----------------------------------------------------------------------------

if(NOT Slicer_SOURCE_DIR)
  find_package(Slicer REQUIRED)
  include(${Slicer_USE_FILE})
endif()

#-----------------------------------------------------------------------------

add_subdirectory(CropImages)
#add_subdirectory(ProbeCalibration)
#add_subdirectory(VolumeReconstruction)
add_subdirectory(StructureEnhancer)
# add_subdirectory(GraphCutSegmentation)
add_subdirectory(ICPRegistration)
add_subdirectory(UltrasoundSegmentation)
add_subdirectory(VtkGlue)

#-----------------------------------------------------------------------------
if(NOT Slicer_SOURCE_DIR)  
  include(${Slicer_EXTENSION_CPACK}) 
endif() 

#-----------------------------------------------------------------------------
if( APPLE )
  if( NOT CMAKE_FIRST_PASS_COMPLETED )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-common"
      CACHE STRING "Flags used by all build types." FORCE )
    set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-common"
      CACHE STRING "Flags used by all build types." FORCE )
  endif( NOT CMAKE_FIRST_PASS_COMPLETED )
endif( APPLE )



