//
//  TexturedCuboid.hpp
//  vipre
//
//  Created by Christian Noon on 2/11/12.
//  Copyright (c) 2012 Christian Noon. All rights reserved.
//

#ifndef VIPRE_TEXTURED_CUBOID_HPP
#define VIPRE_TEXTURED_CUBOID_HPP

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/ref_ptr>
#include <osg/Texture2D>
#include <osg/Texture3D>
#include <osgDB/DatabasePager>
#include <vipre/ColorTables.hpp>
#include <vipre/Export.hpp>
#include <vipre/OpacityTables.hpp>

#define VOXEL_TEXTURE               0
#define OCTREE_TEXTURE              1
#define BACKFACES_TEXTURE           2
#define NORMALS_TEXTURE             3
#define OPACITY_TABLE_TEXTURE       4
#define COLOR_TABLE_TEXTURE         5
namespace vipre {

class VIPRE_EXPORT TexturedCuboid : public osg::Geode
{
public:

    /** Default constructor. */
    TexturedCuboid();

    /** Copy constructor using CopyOp to manage deep vs. shallow copy. */
    TexturedCuboid(const TexturedCuboid& cuboid, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY);

    /** Convenience macro defines the standard clone, isSameKindAs, className and accept methods. */
    META_Node(vipre, TexturedCuboid);

    /**
     * Builds a 3D textured cuboid from the given parameters.
     *
     * @param voxelDimensions - the width, height and depth of the voxel data
     * @param voxelSpacing - the width spacing, height spacing and depth spacing for all the voxel data
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
    void build(const osg::Vec3d& voxelDimensions,
               const osg::Vec3d& voxelSpacing,
               void* voxels,
               GLenum voxelChannelType,
               GLenum voxelDataType);

    /** Returns the voxel image. */
    inline osg::ref_ptr<osg::Image> getImage() { return _image; }

    /** Returns the voxel texture. */
    inline osg::ref_ptr<osg::Texture3D> getVoxelTexture() { return _voxelTexture; }

    /** Returns the faces drawable list. */
    inline const osg::Geode::DrawableList& getFaces() { return _faces; }

    /** Returns the cuboid dimensions. */
    inline const osg::Vec3d& getCuboidDimensions() { return _cuboidDimensions; }

    /** Sets the view-to-world matrix uniform. */
    inline void setViewToWorldMatrixUniform(const osg::Matrixf matrix) { _viewToWorldMatrixUniform->set(matrix); }

	//set camera inside/outside
	inline void setCameraInsideVolumeUniform(bool inside) { _cameraInsideUniform->set(inside);  }

	/** Sets the projection matrix uniform */
	inline void setProjectionMatrixUniform(const osg::Matrixf matrix) { _projectionMatrixUniform->set(matrix); }

	inline osg::Matrixf* getCuboidMatrix() { return &_cuboidRotation; }

	inline void setHMDBool(const bool isHMD) { _isHMD->set(isHMD); }

	inline void setCuboidPosition(const osg::Vec3f cuboidPosition) { _cuboidPosition->set(cuboidPosition); }
	inline void setCuboidRotation(const osg::Matrix cuboidMatrix) { _cuboidMatrix->set(cuboidMatrix); _cuboidRotation = cuboidMatrix; }

	inline void setClipPosition(const float clipPos) { _clipPos->set(clipPos); }

	inline void setPointerPosUniform(osg::Vec3 pos) { _pointerPos->set(pos); }
	inline void setPointerQuatUniform(osg::Vec3 pointerquat) { _pointerQuat->set(pointerquat); }
	inline void setLOffset(float lOffset) { _lOffset->set(lOffset); }
	inline void setROffset(float rOffset) { _rOffset->set(rOffset); }
    /** Sets the min intensity uniform. */
    inline void setMinIntensityUniform(float intensity) { _minIntensityUniform->set(intensity); }

    /** Sets the max intensity uniform. */
    inline void setMaxIntensityUniform(float intensity) { _maxIntensityUniform->set(intensity); }

    /** Sets the light position uniform. */
    inline void setLightPositionUniform(osg::Vec3d position) { _lightPositionUniform->set(position); }

    /** Sets the window dimensions uniform. */
    inline void setWindowDimensionsUniform(osg::Vec2d dimensions) { _windowDimensionsUniform->set(dimensions); }

    /** Sets the backfaces texture uniform. */
    void setBackfacesTextureUniform(osg::ref_ptr<osg::Texture2D> texture);

