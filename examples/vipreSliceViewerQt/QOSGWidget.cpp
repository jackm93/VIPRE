//
//  QOSGWidget.cpp
//  vipreSliceViewerQt
//
//  Created by Christian Noon on 12/6/11.
//  Copyright (c) 2011 Christian Noon. All rights reserved.
//

#include "QOSGWidget.hpp"

#include <osgGA/TrackballManipulator>
#include <osgQt/GraphicsWindowQt>
#include <osgViewer/ViewerEventHandlers>

#include <QtGui/QHBoxLayout>

QOSGWidget::QOSGWidget()
{
    // Set up the graphics context traits
    osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();
    osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits();
    traits->windowName = "";
    traits->windowDecoration = false;
    traits->x = 0;
    traits->y = 0;
    traits->width = 800;
    traits->height = 600;
    traits->doubleBuffer = true;
    traits->alpha = ds->getMinimumNumAlphaBits();
    traits->stencil = ds->getMinimumNumStencilBits();
    traits->sampleBuffers = ds->getMultiSamples();
    traits->samples = ds->getNumMultiSamples();

    // Set up the camera
    osgQt::GraphicsWindowQt* gw = new osgQt::GraphicsWindowQt(traits.get());
    getCamera()->setGraphicsContext(gw);
    getCamera()->setViewport(new osg::Viewport(0, 0, traits->width, traits->height));

    // Add the GL widget to this osg widget
    QHBoxLayout* layout = new QHBoxLayout();
    layout->addWidget(gw->getGLWidget());
    layout->setSpacing(0);
    setLayout(layout);

    // Set up the osgViewer::Viewer
    setThreadingModel(osgViewer::Viewer::SingleThreaded);
    addEventHandler(new osgViewer::StatsHandler);
    setCameraManipulator(new osgGA::TrackballManipulator);

    // Connect the timer to the update call which will indirectly call the paintEvent() method
    connect(&_timer, SIGNAL(timeout()), this, SLOT(update()));
    _timer.start(10);
}

QOSGWidget::~QOSGWidget()
{
    ;
}

void QOSGWidget::paintEvent(QPaintEvent* event)
{
    frame();
}
