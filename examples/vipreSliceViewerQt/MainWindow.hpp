
//  MainWindow.hpp
//  vipreSliceViewerQt
//
//  Created by Christian Noon on 12/5/11.
//  Copyright (c) 2011 Christian Noon. All rights reserved.
//

#include "ui_MainWindow.h"

#include <osgDB/ReadFile>
#include <osgGA/TrackballManipulator>
#include <osg/Switch>
#include <osgQt/GraphicsWindowQt>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <QtCore/QTimer>
#include <QtGui/QHBoxLayout>

// Forward declarations
class MainWindow;
class QOSGWidget;

/**
 * This class controls all interactions between the system and the view.
 *
 * Example of how to implement the model/view/controller design pattern using
 * Qt and OSG together.
 */
class MainWindowController : public QObject
{
public:

	Q_OBJECT

public slots:

	/** Callback from the next slice action. */
	void onNextSliceAction();

	/** Callback from the previous slice action. */
	void onPreviousSliceAction();

	/** Callback from the file->exit action. */
	void onExitAction();

public:

	/** Constructor. */
	MainWindowController(osg::ref_ptr<osg::Switch> sliceSwitch);

	/** Destructor. */
	~MainWindowController();

	/** Shows the main window. */
	void showWindow();

protected:

	/** Instance member variables. */
	QSharedPointer<MainWindow> _mainWindow;
    osg::ref_ptr<osg::Switch> _sliceSwitch;
    unsigned int _visibleSliceIndex;
};

/**
 * This class controls all of the user interface of the main window
 * from the Designer UI file as well as rendering the QOSGWidget.
 *
 * It uses the model/view/controller pattern by decoupling the
 * user events from the widget and executing them in the controller
 * class. This class also shows how to properly embed an osg scene
 * inside a Qt widget.
 */
class MainWindow : public QMainWindow, private Ui::MainWindow
{
public:

	/** Constructor. */
	MainWindow(QWidget* parent);

	/** Destructor. */
	~MainWindow();

	/** Connects up the signals with the controller. */
	void connectSignalsWithController(QObject* controller);

    /** Returns the QOSGWidget. */
    inline QOSGWidget* getOSGWidget() { return _osgWidget; }

protected:

    /** Instance member variables. */
    QOSGWidget* _osgWidget;
};
