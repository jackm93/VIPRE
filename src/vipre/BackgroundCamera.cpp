//
//  BackgroundCamera.cpp
//  vipre
//
//  Created by Christian Noon on 12/15/11.
//  Copyright (c) 2011 Christian Noon. All rights reserved.
//

#include <boost/function.hpp>

#include <vipre/BackgroundCamera.hpp>
#include <vipre/Log.hpp>
#include <vipre/NotificationCenter.hpp>

using namespace vipre;

BackgroundCamera::BackgroundCamera() : osg::Camera()
{
    // Initialize the default background, camera and quad geometry
    initializeSolidBackgroundDefaults();
    initializeGradientBackgroundDefaults();
    initializeCamera();
    initializeBackgroundGeometry();

    // Register setter methods with the vipre::NotificationCenter
    boost::function<void (BackgroundCamera*, SolidBackground)> addSolidCallback(&BackgroundCamera::addSolidBackground);
    boost::function<void (BackgroundCamera*, GradientBackground)> addGradientCallback(&BackgroundCamera::addGradientBackground);
    boost::function<void (BackgroundCamera*, String)> removeSolidCallback(&BackgroundCamera::removeSolidBackground);
    boost::function<void (BackgroundCamera*, String)> removeGradientCallback(&BackgroundCamera::removeGradientBackground);
    boost::function<void (BackgroundCamera*, String)> switchToSolidCallback(&BackgroundCamera::switchToSolidBackground);
    boost::function<void (BackgroundCamera*, String)> switchToGradientCallback(&BackgroundCamera::switchToGradientBackground);
    NotificationCenter::instance()->addObserver(this, addSolidCallback, "AddSolidBackground");
    NotificationCenter::instance()->addObserver(this, addGradientCallback, "AddGradientBackground");
    NotificationCenter::instance()->addObserver(this, removeSolidCallback, "RemoveSolidBackground");
    NotificationCenter::instance()->addObserver(this, removeGradientCallback, "RemoveGradientBackground");
    NotificationCenter::instance()->addObserver(this, switchToSolidCallback, "SwitchToSolidBackground");
    NotificationCenter::instance()->addObserver(this, switchToGradientCallback, "SwitchToGradientBackground");
}

BackgroundCamera::BackgroundCamera(const BackgroundCamera& rhs, const osg::CopyOp& copyop) :
    osg::Camera(rhs, copyop)
{
    ;
}

BackgroundCamera::~BackgroundCamera()
{
    NotificationCenter::instance()->removeObserver(this);
}

void BackgroundCamera::initializeSolidBackgroundDefaults()
{
    // Set up some default solid backgrounds
    SolidBackground white("White", osg::Vec4f(1.0f, 1.0f, 1.0f, 1.0f));
    SolidBackground black("Black", osg::Vec4f(0.0f, 0.0f, 0.0f, 1.0f));
    SolidBackground lightGrey("Light Grey", osg::Vec4f(0.75f, 0.75f, 0.75f, 1.0f));
    SolidBackground grey("Grey", osg::Vec4f(0.5f, 0.5f, 0.5f, 1.0f));
    SolidBackground darkGrey("Dark Grey", osg::Vec4f(0.25f, 0.25f, 0.25f, 1.0f));
    _solidBackgrounds.insert(std::pair<String, SolidBackground>(white.name, white));
    _solidBackgrounds.insert(std::pair<String, SolidBackground>(black.name, black));
    _solidBackgrounds.insert(std::pair<String, SolidBackground>(lightGrey.name, lightGrey));
    _solidBackgrounds.insert(std::pair<String, SolidBackground>(grey.name, grey));
    _solidBackgrounds.insert(std::pair<String, SolidBackground>(darkGrey.name, darkGrey));
}

void BackgroundCamera::initializeGradientBackgroundDefaults()
{
    // Set up some default gradient backgrounds
    GradientBackground whiteGradient("White Gradient", osg::Vec4f(1.0f, 1.0f, 1.0f, 1.0f), osg::Vec4f(0.5f, 0.5f, 0.5f, 1.0f));
    GradientBackground blackGradient("Black Gradient", osg::Vec4f(0.1f, 0.1f, 0.1f, 1.0f), osg::Vec4f(0.5f, 0.5f, 0.5f, 1.0f));
    GradientBackground greyGradient("Grey Gradient", osg::Vec4f(0.25f, 0.25f, 0.25f, 1.0f), osg::Vec4f(0.75f, 0.75f, 0.75f, 1.0f));
    _gradientBackgrounds.insert(std::pair<String, GradientBackground>(whiteGradient.name, whiteGradient));
    _gradientBackgrounds.insert(std::pair<String, GradientBackground>(blackGradient.name, blackGradient));
    _gradientBackgrounds.insert(std::pair<String, GradientBackground>(greyGradient.name, greyGradient));
}

