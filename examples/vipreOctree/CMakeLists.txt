
# Add DCMTK headers
INCLUDE_DIRECTORIES (${DCMTK_ROOT_INCLUDE_DIR})

SET (TARGET_COMMON_LIBRARIES
    vipre
    vipreDICOM
)
SET (TARGET_SRC main.cpp)
SETUP_EXAMPLE (vipreOctree)
