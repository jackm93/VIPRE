 //
//  TexturedCuboid.cpp
//  vipre
//
//  Created by Christian Noon on 2/11/12.
//  Copyright (c) 2012 Christian Noon. All rights reserved.
//

#include <vipre/Log.hpp>
#include <vipre/TexturedCuboid.hpp>

using namespace vipre;

TexturedCuboid::TexturedCuboid() : osg::Geode(),
    _image(NULL),
    _voxelTexture(NULL),
    _octreeTexture(NULL),
    //_faces(NULL),
    _cuboidDimensions()
{
    // Build the opacity and color tables
    _opacityTables.generateAllTables();
    _colorTables.generateAllTables();

    // Set up all the uniforms
    _voxelDimensionsUniform = new osg::Uniform("voxelDimensions", osg::Vec3d(0.0, 0.0, 0.0));
    _cuboidDimensionsUniform = new osg::Uniform("cuboidDimensions", osg::Vec3d(0.0, 0.0, 0.0));
    _paddedDepthUniform = new osg::Uniform("paddedDepth", 0.0f);
	_paddedDepthUniform->setType(osg::Uniform::FLOAT);
	_inverseVoxelSpacingUniform = new osg::Uniform("inverseVoxelSpacing", osg::Vec3d(0.0, 0.0, 0.0));

	//_voxelDimensionsUniform = new osg::Uniform("voxelDimensions", osg::Vec3f(0.0f, 0.0f, 0.0f));
	//_cuboidDimensionsUniform = new osg::Uniform("cuboidDimensions", osg::Vec3f(0.0f, 0.0f, 0.0f));
	//_inverseVoxelSpacingUniform = new osg::Uniform("inverseVoxelSpacing", osg::Vec3f(0.0f, 0.0f, 0.0f));

    _viewToWorldMatrixUniform = new osg::Uniform("viewToWorldMatrix", osg::Matrixf()); //changed from Matrixd
	_cameraInsideUniform = new osg::Uniform("insideVolume", false);
	_lOffset = new osg::Uniform("lOffset", 0.f);
	_rOffset = new osg::Uniform("rOffset", 0.f);
	_lOffset->setType(osg::Uniform::FLOAT);
	_rOffset->setType(osg::Uniform::FLOAT);
	//_viewToWorldMatrixUniform->set;
	_pointerPos = new osg::Uniform("pointerpos", osg::Vec3(0.0, 0.0, 0.0));
	_pointerQuat = new osg::Uniform("pointerquat", osg::Vec3(0.0, 0.0, 0.0));
	_cuboidMatrix = new osg::Uniform("cuboidMatrix", osg::Matrixf());

	_projectionMatrixUniform = new osg::Uniform("projectionMatrix", osg::Matrixf());
	_isHMD = new osg::Uniform("hmd", false);
	_cuboidPosition = new osg::Uniform("cuboidPosition", osg::Vec3f(0.0, 0.0, 0.0));
	_clipPos = new osg::Uniform("clipPos", 0.0f);

	unType = _viewToWorldMatrixUniform->getType();
	printf("Uniform type: %i\n", unType);
	
	_sampleSizeUniform = new osg::Uniform("sampleSize", 1.0f);
    _minIntensityUniform = new osg::Uniform("minIntensity", 0.38f);
	_maxIntensityUniform = new osg::Uniform("maxIntensity", 0.54f);
    _lightPositionUniform = new osg::Uniform("lightPosition", osg::Vec3d(0.0, 0.0, 0.0));
    _windowDimensionsUniform = new osg::Uniform("windowDimensions", osg::Vec2d(0.0, 0.0));
    //_windowDimensionsUniform = new osg::Uniform("windowDimensions", osg::Vec2d(0.0, 0.0));
	//_lightPositionUniform = new osg::Uniform("lightPosition", osg::Vec3f(0.0f, 0.0f, 0.0f));
	//_windowDimensionsUniform = new osg::Uniform("windowDimensions", osg::Vec2f(0.0f, 0.0f));


    _backfaceTextureEnabledUniform = new osg::Uniform("backfaceTextureEnabled", false);
    _octreeRenderingEnabledUniform = new osg::Uniform("octreeRenderingEnabled", false);
    _lightingEnabledUniform = new osg::Uniform("lightingEnabled", false);
    getOrCreateStateSet()->addUniform(_voxelDimensionsUniform.get());
    getOrCreateStateSet()->addUniform(_cuboidDimensionsUniform.get());
    getOrCreateStateSet()->addUniform(_paddedDepthUniform.get());
    getOrCreateStateSet()->addUniform(_inverseVoxelSpacingUniform.get());
    getOrCreateStateSet()->addUniform(_viewToWorldMatrixUniform.get());
	getOrCreateStateSet()->addUniform(_projectionMatrixUniform.get());
	getOrCreateStateSet()->addUniform(_cameraInsideUniform.get());
	getOrCreateStateSet()->addUniform(_isHMD.get());
	getOrCreateStateSet()->addUniform(_cuboidPosition.get());
	getOrCreateStateSet()->addUniform(_clipPos.get());
	getOrCreateStateSet()->addUniform(_pointerPos.get());
	getOrCreateStateSet()->addUniform(_pointerQuat.get());
	getOrCreateStateSet()->addUniform(_cuboidMatrix.get());
	getOrCreateStateSet()->addUniform(_lOffset.get());
	getOrCreateStateSet()->addUniform(_rOffset.get());

    getOrCreateStateSet()->addUniform(_sampleSizeUniform.get());
    getOrCreateStateSet()->addUniform(_minIntensityUniform.get());
    getOrCreateStateSet()->addUniform(_maxIntensityUniform.get());
    getOrCreateStateSet()->addUniform(_lightPositionUniform.get());
    getOrCreateStateSet()->addUniform(_windowDimensionsUniform.get());
    getOrCreateStateSet()->addUniform(_backfaceTextureEnabledUniform.get());
    getOrCreateStateSet()->addUniform(_octreeRenderingEnabledUniform.get());
    getOrCreateStateSet()->addUniform(_lightingEnabledUniform.get());
    getOrCreateStateSet()->addUniform(new osg::Uniform("voxels", VOXEL_TEXTURE));
    getOrCreateStateSet()->addUniform(new osg::Uniform("octree", OCTREE_TEXTURE));
    getOrCreateStateSet()->addUniform(new osg::Uniform("backfaces", BACKFACES_TEXTURE));
    getOrCreateStateSet()->addUniform(new osg::Uniform("normals", NORMALS_TEXTURE));
    getOrCreateStateSet()->addUniform(new osg::Uniform("opacityTable", OPACITY_TABLE_TEXTURE));
    getOrCreateStateSet()->addUniform(new osg::Uniform("colorTable", COLOR_TABLE_TEXTURE));
    _opacityTables.loadOpacityTable("Linear Smooth", getOrCreateStateSet(), OPACITY_TABLE_TEXTURE);
    //_colorTables.loadColorTable("Muscle and Bone", getOrCreateStateSet(), COLOR_TABLE_TEXTURE);
	_colorTables.loadColorTable("Muscle and Bone", getOrCreateStateSet(), COLOR_TABLE_TEXTURE);
	//_colorTables.loadColorTable("Cardiac", getOrCreateStateSet(), COLOR_TABLE_TEXTURE);
	// Enable alpha blending on the cuboid stateset
	getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);

	storedValue = 0; storedValue2 = 0;
}

