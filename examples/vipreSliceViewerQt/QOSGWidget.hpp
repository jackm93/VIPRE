//
//  QOSGWidget.hpp
//  vipreSliceViewerQt
//
//  Created by Christian Noon on 12/6/11.
//  Copyright (c) 2011 Christian Noon. All rights reserved.
//

#ifndef Q_OSG_WIDGET_HPP
#define Q_OSG_WIDGET_HPP

#include <osgViewer/Viewer>

#include <QtCore/QTimer>
#include <QtGui/QWidget>

/**
 * A QWidget subclass that is capable of rendering an osg scene.
 *
 * The rendering is performed through the osgViewer::Viewer interface along with
 * the osgQt::GraphicsWindowQt API for controlling the graphics context. The way
 * it is currently configured is just using a bunch of default values for this
 * example.
 */
class QOSGWidget : public QWidget, public osgViewer::Viewer
{
public:

    /** Constructor. */
    QOSGWidget();

    /** Render callback. */
    void paintEvent(QPaintEvent* event);

protected:

    /** Destructor. */
    ~QOSGWidget();

    /** Instance member variables. */
    QTimer _timer;
};

#endif  // End of Q_OSG_WIDGET_HPP