void BackgroundCamera::initializeCamera()
{
    // Set the projection and view matrices
	setProjectionMatrix(osg::Matrix::ortho2D(0, 1000, 0, 1000));
	setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	setViewMatrix(osg::Matrix::identity());

	// Draw subgraph before main camera view
	setRenderOrder(osg::Camera::PRE_RENDER);

	// We don't want the camera to grab event focus from the viewers main camera
	setAllowEventFocus(false);
}

void BackgroundCamera::initializeBackgroundGeometry()
{
    // Setup the geode and geometry nodes
	_backgroundGeode = new osg::Geode;
	_backgroundGeometry = new osg::Geometry();
	addChild(_backgroundGeode.get());
	_backgroundGeode->addDrawable(_backgroundGeometry.get());

    // Set up the vertices
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
    vertices->push_back(osg::Vec3f(0.0f, 0.0f, 0.0f));
    vertices->push_back(osg::Vec3f(1000.0f, 0.0f, 0.0f));
    vertices->push_back(osg::Vec3f(1000.0f, 1000.0f, 0.0f));
    vertices->push_back(osg::Vec3f(0.0f, 1000.0f, 0.0f));
    _backgroundGeometry->setVertexArray(vertices.get());

    // Set up the background colors
	_backgroundColors = new osg::Vec4Array;
    switchToGradientBackground("Black Gradient");

    // Add a quad primitive to the geometry
    _backgroundGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, vertices->size()));

	// Disable lighting geode
	_backgroundGeode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
}

void BackgroundCamera::addSolidBackground(const SolidBackground& background)
{
    // Warn the user if they are going to overwrite a solid background
    if (_solidBackgrounds.find(background.name) != _solidBackgrounds.end())
    {
        vipreLogWARNING(viprePrefix) << "The solid background: " << background.name
            << " already exists...will be overwritten with new one." << std::endl;
    }

    _solidBackgrounds.insert(std::pair<String, SolidBackground>(background.name, background));
}

void BackgroundCamera::addGradientBackground(const GradientBackground& background)
{
    // Warn the user if they are going to overwrite a gradient background
    if (_gradientBackgrounds.find(background.name) != _gradientBackgrounds.end())
    {
        vipreLogWARNING(viprePrefix) << "The gradient background: " << background.name
            << " already exists...will be overwritten with new one." << std::endl;
    }

    _gradientBackgrounds.insert(std::pair<String, GradientBackground>(background.name, background));
}

void BackgroundCamera::removeSolidBackground(const String& backgroundName)
{
    std::map<String, SolidBackground>::iterator iter = _solidBackgrounds.find(backgroundName);
    if (iter != _solidBackgrounds.end())
    {
        _solidBackgrounds.erase(iter);
    }
    else
    {
        vipreLogWARNING(viprePrefix) << "Could not remove solid background: " << backgroundName
            << " because it does not exist." << std::endl;
    }
}

void BackgroundCamera::removeGradientBackground(const String& backgroundName)
{
    std::map<String, GradientBackground>::iterator iter = _gradientBackgrounds.find(backgroundName);
    if (iter != _gradientBackgrounds.end())
    {
        _gradientBackgrounds.erase(iter);
    }
    else
    {
        vipreLogWARNING(viprePrefix) << "Could not remove gradient background: " << backgroundName
            << " because it does not exist." << std::endl;
    }
}

void BackgroundCamera::switchToSolidBackground(const String& backgroundName)
{
    std::map<String, SolidBackground>::iterator iter = _solidBackgrounds.find(backgroundName);
    if (iter != _solidBackgrounds.end())
    {
        SolidBackground background = iter->second;
        setBackgroundColor(background.color, background.color);
    }
    else
    {
        vipreLogWARNING(viprePrefix) << "Could not switch to solid background: " << backgroundName
            << " because it does not exist." << std::endl;
    }
}

void BackgroundCamera::switchToGradientBackground(const String& backgroundName)
{
    std::map<String, GradientBackground>::iterator iter = _gradientBackgrounds.find(backgroundName);
    if (iter != _gradientBackgrounds.end())
    {
        GradientBackground background = iter->second;
        setBackgroundColor(background.topColor, background.bottomColor);
    }
    else
    {
        vipreLogWARNING(viprePrefix) << "Could not switch to gradient background: " << backgroundName
        << " because it does not exist." << std::endl;
    }
}

void BackgroundCamera::setBackgroundColor(const osg::Vec4f& topColor, const osg::Vec4f& bottomColor)
{
    // Update the background colors array
    _backgroundColors->clear();
    _backgroundColors->push_back(bottomColor);
    _backgroundColors->push_back(bottomColor);
    _backgroundColors->push_back(topColor);
    _backgroundColors->push_back(topColor);

    // Set the color array for the background geometry
    _backgroundGeometry->setColorArray(_backgroundColors.get());
}
