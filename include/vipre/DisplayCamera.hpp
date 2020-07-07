//
//  DisplayCamera.hpp
//  vipre
//
//  Created by Christian Noon on 12/15/11.
//  Copyright (c) 2011 Christian Noon. All rights reserved.
//

#ifndef VIPRE_DISPLAY_CAMERA_HPP
#define VIPRE_DISPLAY_CAMERA_HPP

#include <osg/Camera>
#include <osg/ref_ptr>
#include <osg/Texture2D>

#include <vipre/Export.hpp>

namespace vipre {

/**
 * The display camera is a post-render camera that renders the texture passed in
 * the initializeWithTexture method. This texture is updated by a render camera.
 * The display camera's job is to simply scale up the texture if necessary and
 * display it. The scaling all happens in hardware in fragment shaders using
 * bilinear interpolation.
 */
class VIPRE_EXPORT DisplayCamera : public osg::Camera
{
public:

    /** Default constructor. */
    DisplayCamera();

    /** Copy constructor. */
    DisplayCamera(const DisplayCamera& rhs, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY);

    /** Implements all pure virtual methods from osg::Object. */
    META_Object(vipre, DisplayCamera)

    /** Initializes the camera with the given texture. */
    void initializeWithTexture(osg::ref_ptr<osg::Texture2D> texture);

protected:

    /** Destructor. */
    ~DisplayCamera();
};

}   // End of vipre namespace

#endif  // End of VIPRE_DISPLAY_CAMERA_HPP
