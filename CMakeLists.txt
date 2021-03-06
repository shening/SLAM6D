cmake_minimum_required (VERSION 2.8.2)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/3rdparty/CMakeModules" ${CMAKE_MODULE_PATH})
project (3DTK)

# Hide CMake variables
set (CMAKE_INSTALL_PREFIX "/usr/local" CACHE INTERNAL "" FORCE)
set (CMAKE_BUILD_TYPE "" CACHE INTERNAL "" FORCE)

# being able to set the output directory to a different one than the default
# (the source directory) is important for platforms like windows, where the
# produced binaries cannot be run if being placed in certain locations (like on
# a network drive)
set(OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}" CACHE PATH "The directory where the output will be placed into lib, obj and bin subdirectories (default: ${CMAKE_SOURCE_DIR})" )

# Set output directories for libraries and executables
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY}/lib )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY}/obj )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY}/bin )

#include_directories(OPENGL_INCLUDE_DIR)
if(WIN32)
  set(Boost_USE_STATIC_LIBS TRUE)
else()
  set(Boost_USE_STATIC_LIBS FALSE)
endif()

set(Boost_ADDITIONAL_VERSIONS "1.42" "1.42.0" "1.44" "1.44.0" "1.45.0" "1.45" "1.46" "1.46.1" "1.47.0" "1.47" "1.48" "1.50" "1.52" "1.53" "1.55" "1.56")
if(WIN32)
  # for some unknown reason no one variant works on all windows platforms
  set(Boost_DEBUG 1)  
  find_package( Boost COMPONENTS serialization graph regex filesystem system thread chrono date_time program_options system REQUIRED)
else()
  find_package( Boost COMPONENTS serialization graph regex filesystem system thread date_time program_options system REQUIRED)
endif()

if(Boost_FOUND)
  link_directories(${BOOST_LIBRARY_DIRS})
  include_directories(${Boost_INCLUDE_DIRS})
  add_definitions(${Boost_LIB_DIAGNOSTIC_DEFINITIONS})
endif()

if(NOT WIN32)
  find_package(LibZip REQUIRED)
  if(${LIBZIP_VERSION} VERSION_LESS 0.11.2)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLIBZIP_OLD")
  endif()
  find_package(ANN REQUIRED)
endif()

find_package(OpenCV REQUIRED)
if(EXISTS "${OpenCV_DIR}/OpenCVConfig.cmake")
  include("${OpenCV_DIR}/OpenCVConfig.cmake")
  set(ADDITIONAL_OPENCV_FLAGS
                          "-DCV_MINOR_VERSION=${OpenCV_VERSION_MINOR} -DCV_MAJOR_VERSION=${OpenCV_VERSION_MAJOR}"
                          CACHE STRING "OpenCV Version Defines)"
                          )
  ## Include the standard CMake script
else()
  set(ADDITIONAL_OPENCV_FLAGS
                          ""
                          CACHE STRING "OpenCV Version Defines (BLUB)"
                          )
endif()
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ADDITIONAL_OPENCV_FLAGS}")

#################################################
# Declare Options and modify build accordingly ##
#################################################


function(ENFORCE_OPTION_DEP_3DTK option VALUE)
  set (${option} "${VALUE}" CACHE BOOL "${${option}_DESCRIPTION}" FORCE) # this option set to VALUE as advised
  
  #now make sure other dependencies are also true
  foreach(d ${${option}_DEPENDENCIES}) # look through all my dependencies
    string(REGEX REPLACE " +" ";" CMAKE_DEPENDENT_OPTION_DEP "${d}")

    # check for a not in front
    string(STRIP "${CMAKE_DEPENDENT_OPTION_DEP}" CMAKE_DEPENDENT_OPTION_DEP)
    string(SUBSTRING "${CMAKE_DEPENDENT_OPTION_DEP}" 0 3 CMAKE_DEPENDENT_OPTION_DEP_3)
    string(TOUPPER "${CMAKE_DEPENDENT_OPTION_DEP_3}" CMAKE_DEPENDENT_OPTION_DEP_3)
    string(COMPARE EQUAL "${CMAKE_DEPENDENT_OPTION_DEP_3}" "NOT" CMAKE_DEPENDENT_OPTION_DEP_NOT)
    #string(REPLACE "NOT " "" CMAKE_DEPENDENT_OPTION_DEP "${d}")
    if(CMAKE_DEPENDENT_OPTION_DEP_NOT) # we found a NOT
      string(REPLACE "NOT;" "" CMAKE_DEPENDENT_OPTION_DEP "${CMAKE_DEPENDENT_OPTION_DEP}")
      if(${CMAKE_DEPENDENT_OPTION_DEP})  # not met, make sure it is
        ENFORCE_OPTION_DEP_3DTK(${CMAKE_DEPENDENT_OPTION_DEP} OFF)
      else() # dependency is met
      endif()
    else()
      if(${CMAKE_DEPENDENT_OPTION_DEP})  # dependency is met
      else() # not met, make sure it is
        ENFORCE_OPTION_DEP_3DTK(${CMAKE_DEPENDENT_OPTION_DEP} ON)
      endif()
    endif()
  endforeach()
  
