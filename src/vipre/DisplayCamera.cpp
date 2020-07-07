//
//  DisplayCamera.cpp
//  vipre
//
//  Created by Christian Noon on 12/15/11.
//  Copyright (c) 2011 Christian Noon. All rights reserved.
//

#include <osg/Geode>
#include <osg/Geometry>

#include <vipre/DisplayCamera.hpp>

using namespace vipre;

DisplayCamera::DisplayCamera() : osg::Camera()
{
    ;
}

DisplayCamera::DisplayCamera(const DisplayCamera& rhs, const osg::CopyOp& copyop) :
    osg::Camera(rhs, copyop)
{
    ;
}

DisplayCamera::~DisplayCamera()
{
    ;
}

void DisplayCamera::initializeWithTexture(osg::ref_ptr<osg::Texture2D> texture)
{
	// Set the projection and view matrices
    setProjectionMatrix(osg::Matrix::ortho2D(0, 1000, 0, 1000));
    setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    setViewMatrix(osg::Matrix::identity());
    setRenderOrder(osg::Camera::POST_RENDER);

	// Don't clear any buffers with this camera by default
    setClearMask(0);

	// Create a quad to render the framebuffer texture onto
	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
	osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();
	geode->addDrawable(geometry.get());
    addChild(geode.get());

    // Set up the vertices and the quad
	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();
	vertices->push_back(osg::Vec3f(0.0f, 0.0f, 0.0f));
	vertices->push_back(osg::Vec3f(1000.0f, 0.0f, 0.0f));
	vertices->push_back(osg::Vec3f(1000.0f, 1000.0f, 0.0f));
	vertices->push_back(osg::Vec3f(0.0f, 1000.0f, 0.0f));
	geometry->setVertexArray(vertices.get());
	geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, vertices->size()));

    // Set up the texture coordinates
	osg::ref_ptr<osg::Vec2Array> texCoords = new osg::Vec2Array();
	texCoords->push_back(osg::Vec2f(0.0f, 0.0f));
	texCoords->push_back(osg::Vec2f(1.0f, 0.0f));
	texCoords->push_back(osg::Vec2f(1.0f, 1.0f));
	texCoords->push_back(osg::Vec2f(0.0f, 1.0f));
	geometry->setTexCoordArray(0, texCoords);

    // Attach the texture to the stateset
	osg::ref_ptr<osg::StateSet> stateset = getOrCreateStateSet();
	stateset->setTextureAttribute(0, texture.get());
}
