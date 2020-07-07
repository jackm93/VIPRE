//
//  TrackballManipulator.hpp
//  vipre
//
//  Created by Christian Noon on 12/20/11.
//  Copyright (c) 2011 Christian Noon. All rights reserved.
//

#ifndef VIPRE_TRACKBALL_MANIPULATOR_HPP
#define VIPRE_TRACKBALL_MANIPULATOR_HPP

#include <map>

#include <osgGA/OrbitManipulator>

#include <vipre/Export.hpp>

namespace vipre {

/**
 * The trackball manipulator is an osg orbit manipulator that adds the ability to set preset views through
 * the vipre::NotificationCenter. It will default to the "Inferior" view which is the view of the feet with
 * the chest pointing up.
 */
class VIPRE_EXPORT TrackballManipulator : public osgGA::OrbitManipulator
{
public:

    /** Helper struct for storing preset views for the trackball. */
    struct PresetView
    {
        String name;
        osg::Vec3d eyeVector;
        osg::Vec3d upVector;
        
        /** Default constructor. */
        PresetView()
        {
            ;
        }
        
        /** Constructor containing all member variables. */
        PresetView(vipre::String pvName, osg::Vec3d pvEyeVector, osg::Vec3d pvUpVector)
        {
            name = pvName;
            eyeVector = pvEyeVector;
            upVector = pvUpVector;
        }
    };
    
    /** Constructor. */
    TrackballManipulator();

    /** Copy constructor. */
    TrackballManipulator(const TrackballManipulator& rhs, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY);

    /** Implements all pure virtual methods from osg::Object. */
    META_Object(vipre, TrackballManipulator)

    /** Sets the camera for the trackball to manipulate and is required when using preset views. */
    inline void setCamera(osg::ref_ptr<osg::Camera> camera) { _camera = camera; }

    /** Adds a preset view for the trackball. */
    void addPresetView(PresetView presetView);
    
    /** Removes a preset view from the trackball. */
    void removePresetView(String name);
    
    /** Sets the trackball to the given preset view. */
    void setPresetView(String name);

protected:

    /** Destructor. */
    ~TrackballManipulator();

    /** Registers all the callback functions with the vipre::NotificationCenter. */
    void registerNotificationCenterCallbacks();

    /** Computes the home position of the trackball. */
    void computeHomePosition(const osg::Camera* camera, bool useBoundingBox);

    /** Sets the home position and the camera view matrix using the given preset view. */
    void setHomePositionWithPresetView(osg::Camera* camera,
                                       const PresetView& presetView,
                                       const osg::Vec3d& center,
                                       double distance);

    /** Computes the center of the trackball and distance (passed by reference) the camera should be away from the center. */
    void computeBestFitDistance(const osg::Camera* camera, bool useBoundingBox, osg::Vec3d& center, double& distance);

    /** Instance member variables. */
    osg::ref_ptr<osg::Camera> _camera;
    std::map<String, PresetView> _presetViews;
    PresetView _defaultView;
};

}   // End of vipre namespace

#endif  // End of VIPRE_TRACKBALL_MANIPULATOR_HPP