endfunction()

macro(OPT_DEP option doc default depends)
  option(${option} "${doc}" "${default}")
  set(${option} "${${option}}" CACHE BOOL "${doc}" FORCE)
  set(${option}_DEPENDENCIES "${depends}" CACHE INTERNAL "" FORCE)
  set(${option}_DESCRIPTION "${doc}" CACHE INTERNAL "" FORCE)

  if (${option})
    #message(STATUS "Yes ${option} is true")
    #  message("FOREACH d in ${depends}")

    foreach(d ${depends})
      string(REGEX REPLACE " +" ";" CMAKE_DEPENDENT_OPTION_DEP "${d}")

      # check for a not in front
      string(STRIP "${CMAKE_DEPENDENT_OPTION_DEP}" CMAKE_DEPENDENT_OPTION_DEP)
      string(SUBSTRING "${CMAKE_DEPENDENT_OPTION_DEP}" 0 3 CMAKE_DEPENDENT_OPTION_DEP_3)
      string(TOUPPER "${CMAKE_DEPENDENT_OPTION_DEP_3}" CMAKE_DEPENDENT_OPTION_DEP_3)
      string(COMPARE EQUAL "${CMAKE_DEPENDENT_OPTION_DEP_3}" "NOT" CMAKE_DEPENDENT_OPTION_DEP_NOT)
      if(CMAKE_DEPENDENT_OPTION_DEP_NOT) # we found a NOT
        string(REPLACE "NOT;" "" CMAKE_DEPENDENT_OPTION_DEP "${CMAKE_DEPENDENT_OPTION_DEP}")
        if(${CMAKE_DEPENDENT_OPTION_DEP})  # not met, make sure it is
          ENFORCE_OPTION_DEP_3DTK(${CMAKE_DEPENDENT_OPTION_DEP} OFF)
        else() # dependency is met
        endif()
      else()
        if(${CMAKE_DEPENDENT_OPTION_DEP})  # dependency is met
        else() # not met, make sure it is
          ENFORCE_OPTION_DEP_3DTK(${CMAKE_DEPENDENT_OPTION_DEP} ON)
        endif()
      endif()
    endforeach()

  endif()
endmacro()



## FreeGLUT
OPT_DEP(WITH_FREEGLUT "Whether freeglut is available. This enables iterative drawing in show. ON/OFF" ON "")

if(WITH_FREEGLUT)
  message(STATUS "With freeglut")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWITH_FREEGLUT")
else()
  message(STATUS "Without freeglut")
endif()

## Show
OPT_DEP(WITH_SHOW "Whether to build Show. This is the Visualization program of slam6d. ON/OFF" ON "" )

if(WITH_SHOW)
  message(STATUS "With show")
else()
  #  set (WITH_OCTREE_DISPLAY "ON" CACHE INTERNAL "" FORCE)
  message(STATUS "Without show")
endif()

## WXShow
OPT_DEP(WITH_WXSHOW "Whether to build WXShow. This is the wxwidgets variant of Show. ON/OFF" OFF "")

if(WITH_WXSHOW)
  find_package(wxWidgets COMPONENTS core base gl REQUIRED)
  # set wxWidgets_wxrc_EXECUTABLE to be ignored in the configuration
  set (wxWidgets_wxrc_EXECUTABLE " " CACHE INTERNAL "" FORCE)
  # wxWidgets include (this will do all the magic to configure everything)
  include( ${wxWidgets_USE_FILE})
  message(STATUS "With wxshow")
else()
  message(STATUS "Without wxshow")
endif()

## GLUT and OpenGL

