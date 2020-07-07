
# Only compile if we found DCMTK
IF (DCMTK_FOUND)

    # Add definitions for shared or static builds
    IF (VIPRE_DYNAMIC_LINKING)
        ADD_DEFINITIONS(-DVIPREDICOM_LIBRARY)
    ELSE ()
        ADD_DEFINITIONS(-DVIPRE_LIBRARY_STATIC)
    ENDIF ()

    # Add DCMTK headers and libraries
    INCLUDE_DIRECTORIES (${DCMTK_ROOT_INCLUDE_DIR})
    SET (TARGET_EXTERNAL_LIBRARIES ${TARGET_EXTERNAL_LIBRARIES} ${DCMTK_LIBRARIES})
	IF (WIN32)
		SET (TARGET_EXTERNAL_LIBRARIES ${TARGET_EXTERNAL_LIBRARIES} wsock32.lib)
	ENDIF ()
	
    # Set up the lib name and header path
    SET (LIB_NAME vipreDICOM)
    SET (HEADER_PATH ${VIPRE_SOURCE_DIR}/include/${LIB_NAME})

    # Add all the headers
    SET (
        TARGET_H
        ${HEADER_PATH}/Export.hpp
		${HEADER_PATH}/Log.hpp
        ${HEADER_PATH}/MonoVoxelData.hpp
        ${HEADER_PATH}/Object.hpp
        ${HEADER_PATH}/RGBAVoxelData.hpp
        ${HEADER_PATH}/RGBVoxelData.hpp
        ${HEADER_PATH}/Series.hpp
        ${HEADER_PATH}/SeriesBuilder.hpp
        ${HEADER_PATH}/Slice.hpp
        ${HEADER_PATH}/Version.hpp
        ${HEADER_PATH}/vipreDICOM.hpp
        ${HEADER_PATH}/VoxelData.hpp
        ${HEADER_PATH}/VoxelDataTypeTemplate.hpp
    )

    # Add all the source files
    SET (
        TARGET_SRC
        Series.cpp
        SeriesBuilder.cpp
        Slice.cpp
        Version.cpp
    )

    # Add vipre as a target dependency
    SET (TARGET_LIBRARIES vipre)

    # Finish setting up the library
    SETUP_LIBRARY (${LIB_NAME})

ELSE ()

    MESSAGE ("ERROR: Not building vipreDICOM because DCMTK was not found!")

ENDIF ()
