
# Add the Boost and OSG headers for all examples
INCLUDE_DIRECTORIES (${Boost_INCLUDE_DIR} ${OSG_INCLUDE_DIR})

# Add each of the default dependency examples to the build
FOREACH (VIPRE_EXAMPLE
		vipreDefense
		vipreLighting
		vipreOctree
		viprePrerenderPass
        vipreNotificationCenter
        vipreSliceViewerOSG
        vipreDICOMSliceInfo
        vipreDICOMSeriesBuilder
        vipreDICOMSeriesInfo
    )

    MESSAGE("Configuring Example: " ${VIPRE_EXAMPLE})
    ADD_SUBDIRECTORY (${VIPRE_EXAMPLE})

ENDFOREACH()

# Add each of the Qt examples
if (OSG_osgQt_LIBRARY AND QT_FOUND)

    FOREACH (VIPRE_QT_EXAMPLE
            vipreSliceViewerQt
        )

        MESSAGE("Configuring Example: " ${VIPRE_QT_EXAMPLE})
        ADD_SUBDIRECTORY (${VIPRE_QT_EXAMPLE})

    ENDFOREACH()

ENDIF()
