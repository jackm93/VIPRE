//
//  BoundingBoxSwitch.cpp
//  vipre
//
//  Created by Christian Noon on 12/15/11.
//  Copyright (c) 2011 Christian Noon. All rights reserved.
//

#include <boost/function.hpp>

#include <osg/Geode>
#include <osg/LineWidth>

#include <vipre/BoundingBoxSwitch.hpp>
#include <vipre/NotificationCenter.hpp>

using namespace vipre;

BoundingBoxSwitch::BoundingBoxSwitch() :
    osg::Switch(),
    _renderOrder(PRE_RENDER_ORDER),
    _mutex(),
    _xNegGeometry(NULL),
    _xPosGeometry(NULL),
    _yNegGeometry(NULL),
    _yPosGeometry(NULL),
    _zNegGeometry(NULL),
    _zPosGeometry(NULL),
    _borderPoints(),
    _normals(),
    _cameraPosition()
{
	osg::ref_ptr<osg::StateSet> stateset = getOrCreateStateSet();
    stateset->setRenderBinDetails(1, "PreRenderBin");
    registerNotificationCenterCallbacks();
}

BoundingBoxSwitch::BoundingBoxSwitch(RenderOrder renderOrder) :
    osg::Switch(),
    _renderOrder(renderOrder),
    _mutex(),
    _xNegGeometry(NULL),
    _xPosGeometry(NULL),
    _yNegGeometry(NULL),
    _yPosGeometry(NULL),
    _zNegGeometry(NULL),
    _zPosGeometry(NULL),
    _borderPoints(),
    _normals(),
    _cameraPosition()
{
    osg::ref_ptr<osg::StateSet> stateset = getOrCreateStateSet();
	if (_renderOrder == PRE_RENDER_ORDER)
    {
		stateset->setRenderBinDetails(1, "PreRenderBin");
    }
	else
    {
		stateset->setRenderBinDetails(3, "PostRenderBin");
    }

    registerNotificationCenterCallbacks();
}

BoundingBoxSwitch::BoundingBoxSwitch(const BoundingBoxSwitch& rhs, const osg::CopyOp& copyop) :
    osg::Switch(rhs, copyop)
{
    _renderOrder = rhs._renderOrder;
    _xNegGeometry = rhs._xNegGeometry;
    _xPosGeometry = rhs._xPosGeometry;
    _yNegGeometry = rhs._yNegGeometry;
    _yPosGeometry = rhs._yPosGeometry;
    _zNegGeometry = rhs._zNegGeometry;
    _zPosGeometry = rhs._zPosGeometry;
    _borderPoints = rhs._borderPoints;
    _normals = rhs._normals;
    _cameraPosition = rhs._cameraPosition;

    osg::ref_ptr<osg::StateSet> stateset = getOrCreateStateSet();
	if (_renderOrder == PRE_RENDER_ORDER)
    {
		stateset->setRenderBinDetails(1, "PreRenderBin");
    }
	else
    {
		stateset->setRenderBinDetails(3, "PostRenderBin");
    }

    registerNotificationCenterCallbacks();
}

BoundingBoxSwitch::~BoundingBoxSwitch()
{
    NotificationCenter::instance()->removeObserver(this);
}

void BoundingBoxSwitch::traverse(osg::NodeVisitor& nodeVisitor)
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
    osg::Switch::traverse(nodeVisitor);
}

void BoundingBoxSwitch::setRenderOrder(const RenderOrder& renderOrder)
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
    _renderOrder = renderOrder;
	osg::ref_ptr<osg::StateSet> stateset = getOrCreateStateSet();
	if (_renderOrder == PRE_RENDER_ORDER)
    {
		stateset->setRenderBinDetails(1, "PreRenderBin");
    }
	else
    {
		stateset->setRenderBinDetails(3, "PostRenderBin");
    }
}

