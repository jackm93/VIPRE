//
//  TexturedQuad.hpp
//  vipre
//
//  Created by Christian Noon on 12/4/11.
//  Copyright (c) 2011 Christian Noon. All rights reserved.
//

#ifndef VIPRE_TEXTURED_QUAD_HPP
#define VIPRE_TEXTURED_QUAD_HPP

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/ref_ptr>
#include <osg/Texture2D>

#include <vipre/Export.hpp>

namespace vipre {

class VIPRE_EXPORT TexturedQuad : public osg::Geode
{
public:

    /** Default constructor. */
    TexturedQuad();

    /** Copy constructor using CopyOp to manage deep vs. shallow copy. */
    TexturedQuad(const TexturedQuad& quad, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY);

    /** Convenience macro defines the standard clone, isSameKindAs, className and accept methods. */
    META_Node(vipre, TexturedQuad);

    /**
     * Builds a 2D textured quad from the given parameters.
     *
     * @param vertices - the array of corner vertices
     * @param voxelDimensions - the width and height of the voxel data
     * @param voxels - the image data stored in a 1D array
     * @param voxelChannelType - the number of channels contained in the voxel data
     *     - GL_LUMINANCE
     *     - GL_RGB
     *     - GL_RGBA
     * @param voxelDataType - the data type of the voxels
     *     - GL_UNSIGNED_BYTE
     *     - GL_BYTE
     *     - GL_UNSIGNED_SHORT
     *     - GL_SHORT
     *     - GL_UNSIGNED_INT
     *     - GL_INT
     */
    void build(osg::ref_ptr<osg::Vec3dArray> vertices,
               const osg::Vec2d& voxelDimensions,
               void* voxels,
               GLenum voxelChannelType,
               GLenum voxelDataType);

    /** Scales the voxel image applied to the texture to the given dimensions. */
    void scaleImage(const osg::Vec2d& dimensions);

    /** Returns the power of two number closest to the given value. */
    static double computeNearestPowerOfTwo(double value);

    /** Returns the voxel image. */
    inline osg::ref_ptr<osg::Image> getImage() const { return _image; }

    /** Returns the voxel texture. */
    inline osg::ref_ptr<osg::Texture2D> getTexture() const { return _texture; }

    /** Returns the quad geometry. */
    inline osg::ref_ptr<osg::Geometry> getGeometry() const { return _geometry; }

    /** Returns the vertices of the quad. */
    inline osg::ref_ptr<osg::Vec3dArray> getVertices() const { return _vertices; }

protected:

    /** Destructor. */
    ~TexturedQuad();

    /**
     * Builds a 2D image from the given parameters.
     *
     * @param dimensions - the width and height of the voxel data
     * @param voxels - the image data stored in a 1D array
     * @param voxelChannelType - the number of channels contained in the voxel data
     *     - GL_LUMINANCE
     *     - GL_RGB
     *     - GL_RGBA
     * @param voxelDataType - the data type of the voxels
     *     - GL_UNSIGNED_BYTE
     *     - GL_BYTE
     *     - GL_UNSIGNED_SHORT
     *     - GL_SHORT
     *     - GL_UNSIGNED_INT
     *     - GL_INT
     */
    osg::ref_ptr<osg::Image> buildImage(const osg::Vec2d& dimensions, void* voxels, GLenum voxelChannelType, GLenum voxelDataType);

    /**
     * Builds a 2D texture from the image.
     *
     * @param image - a 2D image
     */
    osg::ref_ptr<osg::Texture2D> buildTexture(osg::ref_ptr<osg::Image> image);

    /**
     * Builds a textured quad with corners located at vertices.
     *
     * @param vertices - the array of corner vertices
     * @param texture - the 2D texture to apply to the geometry stateset
     */
    osg::ref_ptr<osg::Geometry> buildGeometry(osg::ref_ptr<osg::Vec3dArray> vertices, osg::ref_ptr<osg::Texture2D> texture);

    /** Instance member variables. */
    osg::ref_ptr<osg::Image> _image;
    osg::ref_ptr<osg::Texture2D> _texture;
    osg::ref_ptr<osg::Geometry> _geometry;
    osg::ref_ptr<osg::Vec3dArray> _vertices;
};

}   // End of vipre namespace

#endif  // End of VIPRE_TEXTURED_QUAD_HPP