TexturedCuboid::TexturedCuboid(const TexturedCuboid& cuboid, const osg::CopyOp& copyop) : osg::Geode(cuboid, copyop)
{
    _image = cuboid._image;
    _voxelTexture = cuboid._voxelTexture;
    //_faces = cuboid._faces;
    _voxelDimensionsUniform = cuboid._voxelDimensionsUniform;
    _cuboidDimensionsUniform = cuboid._cuboidDimensionsUniform;
    _paddedDepthUniform = cuboid._paddedDepthUniform;
    _inverseVoxelSpacingUniform = cuboid._inverseVoxelSpacingUniform;
	//_viewToWorldMatrixUniform->setType(osg::Uniform::FLOAT_MAT4);
	
    _viewToWorldMatrixUniform = cuboid._viewToWorldMatrixUniform;
	_projectionMatrixUniform = cuboid._projectionMatrixUniform;
	_clipPos = cuboid._clipPos;
	_cuboidPosition = cuboid._cuboidPosition;
	

	_sampleSizeUniform = cuboid._sampleSizeUniform;
    _minIntensityUniform = cuboid._minIntensityUniform;
	_maxIntensityUniform = cuboid._maxIntensityUniform;
	storedValue = 0; storedValue2 = 0;
}

TexturedCuboid::~TexturedCuboid()
{
    ;
}