void BoundingBoxSwitch::setBoundingBox(const osg::BoundingBox& boundingBox)
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);

    // First make sure everything is reset properly
    reset();

 	// Set the initial color of the bounding box
    osg::Vec4f initialColor(0.1f, 0.1f, 0.1f, 1.0f);

	// X Negative
	{
		// Create the vertices
		osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
		vertices->push_back(osg::Vec3f(boundingBox.xMin(), boundingBox.yMin(), boundingBox.zMin()));
		vertices->push_back(osg::Vec3f(boundingBox.xMin(), boundingBox.yMin(), boundingBox.zMax()));
		vertices->push_back(osg::Vec3f(boundingBox.xMin(), boundingBox.yMax(), boundingBox.zMax()));
		vertices->push_back(osg::Vec3f(boundingBox.xMin(), boundingBox.yMax(), boundingBox.zMin()));

		// Set up the color
		osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
		colors->push_back(initialColor);

		// Set up the geometry
		_xNegGeometry = new osg::Geometry;
		_xNegGeometry->setVertexArray(vertices.get());
		_xNegGeometry->setColorArray(colors.get());
		_xNegGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
		_xNegGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, 0, vertices->size()));

		// Set up the geode
		osg::ref_ptr<osg::Geode> geode = new osg::Geode;
		geode->addDrawable(_xNegGeometry.get());
		addChild(geode.get());

		// Set up the border points and normal for the geode
		_borderPoints.push_back(vertices->at(0));
		_normals.push_back(osg::Vec3f(-1.0f, 0.0f, 0.0f));
	}

	// X Positive
	{
		// Create the vertices
		osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
		vertices->push_back(osg::Vec3f(boundingBox.xMax(), boundingBox.yMin(), boundingBox.zMax()));
		vertices->push_back(osg::Vec3f(boundingBox.xMax(), boundingBox.yMin(), boundingBox.zMin()));
		vertices->push_back(osg::Vec3f(boundingBox.xMax(), boundingBox.yMax(), boundingBox.zMin()));
		vertices->push_back(osg::Vec3f(boundingBox.xMax(), boundingBox.yMax(), boundingBox.zMax()));

		// Set up the color
		osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
		colors->push_back(initialColor);

		// Set up the geometry
		_xPosGeometry = new osg::Geometry;
		_xPosGeometry->setVertexArray(vertices.get());
		_xPosGeometry->setColorArray(colors.get());
		_xPosGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
		_xPosGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, 0, vertices->size()));

		// Set up the geode
		osg::ref_ptr<osg::Geode> geode = new osg::Geode;
		geode->addDrawable(_xPosGeometry.get());
		addChild(geode.get());

		// Set up the border points and normal for the geode
		_borderPoints.push_back(vertices->at(0));
		_normals.push_back(osg::Vec3f(1.0f, 0.0f, 0.0f));
	}

	// Y Negative
	{
		// Create the vertices
		osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
		vertices->push_back(osg::Vec3f(boundingBox.xMin(), boundingBox.yMin(), boundingBox.zMin()));
		vertices->push_back(osg::Vec3f(boundingBox.xMax(), boundingBox.yMin(), boundingBox.zMin()));
		vertices->push_back(osg::Vec3f(boundingBox.xMax(), boundingBox.yMin(), boundingBox.zMax()));
		vertices->push_back(osg::Vec3f(boundingBox.xMin(), boundingBox.yMin(), boundingBox.zMax()));

		// Set up the color
		osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
		colors->push_back(initialColor);

		// Set up the geometry
		_yNegGeometry = new osg::Geometry;
		_yNegGeometry->setVertexArray(vertices.get());
		_yNegGeometry->setColorArray(colors.get());
		_yNegGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
		_yNegGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, 0, vertices->size()));

		// Set up the geode
		osg::ref_ptr<osg::Geode> geode = new osg::Geode;
		geode->addDrawable(_yNegGeometry.get());
		addChild(geode.get());

		// Set up the border points and normal for the geode
		_borderPoints.push_back(vertices->at(0));
		_normals.push_back(osg::Vec3f(0.0f, -1.0f, 0.0f));
	}

	// Y Positive
	{
		// Create the vertices
		osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
		vertices->push_back(osg::Vec3f(boundingBox.xMin(), boundingBox.yMax(), boundingBox.zMin()));
		vertices->push_back(osg::Vec3f(boundingBox.xMax(), boundingBox.yMax(), boundingBox.zMin()));
		vertices->push_back(osg::Vec3f(boundingBox.xMax(), boundingBox.yMax(), boundingBox.zMax()));
		vertices->push_back(osg::Vec3f(boundingBox.xMin(), boundingBox.yMax(), boundingBox.zMax()));

		// Set up the color
		osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
		colors->push_back(initialColor);

		// Set up the geometry
		_yPosGeometry = new osg::Geometry;
		_yPosGeometry->setVertexArray(vertices.get());
		_yPosGeometry->setColorArray(colors.get());
		_yPosGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
		_yPosGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, 0, vertices->size()));

		// Set up the geode
		osg::ref_ptr<osg::Geode> geode = new osg::Geode;
		geode->addDrawable(_yPosGeometry.get());
		addChild(geode.get());

		// Set up the border points and normal for the geode
		_borderPoints.push_back(vertices->at(0));
		_normals.push_back(osg::Vec3f(0.0f, 1.0f, 0.0f));
	}

	// Z Negative
	{
		// Create the vertices
		osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
		vertices->push_back(osg::Vec3f(boundingBox.xMin(), boundingBox.yMin(), boundingBox.zMin()));
		vertices->push_back(osg::Vec3f(boundingBox.xMax(), boundingBox.yMin(), boundingBox.zMin()));
		vertices->push_back(osg::Vec3f(boundingBox.xMax(), boundingBox.yMax(), boundingBox.zMin()));
		vertices->push_back(osg::Vec3f(boundingBox.xMin(), boundingBox.yMax(), boundingBox.zMin()));

		// Set up the color
		osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
		colors->push_back(initialColor);

		// Set up the geometry
		_zNegGeometry = new osg::Geometry;
		_zNegGeometry->setVertexArray(vertices.get());
		_zNegGeometry->setColorArray(colors.get());
		_zNegGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
		_zNegGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, 0, vertices->size()));

		// Set up the geode
		osg::ref_ptr<osg::Geode> geode = new osg::Geode;
		geode->addDrawable(_zNegGeometry.get());
		addChild(geode.get());

		// Set up the border points and normal for the geode
		_borderPoints.push_back(vertices->at(0));
		_normals.push_back(osg::Vec3f(0.0f, 0.0f, -1.0f));
	}

	// Z Positive
	{
		// Create the vertices
		osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
		vertices->push_back(osg::Vec3f(boundingBox.xMin(), boundingBox.yMin(), boundingBox.zMax()));
		vertices->push_back(osg::Vec3f(boundingBox.xMax(), boundingBox.yMin(), boundingBox.zMax()));
		vertices->push_back(osg::Vec3f(boundingBox.xMax(), boundingBox.yMax(), boundingBox.zMax()));
		vertices->push_back(osg::Vec3f(boundingBox.xMin(), boundingBox.yMax(), boundingBox.zMax()));

		// Set up the color
		osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
		colors->push_back(initialColor);

		// Set up the geometry
		_zPosGeometry = new osg::Geometry;
		_zPosGeometry->setVertexArray(vertices.get());
		_zPosGeometry->setColorArray(colors.get());
		_zPosGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
		_zPosGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, 0, vertices->size()));

		// Set up the geode
		osg::ref_ptr<osg::Geode> geode = new osg::Geode;
		geode->addDrawable(_zPosGeometry.get());
		addChild(geode.get());

		// Set up the border points and normal for the geode
		_borderPoints.push_back(vertices->at(0));
		_normals.push_back(osg::Vec3f(0.0f, 0.0f, 1.0f));
	}

	// Set the default line width
	getOrCreateStateSet()->setAttribute(new osg::LineWidth(2.0f), osg::StateAttribute::ON);
}