    /** Sets the opacity table texture. */
    inline void setOpacityTable(String& table) { _opacityTables.loadOpacityTable(table, getOrCreateStateSet(), OPACITY_TABLE_TEXTURE); }

    /** Sets the color table texture. */
    inline void setColorTable(String& table) { _colorTables.loadColorTable(table, getOrCreateStateSet(), COLOR_TABLE_TEXTURE); }

    /** Sets whether the backface texture is enabled. */
    inline void setBackfaceTextureEnabledUniform(bool enabled) { _backfaceTextureEnabledUniform->set(enabled); }

    /** Sets whether octree rendering is enabled. */
    inline void setOctreeRenderingEnabledUniform(bool enabled) { _octreeRenderingEnabledUniform->set(enabled); }

    /** Sets whether lighting is enabled. */
    inline void setLightingEnabledUniform(bool enabled) { _lightingEnabledUniform->set(enabled); }

	

	float storedValue, storedValue2;

protected:

    /** Destructor. */
    ~TexturedCuboid();

    /**
     * Builds a 3D texture from the given parameters.
     *
     * @param dimensions - the width, height and depth of the voxel data
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
    osg::ref_ptr<osg::Image> buildImage(const osg::Vec3d& dimensions, void* voxels, GLenum voxelChannelType, GLenum voxelDataType);

    /**
     * Builds a 3D texture from the image.
     *
     * @param image - a 3D image
     */
    osg::ref_ptr<osg::Texture3D> buildVoxelTexture(osg::ref_ptr<osg::Image> image);

