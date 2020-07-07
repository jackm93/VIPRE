//
//  TrackballManipulator.cpp
//  vipre
//
//  Created by Christian Noon on 12/20/11.
//  Copyright (c) 2011 Christian Noon. All rights reserved.
//

#include <boost/function.hpp>

#include <osg/ComputeBoundsVisitor>

#include <vipre/Log.hpp>
#include <vipre/NotificationCenter.hpp>
#include <vipre/TrackballManipulator.hpp>

using namespace vipre;

TrackballManipulator::TrackballManipulator() : osgGA::OrbitManipulator(),
    _camera(NULL)
{
    // Set some of the properties for the trackball
    setVerticalAxisFixed(false);
    setAllowThrow(true);

    // Set up the default orthogonal preset views
    PresetView inferior("Inferior", osg::Vec3d(0.0f, 0.0f, 1.0f), osg::Vec3d(0.0f, 1.0f, 0.0f));
    PresetView superior("Superior", osg::Vec3d(0.0f, 0.0f, -1.0f), osg::Vec3d(0.0f, 1.0f, 0.0f));
    PresetView left("Left", osg::Vec3d(1.0f, 0.0f, 0.0f), osg::Vec3d(0.0f, 1.0f, 0.0f));
    PresetView right("Right", osg::Vec3d(-1.0f, 0.0f, 0.0f), osg::Vec3d(0.0f, 1.0f, 0.0f));
    PresetView anterior("Anterior", osg::Vec3d(0.0f, 1.0f, 0.0f), osg::Vec3d(0.0f, 0.0f, -1.0f));
    PresetView posterior("Posterior", osg::Vec3d(0.0f, -1.0f, 0.0f), osg::Vec3d(0.0f, 0.0f, 1.0f));
    addPresetView(inferior);
    addPresetView(superior);
    addPresetView(left);
    addPresetView(right);
    addPresetView(anterior);
    addPresetView(posterior);

    // Set up the default isometric preset views
    PresetView anterior_left_inferior("Anterior_Left_Inferior", osg::Vec3d(1.0f, 2.0f / 3.0f, 1.0f), osg::Vec3d(-1.0f, 1.0f, -1.0f));
    PresetView anterior_right_inferior("Anterior_Right_Inferior", osg::Vec3d(-1.0f, 2.0f / 3.0f, 1.0f), osg::Vec3d(-1.0f, 1.0f, 1.0f));
    PresetView anterior_left_superior("Anterior_Left_Superior", osg::Vec3d(1.0f, 2.0f / 3.0f, -1.0f), osg::Vec3d(1.0f, 1.0f, -1.0f));
    PresetView anterior_right_superior("Anterior_Right_Superior", osg::Vec3d(-1.0f, 2.0f / 3.0f, -1.0f), osg::Vec3d(1.0f, 1.0f, 1.0f));
    PresetView posterior_left_inferior("Posterior_Left_Inferior", osg::Vec3d(1.0f, -2.0f / 3.0f, 1.0f), osg::Vec3d(-1.0f, 1.0f, -1.0f));
    PresetView posterior_right_inferior("Posterior_Right_Inferior", osg::Vec3d(-1.0f, -2.0f / 3.0f, 1.0f), osg::Vec3d(-1.0f, 1.0f, 1.0f));
    PresetView posterior_left_superior("Posterior_Left_Superior", osg::Vec3d(1.0f, -2.0f / 3.0f, -1.0f), osg::Vec3d(1.0f, 1.0f, -1.0f));
    PresetView posterior_right_superior("Posterior_Right_Superior", osg::Vec3d(-1.0f, -2.0f / 3.0f, -1.0f), osg::Vec3d(1.0f, 1.0f, 1.0f));
    addPresetView(anterior_left_inferior);
    addPresetView(anterior_right_inferior);
    addPresetView(anterior_left_superior);
    addPresetView(anterior_right_superior);
    addPresetView(posterior_left_inferior);
    addPresetView(posterior_right_inferior);
    addPresetView(posterior_left_superior);
    addPresetView(posterior_right_superior);

    // Register all the notification center callbacks
    registerNotificationCenterCallbacks();

    // Set the default view for when the home position is initially computed
    _defaultView = inferior;
}

TrackballManipulator::TrackballManipulator(const TrackballManipulator& rhs, const osg::CopyOp& copyop) :
    osgGA::OrbitManipulator(rhs, copyop)
{
    _camera = rhs._camera;
    _presetViews = rhs._presetViews;
    _defaultView = rhs._defaultView;
}

TrackballManipulator::~TrackballManipulator()
{
    vipre::NotificationCenter::instance()->removeObserver(this);
}

void TrackballManipulator::addPresetView(PresetView presetView)
{
    _presetViews[presetView.name] = presetView;
}

void TrackballManipulator::removePresetView(String name)
{
    if (_presetViews.find(name) == _presetViews.end())
    {
        vipreLogWARNING(viprePrefix) << "Failed to remove the preset view because it does not exist: " << name << std::endl;
    }
    else
    {
        _presetViews.erase(name);
    }
}