void BoundingBoxSwitch::setLineWidth(float width)
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
    getOrCreateStateSet()->setAttribute(new osg::LineWidth(width), osg::StateAttribute::ON);
}

void BoundingBoxSwitch::setColor(const osg::Vec4f& color)
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	colors->push_back(color);
	_xNegGeometry->setColorArray(colors.get());
	_xPosGeometry->setColorArray(colors.get());
	_yNegGeometry->setColorArray(colors.get());
	_yPosGeometry->setColorArray(colors.get());
	_zNegGeometry->setColorArray(colors.get());
	_zPosGeometry->setColorArray(colors.get());
}

void BoundingBoxSwitch::setCameraPosition(osg::Vec3f cameraPosition)
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
	for (unsigned int i = 0; i < _normals.size(); ++i)
	{
        osg::Vec3f borderPoint = _borderPoints.at(i);
		osg::Vec3f normal = _normals.at(i);
		osg::Vec3f cameraDirection = borderPoint - cameraPosition;
		double dotProduct = cameraDirection * normal;
		if (_renderOrder == PRE_RENDER_ORDER)
        {
			dotProduct > 0.0f ? setValue(i, true) : setValue(i, false);
        }
		else
        {
			dotProduct > 0.0f ? setValue(i, false) : setValue(i, true);
        }
	}
}

void BoundingBoxSwitch::reset()
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
    removeChildren(0, getNumChildren());
	_xNegGeometry = NULL;
	_xPosGeometry = NULL;
	_yNegGeometry = NULL;
	_yPosGeometry = NULL;
	_zNegGeometry = NULL;
	_zPosGeometry = NULL;
	_borderPoints.clear();
	_normals.clear();
}

void BoundingBoxSwitch::registerNotificationCenterCallbacks()
{
    boost::function<void (BoundingBoxSwitch*, osg::BoundingBox)> setBoundingBoxCallback(&BoundingBoxSwitch::setBoundingBox);
    boost::function<void (BoundingBoxSwitch*, float)> setBoundingBoxLineWidthCallback(&BoundingBoxSwitch::setLineWidth);
    boost::function<void (BoundingBoxSwitch*, osg::Vec4f)> setBoundingBoxColorCallback(&BoundingBoxSwitch::setColor);
    boost::function<void (BoundingBoxSwitch*, osg::Vec3f)> setCameraPositionCallback(&BoundingBoxSwitch::setCameraPosition);
    boost::function<void (BoundingBoxSwitch*)> resetBoundingBoxCallback(&BoundingBoxSwitch::reset);
    NotificationCenter::instance()->addObserver(this, setBoundingBoxCallback, "SetBoundingBox");
    NotificationCenter::instance()->addObserver(this, setBoundingBoxLineWidthCallback, "SetBoundingBoxLineWidth");
    NotificationCenter::instance()->addObserver(this, setBoundingBoxColorCallback, "SetBoundingBoxColor");
    NotificationCenter::instance()->addObserver(this, setCameraPositionCallback, "SetCameraPosition");
    NotificationCenter::instance()->addObserver(this, resetBoundingBoxCallback, "ResetBoundingBox");
}