if(WITH_FREEGLUT OR WITH_SHOW OR WITH_WXSHOW)
  find_package(OpenGL REQUIRED)
  find_package(GLUT REQUIRED)
  include_directories(${GLUT_INCLUDE_DIR})
endif()

## Shapes
OPT_DEP(WITH_SHAPE_DETECTION "Whether to build shapes and planes executable for detecting planes. ON/OFF" OFF "")
if(WITH_SHAPE_DETECTION)
  message(STATUS "With shape detection")
else()
  message(STATUS "Without shape detection")
endif()

OPT_DEP(WITH_FLOORPLAN "Whether to build floorplan generation from 3D scans.  ON/OFF" OFF "")
if(WITH_FLOORPLAN)
  message(STATUS "With floorplan generation")
else()
  message(STATUS "Without floorplan generation")
endif()


## Interior reconstruction
OPT_DEP(WITH_MODEL "Whether to build model executable for modelling interior environments. ON/OFF" OFF "")

if(WITH_MODEL)
  message(STATUS "With interior reconstruction")
else()
  message(STATUS "Without interior reconstruction")
endif()

## Thermo
OPT_DEP(WITH_THERMO "Whether to build executables for mutual calibration of laser scanner and camera. ON/OFF" OFF "WITH_SHAPE_DETECTION")
if(WITH_THERMO)
  add_subdirectory(3rdparty/cvblob)
  include_directories(${CMAKE_SOURCE_DIR}/3rdparty/cvblob)
  link_directories(${CMAKE_SOURCE_DIR}/3rdparty/cvblob)
  message(STATUS "With thermo")
else()
  message(STATUS "Without thermo")
endif()

## OpenCV non-free
OPT_DEP(WITH_OPENCV_NONFREE "Whether to use non-free (patent encumbered) OpenCV functionalities ON/OFF" OFF "")
if(WITH_OPENCV_NONFREE)
  message(STATUS "Using non-free OpenCV functionality")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWITH_OPENCV_NONFREE")
else()
  message(STATUS "Not using non-free OpenCV functionality")
endif()

## Octree
OPT_DEP(WITH_COMPACT_OCTREE "Whether to use the compact octree display ON/OFF" OFF "")

if(WITH_COMPACT_OCTREE)
  message(STATUS "Using compact octrees")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_COMPACT_TREE")
else()
  message(STATUS "Not using compact octreees: Warning uses more memory")
endif()

## Glee?
OPT_DEP(WITH_GLEE "Whether to use OpenGL extensions, requires glee. ON/OFF" OFF "")

if(WITH_GLEE)
  message(STATUS "Using opengl extensions")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWITH_GLEE")
else()
  message(STATUS "Not using opengl extensions")
endif()

## Gridder
OPT_DEP(WITH_GRIDDER "Whether to build the 2DGridder binary ON/OFF" OFF "")

if(WITH_GRIDDER)
   find_package (Boost COMPONENTS system REQUIRED)
  message(STATUS "With 2DGridder")
else()
  message(STATUS "Without 2DGridder")
endif()

## Dynamic VELOSLAM
OPT_DEP(WITH_VELOSLAM "Whether to build the Velodyne data processing (veloslam/veloshow) ON/OFF" OFF "WITH_SHOW")

if(WITH_VELOSLAM)
  message(STATUS "With VELOSLAM")
else()
  message(STATUS "Without VELOSLAM")
endif()

## Home-made Laserscanner
OPT_DEP(WITH_DAVID_3D_SCANNER "Whether to build the David scanner app for homemade laser scanners binary ON/OFF" OFF "")

if(WITH_DAVID_3D_SCANNER)
  message(STATUS "With David scanner")
else()
  message(STATUS "Without David scanner")
endif()

## Tools

OPT_DEP(WITH_TOOLS "Whether to build additional tools like convergence frame_to_graph etc. ON/OFF" OFF "")

if(WITH_TOOLS)
  message(STATUS "With Tools")
  # seems to override previous find_package command resulting in linker errors, either provide ALL necessary libs again or do not use
  #find_package (Boost COMPONENTS serialization graph regex filesystem system thread chrono date_time program_options system REQUIRED)
else()
  message(STATUS "Without Tools")
endif()

## Segmentation

OPT_DEP(WITH_SEGMENTATION "Whether to build scan segmantion program ON/OFF" OFF "WITH_FBR")

if(WITH_SEGMENTATION)
  message(STATUS "With segmentation")
  find_package (Boost COMPONENTS program_options REQUIRED)