void TrackballManipulator::setPresetView(String name)
{
    if (_presetViews.find(name) == _presetViews.end())
    {
        vipreLogWARNING(viprePrefix) << "Failed to set the preset view because it does not exist: " << name << std::endl;
    }
    else if (!_camera.valid())
    {
        vipreLogWARNING(viprePrefix) << "Failed to set the preset view because the camera needs to be set to the trackball "
            "using the vipre::TrackballManipulator::setCamera method" << std::endl;
    }
    else if (!getNode())
    {
        vipreLogWARNING(viprePrefix) << "Failed to set the preset view because the trackball doesn't have a node attached" << std::endl;
    }
    else
    {
        vipreLogDEBUG(viprePrefix) << "Setting preset view: " << name << std::endl;

        PresetView view_to_set = _presetViews.find(name)->second;
        double distance;
        osg::Vec3d center;
        computeBestFitDistance(_camera.get(), true, center, distance);

        // Adjust the distance if not an orthogonal view
        osg::Vec3d eye_vector = view_to_set.eyeVector;
        double eye_vec_dist = 1.0f;
        if (eye_vector[0] != 0.0f)
        {
            eye_vec_dist *= fabs(eye_vector[0]);
        }
        if (eye_vector[1] != 0.0f)
        {
            eye_vec_dist *= fabs(eye_vector[1]);
        }
        if (eye_vector[2] != 0.0f)
        {
            eye_vec_dist *= fabs(eye_vector[2]);
        }
        distance *= eye_vec_dist;

        setHomePositionWithPresetView(_camera.get(), view_to_set, center, distance);
    }
}

void TrackballManipulator::registerNotificationCenterCallbacks()
{
    boost::function<void (TrackballManipulator*, PresetView)> addPresetViewCallback(&TrackballManipulator::addPresetView);
    boost::function<void (TrackballManipulator*, String)> removePresetViewCallback(&TrackballManipulator::removePresetView);
    boost::function<void (TrackballManipulator*, String)> setPresetViewCallback(&TrackballManipulator::setPresetView);
    NotificationCenter::instance()->addObserver(this, addPresetViewCallback, "AddTrackballManipulatorPresetView");
    NotificationCenter::instance()->addObserver(this, removePresetViewCallback, "RemoveTrackballManipulatorPresetView");
    NotificationCenter::instance()->addObserver(this, setPresetViewCallback, "SetTrackballManipulatorPresetView");
}

void TrackballManipulator::computeHomePosition(const osg::Camera* camera, bool useBoundingBox)
{
    // Can only compute a home position if there is geometry attached to the trackball
    if (getNode())
    {
        double distance;
        osg::Vec3d center;
        computeBestFitDistance(camera, useBoundingBox, center, distance);
        setHomePositionWithPresetView(const_cast<osg::Camera*>(camera), _defaultView, center, distance);
    }
}

void TrackballManipulator::setHomePositionWithPresetView(osg::Camera* camera,
                                                         const PresetView& presetView,
                                                         const osg::Vec3d& center,
                                                         double distance)
{
    osg::Vec3d eye_pos = presetView.eyeVector * distance;
    osg::Vec3d up_vec = presetView.upVector;
    setHomePosition(center + eye_pos, center, up_vec, _autoComputeHomePosition);
    setTransformation(_homeEye, _homeCenter, _homeUp);
    if (camera != NULL)
    {
        camera->setViewMatrix(getInverseMatrix());
    }
}

void TrackballManipulator::computeBestFitDistance(const osg::Camera* camera, bool useBoundingBox, osg::Vec3d& center, double& distance)
{
    osg::BoundingSphere boundingSphere;
    if (useBoundingBox)
    {
        // Compute bounding box (bounding box computes model center more precisely than bounding sphere
        osg::ComputeBoundsVisitor cbVisitor;
        getNode()->accept(cbVisitor);
        osg::BoundingBox &bb = cbVisitor.getBoundingBox();

        if (bb.valid())
        {
            boundingSphere.expandBy(bb);
        }
        else
        {
            boundingSphere = getNode()->getBound();
        }
    }
    else
    {
        // Compute bounding sphere
        boundingSphere = getNode()->getBound();
    }

    // Set distance to default and set the center of the bounding sphere
    distance = 3.5f * boundingSphere.radius();
    center = boundingSphere.center();

    // If there's a camera, try to compute the dist automatically using the projection matrix
    if (camera)
    {
        // Try to compute dist from frustrum
        double left, right, bottom, top, zNear, zFar;
        if (camera->getProjectionMatrixAsFrustum(left, right, bottom, top, zNear, zFar))
        {
            double vertical2 = fabs(right - left) / zNear / 2.0f;
            double horizontal2 = fabs(top - bottom) / zNear / 2.0f;
            double dim = horizontal2 < vertical2 ? horizontal2 : vertical2;
            double viewAngle = atan2(dim, 1.0f);
            distance = boundingSphere.radius() / sin(viewAngle);
        }
        else
        {
            // Try to compute dist from ortho
            if (camera->getProjectionMatrixAsOrtho(left, right, bottom, top, zNear, zFar))
            {
                distance = fabs(zFar - zNear) / 2.0f;
            }
        }
    }
}
