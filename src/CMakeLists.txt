
# Only compile if we found OSG
if (OSG_FOUND AND Boost_FOUND)

    # Add the Boost and OSG headers as well as force linking with OSG
    INCLUDE_DIRECTORIES (${OSG_INCLUDE_DIR} ${Boost_INCLUDE_DIR})
    SET (TARGET_EXTERNAL_LIBRARIES ${TARGET_EXTERNAL_LIBRARIES} ${OSG_LIBRARIES})

    # Add each of the libraries to the build
    FOREACH (VIPRE_LIB
            vipre
            vipreDICOM
        )

        MESSAGE("Configuring Library: " ${VIPRE_LIB})
        ADD_SUBDIRECTORY (${VIPRE_LIB})

    ENDFOREACH()

ENDIF ()
