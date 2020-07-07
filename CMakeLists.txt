
# Set the minimum required version of CMake
CMAKE_MINIMUM_REQUIRED (VERSION 2.8)

# Project settings

PROJECT (VIPRE)
SET (VIPRE_MAJOR_VERSION 0)
SET (VIPRE_MINOR_VERSION 3)
SET (VIPRE_PATCH_VERSION 0)
SET (VIPRE_SOVERSION 3)
SET (VIPRE_VERSION ${VIPRE_MAJOR_VERSION}.${VIPRE_MINOR_VERSION}.${VIPRE_PATCH_VERSION})

# Include all the header directories
INCLUDE_DIRECTORIES (
    ${VIPRE_SOURCE_DIR}/include
)

# Make the headers visible to everything
IF (NOT ${PROJECT_BINARY_DIR} EQUAL ${PROJECT_SOURCE_DIR})
   INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR}/include)
ENDIF()

# Set up the vipre Version file
SET (VIPRE_VERSION_HEADER "${PROJECT_SOURCE_DIR}/include/vipre/Version.hpp")
CONFIGURE_FILE ("${PROJECT_SOURCE_DIR}/src/vipre/Version.in" ${VIPRE_VERSION_HEADER})

# Set up the vipreDICOM Version file
SET (VIPREDICOM_VERSION_HEADER "${PROJECT_SOURCE_DIR}/include/vipreDICOM/Version.hpp")
CONFIGURE_FILE ("${PROJECT_SOURCE_DIR}/src/vipreDICOM/Version.in" ${VIPREDICOM_VERSION_HEADER})

# Add the custom macro utilities
SET (CMAKE_MODULE_PATH "${VIPRE_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")
INCLUDE (VIPREMacroUtils)

# Set whether to use dynamic or static linking
OPTION (VIPRE_DYNAMIC_LINKING "Set to ON to build VIPRE with dynamic linking (shared libraries). Use OFF for static." ON)
IF (VIPRE_DYNAMIC_LINKING OR VIPRE_COMPILE_FRAMEWORKS)
    SET (VIPRE_USER_DEFINED_DYNAMIC_OR_STATIC "SHARED")
ELSE ()
    SET (VIPRE_USER_DEFINED_DYNAMIC_OR_STATIC "STATIC")
ENDIF()

# If on an apple device, set whether to compile frameworks
IF (APPLE)
    OPTION (VIPRE_COMPILE_FRAMEWORKS "Compile frameworks instead of dylibs" ON)
    SET (VIPRE_INSTALL_NAME_DIR "@executable_path/../Frameworks"
        CACHE STRING "install name dir for compiled frameworks"
    )
ENDIF ()

# If compiling with Visual Studio, set up some specific flags
IF (WIN32 AND MSVC)

    # This option is to enable the /MP switch for Visual Studio 2005 and above compilers
    OPTION (WIN32_USE_MP "Set to ON to build VIPRE with the /MP option (Visual Studio 2005 and above)." OFF)
    MARK_AS_ADVANCED (WIN32_USE_MP)
    IF (WIN32_USE_MP)
        SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
    ENDIF ()

    # Some specific compiler flags to disable some warnings
    ADD_DEFINITIONS (-D_SCL_SECURE_NO_WARNINGS)
    ADD_DEFINITIONS (-D_CRT_SECURE_NO_DEPRECATE)

ENDIF ()

# Find all the dependency packages
FIND_PACKAGE (Boost 1.47.0 REQUIRED)
FIND_PACKAGE (DCMTK 3.6.0 REQUIRED)
FIND_PACKAGE (OSG 3.0.1 REQUIRED)
FIND_PACKAGE (Qt4)

# Add the src subdirectory
ADD_SUBDIRECTORY (src)

# Set whether to build the examples
OPTION (VIPRE_BUILD_EXAMPLES "Enable to build VIPRE Examples" ON)
IF (VIPRE_BUILD_EXAMPLES)
    ADD_SUBDIRECTORY (examples)
ENDIF ()
