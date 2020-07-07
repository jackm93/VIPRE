//
//  BackgroundCamera.hpp
//  vipre
//
//  Created by Christian Noon on 12/15/11.
//  Copyright (c) 2011 Christian Noon. All rights reserved.
//

#ifndef VIPRE_BACKGROUND_CAMERA_HPP
#define VIPRE_BACKGROUND_CAMERA_HPP

#include <osg/Camera>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Array>
#include <osg/ref_ptr>

#include <vipre/Export.hpp>
#include <vipre/String.hpp>

namespace vipre {

/** A helper struct for storing solid background information. */
struct VIPRE_EXPORT SolidBackground
{
    String name;
    osg::Vec4f color;

    /** Default constructor. */
    SolidBackground() :
        name(),
        color()
    {
        ;
    }

    /** Constructor with member variables. */
    SolidBackground(const String& myName, const osg::Vec4f& myColor) :
        name(myName),
        color(myColor)
    {
        ;
    }
};

/** A helper struct for storing gradient background information. */
struct VIPRE_EXPORT GradientBackground
{
    String name;
    osg::Vec4f topColor;
    osg::Vec4f bottomColor;

    /** Default constructor. */
    GradientBackground() :
        name(),
        topColor(),
        bottomColor()
    {
        ;
    }

    /** Constructor with member variables. */
    GradientBackground(const String& myName, const osg::Vec4f& myTopColor, const osg::Vec4f& myBottomColor) :
        name(myName),
        topColor(myTopColor),
        bottomColor(myBottomColor)
    {
        ;
    }
};

/**
 * This class can be used to render a solid or gradient background in the scene. This gives you
 * flexibility than just relying on the clear color of the main camera. The setter methods for
 * this convenience class are accessible through the NotificationCenter for quick easy access.
 */
class VIPRE_EXPORT BackgroundCamera : public osg::Camera
{
public:

    /** Constructor. */
    BackgroundCamera();

    /** Copy constructor. */
    BackgroundCamera(const BackgroundCamera& rhs, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY);

    /** Implements all pure virtual methods from osg::Object. */
    META_Object(vipre, BackgroundCamera)

    /** Adds the new solid background to the list of available solid backgrounds. */
    void addSolidBackground(const SolidBackground& background);

    /** Adds the new gradient background to the list of available gradient backgrounds. */
    void addGradientBackground(const GradientBackground& background);

    /** Removes the matching solid background from the list of available solid backgrounds. */
    void removeSolidBackground(const String& backgroundName);

    /** Removes the matching gradient background from the list of available gradient backgrounds. */
    void removeGradientBackground(const String& backgroundName);

    /** Switches the current background colors to the matching solid background. */
    void switchToSolidBackground(const String& backgroundName);

    /** Switches the current background colors to the matching gradient background. */
    void switchToGradientBackground(const String& backgroundName);

    /** Returns the current list of solid backgrounds. */
    inline const std::map<String, SolidBackground>& getSolidBackgrounds() { return _solidBackgrounds; }

    /** Returns the current list of gradient backgrounds. */
    inline const std::map<String, GradientBackground>& getGradientBackgrounds() { return _gradientBackgrounds; }

protected:

    /** Destructor. */
    ~BackgroundCamera();

    /** Initializes a set of default solid backgrounds. */
    void initializeSolidBackgroundDefaults();

    /** Initializes a set of default gradient backgrounds. */
    void initializeGradientBackgroundDefaults();

    /** Initializes the camera settings. */
    void initializeCamera();

    /** Initializes the background quad. */
    void initializeBackgroundGeometry();

    /** Handles modifying the background colors. */
	void setBackgroundColor(const osg::Vec4f& topColor, const osg::Vec4f& bottomColor);

    /** Instance member variables. */
    osg::ref_ptr<osg::Geode>             _backgroundGeode;
	osg::ref_ptr<osg::Geometry>          _backgroundGeometry;
    osg::ref_ptr<osg::Vec4Array>         _backgroundColors;
	std::map<String, SolidBackground>    _solidBackgrounds;
	std::map<String, GradientBackground> _gradientBackgrounds;
};

}   // End of vipre namespace

#endif  // End of VIPRE_BACKGROUND_CAMERA_HPP
