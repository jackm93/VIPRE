//
//  VoxelDataTypeTemplate.hpp
//  vipreDICOM
//
//  Created by Christian Noon on 12/2/11.
//  Copyright (c) 2011 Christian Noon. All rights reserved.
//

#ifndef VIPREDICOM_VOXEL_DATA_TYPE_TEMPLATE_HPP
#define VIPREDICOM_VOXEL_DATA_TYPE_TEMPLATE_HPP

#include <vipreDICOM/Export.hpp>
#include <vipreDICOM/VoxelData.hpp>

namespace vipreDICOM {

/**
 * This templated class abstracts allows the data type and data type size to be queried using
 * the template type.
 */
template <class T>
class VIPREDICOM_EXPORT VoxelDataTypeTemplate
{
public:

    /** Constructor. */
    VoxelDataTypeTemplate() {}

    /** Destructor. */
    ~VoxelDataTypeTemplate() {}

    /** Returns the voxel data type. */
    inline VoxelDataType getTemplateDataType();

    /** Returns the voxel data type as a vipre::String. */
    inline vipre::String getTemplateDataTypeAsString();

    /** Returns the data type size in bytes. */
    inline unsigned int getTemplateDataTypeSizeInBytes();
};

//================================================================================================
// Overloads for getTemplateDataType()
template <>
inline VoxelDataType VoxelDataTypeTemplate<unsigned char>::getTemplateDataType()
{
    return UNSIGNED_INT_8;
}

template <>
inline VoxelDataType VoxelDataTypeTemplate<char>::getTemplateDataType()
{
    return SIGNED_INT_8;
}

template <>
inline VoxelDataType VoxelDataTypeTemplate<unsigned short>::getTemplateDataType()
{
    return UNSIGNED_INT_16;
}

template <>
inline VoxelDataType VoxelDataTypeTemplate<short>::getTemplateDataType()
{
    return SIGNED_INT_16;
}

template <>
inline VoxelDataType VoxelDataTypeTemplate<unsigned int>::getTemplateDataType()
{
    return UNSIGNED_INT_32;
}

template <>
inline VoxelDataType VoxelDataTypeTemplate<int>::getTemplateDataType()
{
    return SIGNED_INT_32;
}

//================================================================================================
// Overloads for getTemplateDataTypeAsString()
template <>
inline vipre::String VoxelDataTypeTemplate<unsigned char>::getTemplateDataTypeAsString()
{
    return "UNSIGNED_INT_8";
}

template <>
inline vipre::String VoxelDataTypeTemplate<char>::getTemplateDataTypeAsString()
{
    return "SIGNED_INT_8";
}

template <>
inline vipre::String VoxelDataTypeTemplate<unsigned short>::getTemplateDataTypeAsString()
{
    return "UNSIGNED_INT_16";
}

template <>
inline vipre::String VoxelDataTypeTemplate<short>::getTemplateDataTypeAsString()
{
    return "SIGNED_INT_16";
}

template <>
inline vipre::String VoxelDataTypeTemplate<unsigned int>::getTemplateDataTypeAsString()
{
    return "UNSIGNED_INT_32";
}

template <>
inline vipre::String VoxelDataTypeTemplate<int>::getTemplateDataTypeAsString()
{
    return "SIGNED_INT_32";
}

//================================================================================================
// Overloads for getTemplateDataTypeSizeInBytes()
template <>
inline unsigned int VoxelDataTypeTemplate<unsigned char>::getTemplateDataTypeSizeInBytes()
{
    return 1;
}

template <>
inline unsigned int VoxelDataTypeTemplate<char>::getTemplateDataTypeSizeInBytes()
{
    return 1;
}

template <>
inline unsigned int VoxelDataTypeTemplate<unsigned short>::getTemplateDataTypeSizeInBytes()
{
    return 2;
}

template <>
inline unsigned int VoxelDataTypeTemplate<short>::getTemplateDataTypeSizeInBytes()
{
    return 2;
}

template <>
inline unsigned int VoxelDataTypeTemplate<unsigned int>::getTemplateDataTypeSizeInBytes()
{
    return 4;
}

template <>
inline unsigned int VoxelDataTypeTemplate<int>::getTemplateDataTypeSizeInBytes()
{
    return 4;
}

}   // End of vipreDICOM namespace

#endif  // End of VIPREDICOM_VOXEL_DATA_TYPE_TEMPLATE_HPP
