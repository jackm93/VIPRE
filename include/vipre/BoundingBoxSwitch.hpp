//
//  BoundingBoxSwitch.hpp
//  vipre
//
//  Created by Christian Noon on 12/15/11.
//  Copyright (c) 2011 Christian Noon. All rights reserved.
//

#ifndef VIPRE_BOUNDING_BOX_SWITCH_HPP
#define VIPRE_BOUNDING_BOX_SWITCH_HPP

#include <osg/BoundingBox>
#include <osg/Geometry>
#include <osg/Switch>

#include <vipre/Enums.hpp>
#include <vipre/Export.hpp>

namespace vipre {

/**
 * The bounding box switch is a thread-safe osg::Switch that handles rendering a depth-sorted
 * bounding box. All access and traversal to the class is thread-safe to ensure easy access
 * when using a multi-threaded renderer. In order to use this class effectively, you will want
 * to create both a PRE and POST_RENDER_ORDER bounding box switch. Attach the PRE_RENDER_ORDER
 * switch to the scenegraph before rendering the geometry inside the bounding box. Then attach
 * the POST_RENDER_ORDER switch to the scenegraph after the rendered geometry. This will then
 * ensure a properly rendered bounding box even when the geometry is being rendered with
 * alpha blending. This is accomplished by using a mixture of render bins.
 *
 * Also, most all of the API is accessible through the vipre::NotificationCenter.
 */
class VIPRE_EXPORT BoundingBoxSwitch : public osg::Switch
{
public:

    /** Default constructor. */
    BoundingBoxSwitch();

    /** Constructor with render order. */
    BoundingBoxSwitch(RenderOrder renderOrder);

    /** Copy constructor. */
    BoundingBoxSwitch(const BoundingBoxSwitch& rhs, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY);

    /** Implements all pure virtual methods from osg::Object. */
    META_Object(vipre, BoundingBoxSwitch)

    /** Traverses the bounding box switch and all its children during rendering. */
    void traverse(osg::NodeVisitor& nodeVisitor);

    /** Sets the render order of the bounding box switch. */
    void setRenderOrder(const RenderOrder& renderOrder);

    /** Sets the bounding box for the the switch. */
	void setBoundingBox(const osg::BoundingBox& boundingBox);

	/** Updates the line width of the bounding box planes. */
	void setLineWidth(float width);

	/** Updates the color of the bounding box planes. */
	void setColor(const osg::Vec4f& color);

	/** Updates which planes are visible based on the dot product plane normals and the camera direction. */
    void setCameraPosition(osg::Vec3f cameraPosition);

	/** Resets the bounding box switch children and stored geometry information. */
    void reset();

protected:

    /** Destructor. */
    ~BoundingBoxSwitch();

    /** Registers all the callback functions with the vipre::NotificationCenter. */
    void registerNotificationCenterCallbacks();

    /** Instance member variables. */
	RenderOrder                 _renderOrder;
    OpenThreads::Mutex          _mutex;
    osg::ref_ptr<osg::Geometry>	_xNegGeometry;
	osg::ref_ptr<osg::Geometry>	_xPosGeometry;
	osg::ref_ptr<osg::Geometry>	_yNegGeometry;
	osg::ref_ptr<osg::Geometry>	_yPosGeometry;
	osg::ref_ptr<osg::Geometry>	_zNegGeometry;
	osg::ref_ptr<osg::Geometry>	_zPosGeometry;
	std::vector<osg::Vec3f>     _borderPoints;
	std::vector<osg::Vec3f>     _normals;
    osg::Vec3f                  _cameraPosition;
};

}   // End of vipre namespace

#endif  // End of VIPRE_BOUNDING_BOX_SWITCH_HPP
