//
//  RenderCamera.hpp
//  vipre
//
//  Created by Christian Noon on 12/15/11.
//  Copyright (c) 2011 Christian Noon. All rights reserved.
//

#ifndef VIPRE_RENDER_CAMERA_HPP
#define VIPRE_RENDER_CAMERA_HPP

#include <osg/Camera>
#include <osg/ref_ptr>
#include <osg/Texture2D>

#include <vipre/Export.hpp>

namespace vipre {

/**
 * This is a pre-render camera that will contain the scene to be rendered. It is then rendered
 * the the given texture in the initializeWithTexture method. Once the texture is written, it
 * is then displayed using a display camera.
 */
class VIPRE_EXPORT RenderCamera : public osg::Camera
{
public:

    /** Default constructor. */
    RenderCamera();

    /** Copy constructor. */
    RenderCamera(const RenderCamera& rhs, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY);

    /** Implements all pure virtual methods from osg::Object. */
    META_Object(vipre, RenderCamera)

    /** Initializes the camera with the given texture. */
    void initializeWithTexture(osg::ref_ptr<osg::Texture2D> texture);

    /** Updates the viewport to match the new texture size. */
	void updateViewport(int width, int height);

protected:

    /** Destructor. */
    ~RenderCamera();
};

}   // End of vipre namespace

#endif  // End of VIPRE_RENDER_CAMERA_HPP