void TexturedCuboid::build(const osg::Vec3d& voxelDimensions,
                           const osg::Vec3d& voxelSpacing,
                           void* voxels,
                           GLenum voxelChannelType,
                           GLenum voxelDataType)
{
    // Create the bounding box from the voxel dimensions and the voxel spacing
    osg::Vec3d min_corner(0.0f, 0.0f, 0.0f);
    osg::Vec3d max_corner(voxelDimensions[0] * voxelSpacing[0],
                          voxelDimensions[1] * voxelSpacing[1],
                          voxelDimensions[2] * voxelSpacing[2]);
    osg::BoundingBox bounds(min_corner, max_corner);

    // Set the cuboid dimensions uniform
    _cuboidDimensionsUniform->set(max_corner);
    _cuboidDimensions = max_corner;

    // Set the inverse voxel spacing uniform
    osg::Vec3d inverse_voxel_spacing(1.0f / voxelSpacing[0], 1.0f / voxelSpacing[1], 1.0f / voxelSpacing[2]);
    _inverseVoxelSpacingUniform->set(inverse_voxel_spacing);

    // Build the image (with padding, then the 3D texture and finally the cuboid face geometry
    _image = buildImage(voxelDimensions, voxels, voxelChannelType, voxelDataType);
    _voxelTexture = buildVoxelTexture(_image);
    _faces = buildFaces(bounds, _voxelTexture);
	//buildFaces(bounds, _voxelTexture);
    // Attach the voxel texture
    getOrCreateStateSet()->setTextureAttributeAndModes(VOXEL_TEXTURE, _voxelTexture.get());
}

void TexturedCuboid::setBackfacesTextureUniform(osg::ref_ptr<osg::Texture2D> texture)
{
    // Attach the backfaces texture
    getOrCreateStateSet()->setTextureAttributeAndModes(BACKFACES_TEXTURE, texture.get());
}