else()
  message(STATUS "Without segmentation")
endif()

## LASlib

OPT_DEP(WITH_LASLIB "Whether to build LASlib based scanio library" OFF WITH_LASLIB)

if(WITH_LASLIB)
  message(STATUS "With LASlib")
else()
  message(STATUS "Without LASlib")
endif()

## Collision detection

OPT_DEP(WITH_COLLISION "Whether to build collision detection tools" OFF "WITH_COLLISION")

if(WITH_COLLISION)
  message(STATUS "With collision detection")
  # as in WITH_TOOLS, seems to override previous find_package command resulting in linker errors, either provide ALL necessary libs again or do not use
  # find_package (Boost COMPONENTS program_options REQUIRED)
else()
  message(STATUS "Without collision detection")
endif()

## CUDA accelerated collision detection
OPT_DEP(WITH_CUDA "Whether to build CUDA accelerated collision detection tools" OFF "WITH_CUDA")

if(WITH_CUDA)
  find_package(CUDA REQUIRED)         
  include_directories(${FOUND_CUDA_NVCC_INCLUDE})
  
  add_definitions(-DWITH_CUDA)
  
  #owerrite users input - need to be fixed
  #select all NVIDIA GPU arch which support unified memory (CUDA toolkit >= 6.0) and arch>=30
  set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS};-gencode arch=compute_30,code=sm_30;-gencode arch=compute_35,code=sm_35;-gencode arch=compute_50,code=sm_50;-gencode arch=compute_52,code=sm_52)
  
  message("CUDA_NVCC_FLAGS = ${CUDA_NVCC_FLAGS}")
  
  message(STATUS "With CUDA accelerated collision detection")
else()
  message(STATUS "Without CUDA accelerated collision detection")
endif()


## RivLib
OPT_DEP(WITH_RIVLIB "Whether the RIEGL rivlib is present ON/OFF" OFF "")

if(WITH_RIVLIB)
  message(STATUS "Compiling a scan IO for RXP files")
  set(RIEGL_DIR ${CMAKE_SOURCE_DIR}/3rdparty/riegl/)
  include_directories(${CMAKE_SOURCE_DIR}/3rdparty)
  if(WIN32)
    set(RIVLIB ${RIEGL_DIR}libscanlib-mt.lib ${RIEGL_DIR}libctrllib-mt.lib ${RIEGL_DIR}libboost_system-mt-1_43_0-vns.lib)
  else()
    set(RIVLIB ${RIEGL_DIR}libscanlib-mt-s.a ${RIEGL_DIR}libctrllib-mt-s.a ${RIEGL_DIR}libboost_system-mt-s-1_43_0-vns.a pthread)
  endif()
  find_package(LibXml2 REQUIRED)

else()
  message(STATUS "Do NOT compile a scan IO for RXP")
endif()

## PMD
OPT_DEP(WITH_PMD "Whether to build the PMD tools like grabVideoAnd3D calibrate etc. ON/OFF" OFF "")

if(WITH_PMD)
  find_package(OpenGL REQUIRED)
  message(STATUS "With PMD")
else()
  message(STATUS "Without PMD")
endif()

## FBR
OPT_DEP(WITH_FBR "Whether to compile feature based registration. ON/OFF" OFF "")

if(WITH_FBR)
  message(STATUS "With FBR ")
else()
  message(STATUS "Without FBR")
endif()

# OPEN
find_package(OpenMP)
if(OPENMP_FOUND)
  OPT_DEP(WITH_OPENMP "Whether to use parallel processing capabilities of OPENMP. ON/OFF" ON "")
endif()

OPT_DEP(EXPORT_SHARED_LIBS "Whether to build additional shared libraries for use in other projects. ON/OFF" OFF "WITH_SHOW;WITH_FBR")
#OPT_DEP(EXPORT_SHARED_LIBS "Whether to build additional shared libraries for use in other projects. ON/OFF" OFF "WITH_SHOW")

if(EXISTS "$ENV{ROS_ROOT}")
  set(WITH_FBR ON)
  set(EXPORT_SHARED_LIBS ON)
endif()

if(EXPORT_SHARED_LIBS)
  ## Compile a single shared library containing all of 3DTK
  add_library(slam SHARED src/slam6d/icp6D.cc)
  if (WIN32)
  target_link_libraries(slam scan_s ANN_s cxsparse_s newmat_s show_s fbr_s)
  else()
  target_link_libraries(slam scan_s ann cxsparse newmat show_s fbr_s)
  endif()
  message(STATUS "exporting additional libraries")
