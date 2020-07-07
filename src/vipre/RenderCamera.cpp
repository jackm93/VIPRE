//
//  RenderCamera.cpp
//  vipre
//
//  Created by Christian Noon on 12/15/11.
//  Copyright (c) 2011 Christian Noon. All rights reserved.
//

#include <vipre/RenderCamera.hpp>

using namespace vipre;

RenderCamera::RenderCamera() : osg::Camera()
{
    ;
}

RenderCamera::RenderCamera(const RenderCamera& rhs, const osg::CopyOp& copyop) :
    osg::Camera(rhs, copyop)
{
    ;
}

RenderCamera::~RenderCamera()
{
    ;
}

void RenderCamera::initializeWithTexture(osg::ref_ptr<osg::Texture2D> texture)
{
    // Set to be a prerender camera
	setRenderOrder(osg::Camera::PRE_RENDER);

	// Set a custom viewport
	int width = texture->getTextureWidth();
	int height = texture->getTextureHeight();
	setViewport(0, 0, width, height);

	// Don't clear any of the buffers each time
	setClearMask(0);

	// Attach the color buffer render to the texture
	attach(osg::Camera::COLOR_BUFFER, texture.get());
}

void RenderCamera::updateViewport(int width, int height)
{
    setViewport(0, 0, width, height);
}