osg::ref_ptr<osg::Image> TexturedCuboid::buildImage(const osg::Vec3d& dimensions, void* voxels, GLenum voxelChannelType, GLenum voxelDataType)
{
    osg::ref_ptr<osg::Image> image = new osg::Image();

    // Pad the depth if it isn't a power of two
    osg::Vec3d padded_dimensions = dimensions;
    void* padded_voxels = voxels;
    int padded_depth = osg::Image::computeNearestPowerOfTwo(dimensions[2]);
    if (padded_depth != dimensions[2])
    {
        // If the padded depth is actually smaller that the actual depth, we need to raise it
        // to the next power of two. For example, if the depth came in as 75, the padded depth
        // would now be 64, but it needs to be 128. This check handles this edge case
        if (padded_depth < dimensions[2])
        {
            padded_depth *= 2;
        }
        padded_dimensions[2] = padded_depth;

        vipreLogDEBUG(viprePrefix) << "Padding the texture depth from " << dimensions[2] << " to " << padded_depth << std::endl;

        // Pad the voxels using the templated padVoxels method
        if (voxelDataType == GL_UNSIGNED_BYTE)
        {
            padded_voxels = padVoxels(dimensions, padded_dimensions, (unsigned char*)voxels);
        }
        else if (voxelDataType == GL_BYTE)
        {
            padded_voxels = padVoxels(dimensions, padded_dimensions, (char*)voxels);
        }
        else if (voxelDataType == GL_UNSIGNED_SHORT)
        {
            padded_voxels = padVoxels(dimensions, padded_dimensions, (unsigned short*)voxels);
        }
        else if (voxelDataType == GL_SHORT)
        {
            padded_voxels = padVoxels(dimensions, padded_dimensions, (short*)voxels);
        }
        else if (voxelDataType == GL_UNSIGNED_INT)
        {
            padded_voxels = padVoxels(dimensions, padded_dimensions, (unsigned int*)voxels);
        }
        else // GL_INT
        {
            padded_voxels = padVoxels(dimensions, padded_dimensions, (int*)voxels);
        }
    }

    // Build the octree texture
    if (voxelDataType == GL_UNSIGNED_BYTE)
    {
        _octreeTexture = buildOctreeTexture(padded_dimensions, (unsigned char*)padded_voxels, voxelDataType, 255, 0);
    }
    else if (voxelDataType == GL_BYTE)
    {
        _octreeTexture = buildOctreeTexture(padded_dimensions, (char*)padded_voxels, voxelDataType, 127, -128);
    }
    else if (voxelDataType == GL_UNSIGNED_SHORT)
    {
        _octreeTexture = buildOctreeTexture(padded_dimensions, (unsigned short*)padded_voxels, voxelDataType, 65535, 0);
    }
    else if (voxelDataType == GL_SHORT)
    {
        _octreeTexture = buildOctreeTexture(padded_dimensions, (short*)padded_voxels, voxelDataType, 32767, -32768);
    }
    else if (voxelDataType == GL_UNSIGNED_INT)
    {
        _octreeTexture = buildOctreeTexture(padded_dimensions, (unsigned int*)padded_voxels, voxelDataType, 4294967295, 0);
    }
    else // GL_INT
    {
        _octreeTexture = buildOctreeTexture(padded_dimensions, (int*)padded_voxels, voxelDataType, 2147483647, -2147483648);
    }
    
    // Attach the octree texture
    getOrCreateStateSet()->setTextureAttributeAndModes(OCTREE_TEXTURE, _octreeTexture);

    // Finally build the image using the padded depth and padded voxels
    image->setImage(padded_dimensions[0],
                    padded_dimensions[1],
                    padded_dimensions[2],
                    voxelChannelType,
                    voxelChannelType,
                    voxelDataType,
                    (unsigned char*)padded_voxels,
                    osg::Image::NO_DELETE);

    // Set the voxel dimensions and padded depth uniforms
    _voxelDimensionsUniform->set(dimensions);
    _paddedDepthUniform->set((float)padded_depth - (float)dimensions[2]);

    return image;
}