else()
  message(STATUS "not exporting libraries")
endif()


OPT_DEP(WITH_METRICS "Whether to use metrics in slam6d. ON/OFF" OFF "")

if(WITH_METRICS)
  message(STATUS "With metrics in slam6d.")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWITH_METRICS")
else()
  message(STATUS "Without metrics in slam6d.")
endif()

OPT_DEP(WITH_ADDONS "Whether to download and use addons to 3DTK. ON/OFF" OFF "")
if(WITH_ADDONS)
  message(STATUS "Compiling addons directory")
  #execute_process(COMMAND svn co https://robotik.informatik.uni-wuerzburg.de/slam6dprivate/trunk/addons addons 
  #                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  #                OUTPUT_QUIET
  #                ) 
  message(STATUS "With addons.")
else()
  message(STATUS "Without addons.")
endif()

OPT_DEP(WITH_TESTING "Whether to download and use testing data to 3DTK. ON/OFF" OFF "")
if(WITH_TESTING)
  execute_process(COMMAND svn co https://robotik.informatik.uni-wuerzburg.de/testing testing 
                  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                  OUTPUT_QUIET
                  ) 
  message(STATUS "With testing.")
else()
  message(STATUS "Without testing.")
endif()

#################################################
# OPERATING SYSTEM SPECIFIC BEHAVIOUR          ##
#################################################

## Special treatment for system specifics
if(APPLE)
add_definitions(-Dfopen64=fopen)
endif()

## Multiple Cores
include(ProcessorCount)
ProcessorCount(PROCESSOR_COUNT)
if(NOT PROCESSOR_COUNT EQUAL 0)
  set(NUMBER_OF_CPUS "${PROCESSOR_COUNT}" CACHE STRING "The number of processors to use (default: ${PROCESSOR_COUNT})" )
else()
  set(NUMBER_OF_CPUS "1" CACHE STRING "The number of processors to use (default: 1)" )
endif()

# OPENMP_NUM_THREADS
if(OPENMP_FOUND AND WITH_OPENMP)
  message(STATUS "With OpenMP ")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMAX_OPENMP_NUM_THREADS=${NUMBER_OF_CPUS} -DOPENMP_NUM_THREADS=${NUMBER_OF_CPUS} ${OpenMP_CXX_FLAGS} -DOPENMP")
else()
  message(STATUS "Without OpenMP")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMAX_OPENMP_NUM_THREADS=1 -DOPENMP_NUM_THREADS=1")
endif()

if(WIN32)
  include_directories(${CMAKE_SOURCE_DIR}/3rdparty/ann_1.1.1_modified/include)
  include_directories(${CMAKE_SOURCE_DIR}/3rdparty/cxsparse-4.4.4)
  include_directories(${CMAKE_SOURCE_DIR}/3rdparty/newmat-10)
  include_directories(${CMAKE_SOURCE_DIR}/3rdparty/windows/)
  include_directories(${CMAKE_SOURCE_DIR}/3rdparty/windows/zlib/)
  include_directories(${CMAKE_SOURCE_DIR}/3rdparty/libzip-1.0.1/lib/)
  link_directories(${CMAKE_SOURCE_DIR}/3rdparty/windows)
  link_directories(${BOOST_LIBRARYDIR})
  if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    link_directories(${CMAKE_SOURCE_DIR}/3rdparty/windows/freeglut/lib/x64/)
  else ()
    link_directories(${CMAKE_SOURCE_DIR}/3rdparty/windows/freeglut/lib/)
  endif ()
  add_library(XGetopt STATIC ${CMAKE_SOURCE_DIR}/3rdparty/windows/XGetopt.cpp)
  set(CMAKE_STATIC_LIBRARY_SUFFIX "32.lib")
  # to be able to use numeric_limits<int>::max() and friends on windows
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNOMINMAX")
else()
  # Add include path for OpenGL without GL/-prefix
  # to avoid the include incompatibility between MACOS
  # and linux
    find_path(OPENGL_INC gl.h /usr/include/GL)
    include_directories(${OPENGL_INC})
endif()

