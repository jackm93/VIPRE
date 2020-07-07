
# Add definitions for shared or static builds
IF (VIPRE_DYNAMIC_LINKING)
    ADD_DEFINITIONS(-DVIPRE_LIBRARY)
ELSE ()
    ADD_DEFINITIONS(-DVIPRE_LIBRARY_STATIC)
ENDIF ()

# Set up the lib name and header path
SET (LIB_NAME vipre)
SET (HEADER_PATH ${VIPRE_SOURCE_DIR}/include/${LIB_NAME})

# Add all the headers
SET (
    TARGET_H
    ${HEADER_PATH}/BackgroundCamera.hpp
    ${HEADER_PATH}/BoundingBoxSwitch.hpp
	${HEADER_PATH}/ColorTables.hpp
    ${HEADER_PATH}/DisplayCamera.hpp
    ${HEADER_PATH}/Enums.hpp
	${HEADER_PATH}/Environment.hpp
    ${HEADER_PATH}/Export.hpp
    ${HEADER_PATH}/Log.hpp
    ${HEADER_PATH}/NotificationCenter.hpp
    ${HEADER_PATH}/OpacityTables.hpp
    ${HEADER_PATH}/RenderCamera.hpp
	${HEADER_PATH}/ShaderHandler.hpp
    ${HEADER_PATH}/String.hpp
	${HEADER_PATH}/TexturedCuboid.hpp
    ${HEADER_PATH}/TexturedQuad.hpp
    ${HEADER_PATH}/TrackballManipulator.hpp
    ${HEADER_PATH}/Version.hpp
	${HEADER_PATH}/vipre.hpp
)

# Add all the source files
SET (
    TARGET_SRC
    BackgroundCamera.cpp
    BoundingBoxSwitch.cpp
	ColorTables.cpp
    DisplayCamera.cpp
	Environment.cpp
    Log.cpp
    NotificationCenter.cpp
	OpacityTables.cpp
    RenderCamera.cpp
	ShaderHandler.cpp
    String.cpp
	TexturedCuboid.cpp
    TexturedQuad.cpp
    TrackballManipulator.cpp
    Version.cpp
)

# Finish setting up the library
SETUP_LIBRARY (${LIB_NAME})