osg::ref_ptr<osg::Texture3D> TexturedCuboid::buildVoxelTexture(osg::ref_ptr<osg::Image> image)
{
    osg::ref_ptr<osg::Texture3D> texture = new osg::Texture3D();
    texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
    texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
    texture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
    texture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
    texture->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);
    texture->setImage(image);
	texture->setTextureWidth(image->s());
	texture->setTextureHeight(image->t());
	texture->setTextureDepth(image->r());
    return texture;
}
/*
void TexturedCuboid::buildFaces(osg::BoundingBox bounds, osg::ref_ptr<osg::Texture3D> texture)
{
	/************************************************************************
	*  Quick diagram of the bounding box scheme used to create the cuboid
	*
	*
	*       p3                                p2
	*     (0,H,0)                          (W,H,0)
	*            -----------------------------
	*           |\                            |\
	*           | \                           | \
	*           |  \                          |  \
	*           |   \                         |   \    p6
	*           | p7 \ -----------------------|----- (W,H,D)
	*         (0,H,D) |                       |     |
	*           |     |                       |     |
	*           |     |                       |     |
	*           |     |                       |    p1
	*      p0   | ----|------------------------  (W,0,0)
	*   (0,0,0)  \    |                        \    |
	*             \   |                         \   |
	*              \  |                          \  |
	*               \ |                           \ |
	*                \|                            \|
	*                  -----------------------------
	*               (0,D,0)                       (W,0,D)
	*                 p4                            p5
	*
	************************************************************************




	// Create all the vertices
	osg::Vec3d p0(bounds.xMin(), bounds.yMin(), bounds.zMin());
	osg::Vec3d p1(bounds.xMax(), bounds.yMin(), bounds.zMin());
	osg::Vec3d p2(bounds.xMax(), bounds.yMax(), bounds.zMin());
	osg::Vec3d p3(bounds.xMin(), bounds.yMax(), bounds.zMin());
	osg::Vec3d p4(bounds.xMin(), bounds.yMin(), bounds.zMax());
	osg::Vec3d p5(bounds.xMax(), bounds.yMin(), bounds.zMax());
	osg::Vec3d p6(bounds.xMax(), bounds.yMax(), bounds.zMax());
	osg::Vec3d p7(bounds.xMin(), bounds.yMax(), bounds.zMax());

	// Inferior (feet of body, front of view)
	{
		osg::Vec4d color(45.0f / 255.0f, 141.0f / 255.0f, 1.0f, 1.0f); // blue
		osg::Vec3d normal(0.0f, 0.0f, 1.0f);
		osg::ref_ptr<osg::Vec3dArray> vertices = new osg::Vec3dArray;
		vertices->push_back(p4);
		vertices->push_back(p5);
		vertices->push_back(p6);
		vertices->push_back(p7);
		osg::ref_ptr<osg::Geometry> geometry = buildFace(vertices, color, normal);
		this->addDrawable(geometry.get());

	}

	// Superior (head of body, back of view)
	{
		osg::Vec4d color(136.0f / 255.0f, 207.0f / 255.0f, 8.0f / 255.0f, 1.0f); // green
		osg::Vec3d normal(0.0f, 0.0f, -1.0f);
		osg::ref_ptr<osg::Vec3dArray> vertices = new osg::Vec3dArray;
		vertices->push_back(p3);
		vertices->push_back(p2);
		vertices->push_back(p1);
		vertices->push_back(p0);
		osg::ref_ptr<osg::Geometry> geometry = buildFace(vertices, color, normal);
		this->addDrawable(geometry.get());

	}

	// Left (left side of body, right side of view)
	{
		osg::Vec4d color(166.0f / 255.0f, 131.0f / 255.0f, 240.0f / 255.0f, 1.0f); // purple
		osg::Vec3d normal(1.0f, 0.0f, 0.0f);
		osg::ref_ptr<osg::Vec3dArray> vertices = new osg::Vec3dArray;
		vertices->push_back(p1);
		vertices->push_back(p2);
		vertices->push_back(p6);
		vertices->push_back(p5);
		osg::ref_ptr<osg::Geometry> geometry = buildFace(vertices, color, normal);
		this->addDrawable(geometry.get());

	}

	// Right (right side of body, left side of view)
	{
		osg::Vec4d color(233.0f / 255.0f, 165.0f / 255.0f, 47.0f / 255.0f, 1.0f); // orange
		osg::Vec3d normal(-1.0f, 0.0f, 0.0f);
		osg::ref_ptr<osg::Vec3dArray> vertices = new osg::Vec3dArray;
		vertices->push_back(p0);
		vertices->push_back(p4);
		vertices->push_back(p7);
		vertices->push_back(p3);
		osg::ref_ptr<osg::Geometry> geometry = buildFace(vertices, color, normal);
		this->addDrawable(geometry.get());

	}

	// Anterior (torso of body, top of view)
	{
		osg::Vec4d color(219.0f / 255.0f, 44.0f / 255.0f, 56.0f / 255.0f, 1.0f); // red
		osg::Vec3d normal(0.0f, 1.0f, 0.0f);
		osg::ref_ptr<osg::Vec3dArray> vertices = new osg::Vec3dArray;
		vertices->push_back(p2);
		vertices->push_back(p3);
		vertices->push_back(p7);
		vertices->push_back(p6);
		osg::ref_ptr<osg::Geometry> geometry = buildFace(vertices, color, normal);
		this->addDrawable(geometry.get());
	}

	// Posterior (butt of body, bottom of view)
	{
		osg::Vec4d color(63.f / 255.0f, 63.f / 110.0f, 116.f / 255.0f, 1.0f); // aqua green
		osg::Vec3d normal(0.0f, -1.0f, 0.0f);
		osg::ref_ptr<osg::Vec3dArray> vertices = new osg::Vec3dArray;
		vertices->push_back(p0);
		vertices->push_back(p1);
		vertices->push_back(p5);
		vertices->push_back(p4);
		osg::ref_ptr<osg::Geometry> geometry = buildFace(vertices, color, normal);
		this->addDrawable(geometry.get());
	}

}
*/