if (${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
  # using Clang
  set(ADDITIONAL_CFLAGS "-O3 -std=c++0x -msse3 -Wall -Wno-write-strings -Wno-char-subscripts -Wno-unused-result" CACHE STRING "Additional flags given to the compiler (-O3 -Wall -Wno-write-strings)" )
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
  # using GCC
  set(ADDITIONAL_CFLAGS "-O3 -std=c++0x -msse3 -Wall -finline-functions -Wno-unused-but-set-variable -Wno-write-strings -Wno-char-subscripts -Wno-unused-result" CACHE STRING "Additional flags given to the compiler (-O3 -Wall -finline-functions -Wno-write-strings)" )
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL Intel)
  # using Intel C++
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
  # using Visual Studio C++
  set(ADDITIONAL_CFLAGS "-O2" CACHE STRING "Additional flags given to the compiler ( -O2)" )
endif()

# Add OpenGL includes for MACOS if needed
# The OSX OpenGL frameworks natively supports freeglut extensions
if(APPLE)
  include_directories(/System/Library/Frameworks/GLUT.framework/Headers)
  include_directories(/System/Library/Frameworks/OpenGL.framework/Headers)
endif()

# hack to "circumvent" Debug and Release folders that are created under visual studio
# this is why the INSTALL target has to be used in visual studio
if(MSVC)
  install(DIRECTORY ${CMAKE_SOURCE_DIR}/bin/Release/ CONFIGURATIONS Release DESTINATION ${CMAKE_SOURCE_DIR}/windows FILES_MATCHING PATTERN "*.dll" PATTERN "*.exe")
  if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
	  install(DIRECTORY ${CMAKE_SOURCE_DIR}/3rdparty/windows/x64/ CONFIGURATIONS Release DESTINATION ${CMAKE_SOURCE_DIR}/windows FILES_MATCHING PATTERN "*.dll" PATTERN "*.exe")
  else()
	  install(DIRECTORY ${CMAKE_SOURCE_DIR}/3rdparty/windows/ CONFIGURATIONS Release DESTINATION ${CMAKE_SOURCE_DIR}/windows FILES_MATCHING PATTERN "*.dll" PATTERN "*.exe")
  endif()

  install(DIRECTORY ${CMAKE_SOURCE_DIR}/bin/Debug/ CONFIGURATIONS Debug DESTINATION ${CMAKE_SOURCE_DIR}/windows FILES_MATCHING PATTERN "*.dll" PATTERN "*.exe")
  if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/3rdparty/windows/x64/ CONFIGURATIONS Debug DESTINATION ${CMAKE_SOURCE_DIR}/windows FILES_MATCHING PATTERN "*.dll" PATTERN "*.exe")
  else()
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/3rdparty/windows/ CONFIGURATIONS Debug DESTINATION ${CMAKE_SOURCE_DIR}/windows FILES_MATCHING PATTERN "*.dll" PATTERN "*.exe")
  endif()
endif()



#################################################
# GENERAL PROJECT SETTINGS                     ##
#################################################

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ADDITIONAL_CFLAGS}")


# Set include and link dirs ...
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/3rdparty/glui-2.36/include/)
include_directories(${CMAKE_SOURCE_DIR}/3rdparty/wxthings/include/)
link_directories(${CMAKE_SOURCE_DIR}/obj)
link_directories(${CMAKE_SOURCE_DIR}/lib)

add_subdirectory(src/slam6d)
add_subdirectory(src/scanio)
add_subdirectory(src/scanserver)
add_subdirectory(src/segmentation)
add_subdirectory(src/normals)
add_subdirectory(src/veloslam)
add_subdirectory(src/show)
add_subdirectory(src/grid)
add_subdirectory(src/pmd)
add_subdirectory(src/shapes)
add_subdirectory(src/floorplan)
add_subdirectory(src/thermo)
add_subdirectory(src/slam6d/fbr)
add_subdirectory(src/scanner)
add_subdirectory(src/model)
add_subdirectory(src/collision)
add_subdirectory(src/cuda)
add_subdirectory(3rdparty)
if(WITH_ADDONS)
  message(STATUS "With 3dtkaddons.")
  add_subdirectory(addons)
endif()
if(WITH_TESTING)
  message(STATUS "With 3dtk testing.")
  find_package(Boost COMPONENTS system filesystem unit_test_framework REQUIRED)
  enable_testing()
  add_subdirectory(testing/scanio)
  add_subdirectory(testing/kdtree)
endif()


message (STATUS "Build environment is set up!")
