//
//  TexturedQuad.cpp
//  vipre
//
//  Created by Christian Noon on 12/4/11.
//  Copyright (c) 2011 Christian Noon. All rights reserved.
//

#include <vipre/TexturedQuad.hpp>

using namespace vipre;

TexturedQuad::TexturedQuad() : osg::Geode()
{
    ;
}

TexturedQuad::TexturedQuad(const TexturedQuad& quad, const osg::CopyOp& copyop) : osg::Geode(quad, copyop)
{
    _image = quad._image;
    _texture = quad._texture;
    _geometry = quad._geometry;
    _vertices = quad._vertices;
}

TexturedQuad::~TexturedQuad()
{
	;
}

void TexturedQuad::build(osg::ref_ptr<osg::Vec3dArray> vertices,
                         const osg::Vec2d& voxelDimensions,
                         void* voxels,
                         GLenum voxelChannelType,
                         GLenum voxelDataType)
{
    _vertices = vertices;
    _image = buildImage(voxelDimensions, voxels, voxelChannelType, voxelDataType);
    _texture = buildTexture(_image);
    _geometry = buildGeometry(_vertices, _texture);
}

void TexturedQuad::scaleImage(const osg::Vec2d& dimensions)
{
    if (_image.valid())
    {
        _image->scaleImage(dimensions[0], dimensions[1], 1);
        if (_texture.valid())
        {
            _texture->setTextureSize(dimensions[0], dimensions[1]);
            _texture->dirtyTextureObject();
            _texture->dirtyTextureParameters();
        }
    }
}

double TexturedQuad::computeNearestPowerOfTwo(double value)
{
    return osg::Image::computeNearestPowerOfTwo(value);
}

osg::ref_ptr<osg::Image> TexturedQuad::buildImage(const osg::Vec2d& dimensions, void* voxels, GLenum voxelChannelType, GLenum voxelDataType)
{
    osg::ref_ptr<osg::Image> image = new osg::Image();
    image->setImage(dimensions[0],
                    dimensions[1],
                    1,
                    voxelChannelType,
                    voxelChannelType,
                    voxelDataType,
                    (unsigned char*)voxels,
                    osg::Image::NO_DELETE);

    return image;
}

osg::ref_ptr<osg::Texture2D> TexturedQuad::buildTexture(osg::ref_ptr<osg::Image> image)
{
    osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D();
    texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
    texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
    texture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
    texture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
    texture->setImage(image);

    return texture;
}

osg::ref_ptr<osg::Geometry> TexturedQuad::buildGeometry(osg::ref_ptr<osg::Vec3dArray> vertices, osg::ref_ptr<osg::Texture2D> texture)
{
    // Create the geometry node
    osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();
    addDrawable(geometry.get());

    // Apply the vertices
    geometry->setVertexArray(vertices.get());
    geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, vertices->size()));

    // Apply the texture coordinates
    osg::ref_ptr<osg::Vec2Array> texCoords = new osg::Vec2Array();
    texCoords->push_back(osg::Vec2d(0.0f, 0.0f));
    texCoords->push_back(osg::Vec2d(1.0f, 0.0f));
    texCoords->push_back(osg::Vec2d(1.0f, 1.0f));
    texCoords->push_back(osg::Vec2d(0.0f, 1.0f));
    geometry->setTexCoordArray(0, texCoords);

    // Apply the texture to the geometry stateset
    osg::ref_ptr<osg::StateSet> stateset = geometry->getOrCreateStateSet();
    stateset->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
    stateset->setMode(GL_LIGHTING, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF);

    return geometry;
}
