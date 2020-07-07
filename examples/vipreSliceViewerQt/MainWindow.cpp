//
//  MainWindow.cpp
//  vipreSliceViewerQt
//
//  Created by Christian Noon on 12/5/11.
//  Copyright (c) 2011 Christian Noon. All rights reserved.
//

#include "MainWindow.hpp"
#include "QOSGWidget.hpp"

#include <vipre/Log.hpp>
#include <vipre/String.hpp>

MainWindowController::MainWindowController(osg::ref_ptr<osg::Switch> sliceSwitch) :
    _sliceSwitch(sliceSwitch),
    _visibleSliceIndex(0)
{
	_mainWindow = QSharedPointer<MainWindow>(new MainWindow(0));
	_mainWindow->connectSignalsWithController(this);
    _mainWindow->getOSGWidget()->setSceneData(_sliceSwitch.get());
}

MainWindowController::~MainWindowController()
{
	;
}

void MainWindowController::showWindow()
{
	_mainWindow->show();
}

void MainWindowController::onNextSliceAction()
{
    unsigned int numSlices = _sliceSwitch->getNumChildren();
    if (_visibleSliceIndex < numSlices-1)
    {
        ++_visibleSliceIndex;
        _sliceSwitch->setSingleChildOn(_visibleSliceIndex);
    }
}

void MainWindowController::onPreviousSliceAction()
{
    if (_visibleSliceIndex > 0)
    {
        --_visibleSliceIndex;
        _sliceSwitch->setSingleChildOn(_visibleSliceIndex);
    }
}

void MainWindowController::onExitAction()
{
	QApplication::instance()->exit();
}

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
    // Setup all the interface elements
	setupUi(this);
	setWindowTitle("vipreSliceViewerQt");

    // Create the QOSGWidget
    _osgWidget = new QOSGWidget();
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	sizePolicy.setHeightForWidth(_osgWidget->sizePolicy().hasHeightForWidth());
	sizePolicy.setHeightForWidth(_osgWidget->sizePolicy().hasHeightForWidth());
	_osgWidget->setSizePolicy(sizePolicy);
	_osgWidget->setMinimumSize(QSize(400, 300));
    setCentralWidget(_osgWidget);
}

MainWindow::~MainWindow()
{
	;
}

void MainWindow::connectSignalsWithController(QObject* controller)
{
	connect(nextSliceAction,	 SIGNAL(triggered()), controller, SLOT(onNextSliceAction()));
	connect(previousSliceAction, SIGNAL(triggered()), controller, SLOT(onPreviousSliceAction()));
	connect(exitAction, 		 SIGNAL(triggered()), controller, SLOT(onExitAction()));
}