    /**
     * Builds a 3D octree texture from the image at 1/8 the size.
     */
    template <class T>
    osg::ref_ptr<osg::Texture3D> buildOctreeTexture(const osg::Vec3d& dimensions,
                                                    T* voxels,
                                                    GLenum voxelDataType,
                                                    double minValue,
                                                    double maxValue)
    {
        // Create the new octree array which will have three channels (RGB)
        unsigned int block_size = 2;
        osg::Vec3d voxel_dim = dimensions;
        osg::Vec3d octree_dim = voxel_dim / block_size;
        T* octree = new T[(unsigned int)(octree_dim[0] * octree_dim[1] * octree_dim[2]) * 3];

        // Build the octree array
        for (unsigned int k = 0; k < octree_dim[2]; ++k)
        {
            for (unsigned int j = 0; j < octree_dim[1]; ++j)
            {
                for (unsigned int i = 0; i < octree_dim[0]; ++i)
                {
                    // Double all the indices to match the voxel dimensions
                    unsigned int x = i * block_size;
                    unsigned int y = j * block_size;
                    unsigned int z = k * block_size;

                    // Iterate through the 8 values of the octree block and compute the min/max values for the block
                    T min_value = minValue;
                    T max_value = maxValue;
                    for (unsigned int a = z; a < z + block_size; ++a)
                    {
                        for (unsigned int b = y; b < y + block_size; ++b)
                        {
                            for (unsigned int c = x; c < x + block_size; ++c)
                            {
                                unsigned int block_index = (a * voxel_dim[0] * voxel_dim[1]) + (b * voxel_dim[1]) + c;
                                T voxel_value = voxels[block_index];
                                if (voxel_value < min_value)
                                {
                                    min_value = voxel_value;
                                }
                                if (voxel_value > max_value)
                                {
                                    max_value = voxel_value;
                                }
                            }
                        }
                    }

                    // Compute octree index and append the min / max values
                    unsigned int octree_index = (k * octree_dim[0] * octree_dim[1]) + (j * octree_dim[1]) + i;
                    octree[octree_index * 3] = min_value;
                    octree[octree_index * 3 + 1] = max_value;
                    octree[octree_index * 3 + 2] = 0;
                }
            }
        }

        // Build the image
        osg::ref_ptr<osg::Image> octree_image = new osg::Image();
        octree_image->setImage(octree_dim[0],
                               octree_dim[1],
                               octree_dim[2],
                               GL_RGB,
                               GL_RGB,
                               voxelDataType,
                               (unsigned char*)octree,
                               osg::Image::NO_DELETE);

        // Build the octree texture
        osg::ref_ptr<osg::Texture3D> octree_texture = new osg::Texture3D();
        octree_texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::NEAREST);
        octree_texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::NEAREST);
        octree_texture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
        octree_texture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
        octree_texture->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);
        octree_texture->setImage(octree_image);


		octree_texture->setTextureWidth(octree_image->s());
		octree_texture->setTextureHeight(octree_image->t());
		octree_texture->setTextureDepth(octree_image->r());

        return octree_texture;
    }

    /**
     * Builds all the faces for the cuboid based on the bounding box.
     *
     * @param bounds - the bounding box for the cuboid
     * @param texture - the 3D texture to apply to the geode stateset
     */
    osg::Geode::DrawableList buildFaces(osg::BoundingBox bounds, osg::ref_ptr<osg::Texture3D> texture);

	//void buildFaces(osg::BoundingBox bounds, osg::ref_ptr<osg::Texture3D> texture);


    /** Builds a face quad for the vertices passed in. */
    osg::ref_ptr<osg::Geometry> buildFace(osg::ref_ptr<osg::Vec3dArray> vertices, const osg::Vec4d& color, const osg::Vec3d& normal);

    /** Pads the depth of the voxel array with empty data. */
    template <class T>
    T* padVoxels(const osg::Vec3d& voxelDimensions, const osg::Vec3d& paddedDimensions, T* voxels)
    {
        // Compute the voxel count
        unsigned int voxel_count = voxelDimensions[0] * voxelDimensions[1] * voxelDimensions[2];

        // Compute the padded voxel count
        double width = paddedDimensions[0];
        double height = paddedDimensions[1];
        double depth = paddedDimensions[2];
        unsigned int padded_voxel_count = width * height * depth;

        // Populate the padded voxel array
        unsigned int last_slice = 0;
        T* padded_voxels = new T[padded_voxel_count];
        for (int i = 0; i < depth; ++i)
        {
            for (int j = 0; j < height; ++j)
            {
                for (int k = 0; k < width; ++k)
                {
                    unsigned int index = (i * height * width) + (j * width) + k;
                    if (index < voxel_count)
                    {
                        last_slice = i;
                        padded_voxels[index] = voxels[index];
                    }
                    else
                    {
                        // Fill in the padded voxels with the last slice to simulate a clamped z
                        // direction. Otherwise the front-most slice would be interpolated much
                        // lower than it actually is.
                        unsigned int last_slice_index = (last_slice * height * width) + (j * width) + k;
                        padded_voxels[index] = voxels[last_slice_index];
                    }
                }
            }
        }

        return padded_voxels;
    }

    /** Instance member variables. */
    osg::ref_ptr<osg::Image> _image;
    osg::ref_ptr<osg::Texture3D> _voxelTexture;
    osg::ref_ptr<osg::Texture3D> _octreeTexture;
    osg::Geode::DrawableList _faces;
    OpacityTables _opacityTables;
    ColorTables _colorTables;

    osg::Vec3d _cuboidDimensions;
	osg::Matrixf _cuboidRotation;
    osg::ref_ptr<osg::Uniform> _voxelDimensionsUniform;
	osg::ref_ptr<osg::Uniform> _cuboidDimensionsUniform;
	osg::ref_ptr<osg::Uniform> _paddedDepthUniform;
	osg::ref_ptr<osg::Uniform> _inverseVoxelSpacingUniform;
	osg::ref_ptr<osg::Uniform> _viewToWorldMatrixUniform;
	osg::ref_ptr<osg::Uniform> _cameraInsideUniform;

	osg::ref_ptr<osg::Uniform> _projectionMatrixUniform;
	osg::ref_ptr<osg::Uniform> _isHMD;
	osg::ref_ptr<osg::Uniform> _cuboidPosition;
	osg::ref_ptr<osg::Uniform> _clipPos;

    osg::ref_ptr<osg::Uniform> _sampleSizeUniform;
    osg::ref_ptr<osg::Uniform> _minIntensityUniform;
    osg::ref_ptr<osg::Uniform> _maxIntensityUniform;
    osg::ref_ptr<osg::Uniform> _lightPositionUniform;
    osg::ref_ptr<osg::Uniform> _windowDimensionsUniform;
    osg::ref_ptr<osg::Uniform> _backfaceTextureEnabledUniform;
    osg::ref_ptr<osg::Uniform> _octreeRenderingEnabledUniform;
    osg::ref_ptr<osg::Uniform> _lightingEnabledUniform;

	osg::ref_ptr<osg::Uniform> _lOffset;
	osg::ref_ptr<osg::Uniform> _rOffset;

	osg::ref_ptr<osg::Uniform> _pointerQuat;
	osg::ref_ptr<osg::Uniform> _pointerPos;

	osg::ref_ptr<osg::Uniform> _cuboidMatrix;

	osg::Uniform::Type unType;
};

}   // End of vipre namespace

#endif  // End of VIPRE_TEXTURED_CUBOID_HPP
