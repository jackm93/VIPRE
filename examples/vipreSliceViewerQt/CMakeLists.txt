
# Add DCMTK headers
INCLUDE_DIRECTORIES (${DCMTK_ROOT_INCLUDE_DIR})

SET (TARGET_COMMON_LIBRARIES
    vipre
    vipreDICOM
)

# Add all the source files
SET (TARGET_SRC
    main.cpp
    MainWindow.cpp
    QOSGWidget.cpp
)

# Add all the headers
SET (
    TARGET_H
    MainWindow.hpp
    QOSGWidget.hpp
)

# Add all the headers including the Q_OBJECT macro needing to run moc
SET (
    TARGET_MOC
    MainWindow.hpp
)

# Add all the ui forms
SET (
    TARGET_UI
    MainWindow.ui
)

# Set up moc to run on the target headers
QT4_WRAP_CPP (vipreSliceViewerQt_HEADERS_MOC ${TARGET_MOC})

# Set up uic to run on the target forms
QT4_WRAP_UI (vipreSliceViewerQt_FORMS_HEADERS ${TARGET_UI})

# Add the ui headers to the target headers
# NOTE: this is how the SETUP_EXAMPLE macro needs them
SET (TARGET_H ${TARGET_H}
    ${vipreSliceViewerQt_HEADERS_MOC}
    ${vipreSliceViewerQt_FORMS_HEADERS}
)

# Add the Qt headers and Qt preprocessor directives
INCLUDE (${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

# How to enable Qt libraries to link against other than just QtCore and QtGui
# SET (QT_USE_QTOPENGL TRUE)
# SET (QT_USE_QTXML TRUE)

# Link against the Qt libraries and osgQt
SET (TARGET_EXTERNAL_LIBRARIES ${TARGET_EXTERNAL_LIBRARIES}
    ${QT_LIBRARIES}
    ${OSG_osgQt_LIBRARY}
)

# Must be added to pick up the moc and uic generated files
INCLUDE_DIRECTORIES (${CMAKE_CURRENT_BINARY_DIR})

SETUP_EXAMPLE (vipreSliceViewerQt)