osg::Geode::DrawableList TexturedCuboid::buildFaces(osg::BoundingBox bounds, osg::ref_ptr<osg::Texture3D> texture)
{
    /************************************************************************
     *  Quick diagram of the bounding box scheme used to create the cuboid
     *
     *
     *       p3                                p2
     *     (0,H,0)                          (W,H,0)
     *            -----------------------------
     *           |\                            |\
     *           | \                           | \
     *           |  \                          |  \
     *           |   \                         |   \    p6
     *           | p7 \ -----------------------|----- (W,H,D)
     *         (0,H,D) |                       |     |
     *           |     |                       |     |
     *           |     |                       |     |
     *           |     |                       |    p1
     *      p0   | ----|------------------------  (W,0,0)
     *   (0,0,0)  \    |                        \    |
     *             \   |                         \   |
     *              \  |                          \  |
     *               \ |                           \ |
     *                \|                            \|
     *                  -----------------------------
     *               (0,D,0)                       (W,0,D)
     *                 p4                            p5
     *
     ***********************************************************************

	 */

    // Create a drawable list
    osg::Geode::DrawableList drawables;

    // Create all the vertices
    osg::Vec3d p0(bounds.xMin(), bounds.yMin(), bounds.zMin());
	osg::Vec3d p1(bounds.xMax(), bounds.yMin(), bounds.zMin());
	osg::Vec3d p2(bounds.xMax(), bounds.yMax(), bounds.zMin());
	osg::Vec3d p3(bounds.xMin(), bounds.yMax(), bounds.zMin());
	osg::Vec3d p4(bounds.xMin(), bounds.yMin(), bounds.zMax());
	osg::Vec3d p5(bounds.xMax(), bounds.yMin(), bounds.zMax());
	osg::Vec3d p6(bounds.xMax(), bounds.yMax(), bounds.zMax());
	osg::Vec3d p7(bounds.xMin(), bounds.yMax(), bounds.zMax());

    // Inferior (feet of body, front of view)
    {
        osg::Vec4d color(45.0f / 255.0f, 141.0f / 255.0f, 1.0f, 1.0f); // blue
        osg::Vec3d normal(0.0f, 0.0f, -1.0f);
        osg::ref_ptr<osg::Vec3dArray> vertices = new osg::Vec3dArray;
        vertices->push_back(p4);
        vertices->push_back(p5);
        vertices->push_back(p6);
        vertices->push_back(p7);
        osg::ref_ptr<osg::Geometry> geometry = buildFace(vertices, color, normal);
        addDrawable(geometry.get());
        drawables.push_back(geometry);
    }

    // Superior (head of body, back of view)
    {
        osg::Vec4d color(136.0f / 255.0f, 207.0f / 255.0f, 8.0f / 255.0f, 1.0f); // green
        osg::Vec3d normal(0.0f, 0.0f, 1.0f);
        osg::ref_ptr<osg::Vec3dArray> vertices = new osg::Vec3dArray;
        vertices->push_back(p3);
        vertices->push_back(p2);
        vertices->push_back(p1);
        vertices->push_back(p0);
        osg::ref_ptr<osg::Geometry> geometry = buildFace(vertices, color, normal);
        addDrawable(geometry.get());
        drawables.push_back(geometry);
    }

    // Left (left side of body, right side of view)
    {
        osg::Vec4d color(166.0f / 255.0f, 131.0f / 255.0f, 240.0f / 255.0f, 1.0f); // purple
        osg::Vec3d normal(1.0f, 0.0f, 0.0f);
        osg::ref_ptr<osg::Vec3dArray> vertices = new osg::Vec3dArray;
        vertices->push_back(p1);
        vertices->push_back(p2);
        vertices->push_back(p6);
        vertices->push_back(p5);
        osg::ref_ptr<osg::Geometry> geometry = buildFace(vertices, color, normal);
        addDrawable(geometry.get());
        drawables.push_back(geometry);
    }

    // Right (right side of body, left side of view)
    {
        osg::Vec4d color(233.0f / 255.0f, 165.0f / 255.0f, 47.0f / 255.0f, 1.0f); // orange
        osg::Vec3d normal(-1.0f, 0.0f, 0.0f);
        osg::ref_ptr<osg::Vec3dArray> vertices = new osg::Vec3dArray;
        vertices->push_back(p0);
        vertices->push_back(p4);
        vertices->push_back(p7);
        vertices->push_back(p3);
        osg::ref_ptr<osg::Geometry> geometry = buildFace(vertices, color, normal);
        addDrawable(geometry.get());
        drawables.push_back(geometry);
    }

    // Anterior (torso of body, top of view)
    {
        osg::Vec4d color(219.0f / 255.0f, 44.0f / 255.0f, 56.0f / 255.0f, 1.0f); // red
        osg::Vec3d normal(0.0f, 1.0f, 0.0f);
        osg::ref_ptr<osg::Vec3dArray> vertices = new osg::Vec3dArray;
        vertices->push_back(p2);
        vertices->push_back(p3);
        vertices->push_back(p7);
        vertices->push_back(p6);
        osg::ref_ptr<osg::Geometry> geometry = buildFace(vertices, color, normal);
        addDrawable(geometry.get());
        drawables.push_back(geometry);
    }

    // Posterior (butt of body, bottom of view)
    {
        osg::Vec4d color(63.f / 255.0f, 63.f / 110.0f, 116.f / 255.0f, 1.0f); // aqua green
        osg::Vec3d normal(0.0f, -1.0f, 0.0f);
        osg::ref_ptr<osg::Vec3dArray> vertices = new osg::Vec3dArray;
        vertices->push_back(p0);
        vertices->push_back(p1);
        vertices->push_back(p5);
        vertices->push_back(p4);
        osg::ref_ptr<osg::Geometry> geometry = buildFace(vertices, color, normal);
        addDrawable(geometry.get());
        drawables.push_back(geometry);
    }

    return drawables;
}



osg::ref_ptr<osg::Geometry> TexturedCuboid::buildFace(osg::ref_ptr<osg::Vec3dArray> vertices,
                                                      const osg::Vec4d& color,
                                                      const osg::Vec3d& normal)
{
    // Create the geometry node
    osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();

    // Apply the vertices
    geometry->setVertexArray(vertices.get());
    geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, vertices->size()));

    // Set the color
	osg::ref_ptr<osg::Vec4dArray> colors = new osg::Vec4dArray;
    colors->push_back(color);
	geometry->setColorArray(colors.get());
	geometry->setColorBinding(osg::Geometry::BIND_OVERALL);

    // Set the normal
	osg::ref_ptr<osg::Vec3dArray> normals = new osg::Vec3dArray;
    normals->push_back(normal);
	geometry->setNormalArray(normals.get());
	geometry->setNormalBinding(osg::Geometry::BIND_OVERALL);

    return geometry;
}
