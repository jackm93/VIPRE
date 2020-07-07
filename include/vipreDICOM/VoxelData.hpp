//
//  VoxelData.hpp
//  vipreDICOM
//
//  Created by Christian Noon on 12/2/11.
//  Copyright (c) 2011 Christian Noon. All rights reserved.
//

#ifndef VIPREDICOM_VOXEL_DATA_HPP
#define VIPREDICOM_VOXEL_DATA_HPP

#include <osg/Referenced>

#include <vipre/String.hpp>
#include <vipreDICOM/Export.hpp>

namespace vipreDICOM {

/** Contains all the voxel data types supported by the vipreDICOM library. */
enum VoxelDataType
{
    UNSIGNED_INT_8,
    SIGNED_INT_8,
    UNSIGNED_INT_16,
    SIGNED_INT_16,
    UNSIGNED_INT_32,
    SIGNED_INT_32
};

/**
 * Abstract base class for all voxel data template subclasses.
 *
 * The voxel data interface can be used to abstract multi-channel datasets using templated subclasses.
 * A user will never instantiate a VoxelData object directly.
 */
class VIPREDICOM_EXPORT VoxelData : public osg::Referenced
{
public:

    /** Returns the voxel data type. */
    virtual VoxelDataType getDataType() = 0;

    /** Returns the voxel data type as a vipre::String. */
    virtual vipre::String getDataTypeAsString() = 0;

    /** Returns the data type size in bytes. */
    virtual unsigned int getDataTypeSizeInBytes() = 0;

    /** Returns the number of channels in the voxel data. */
    virtual unsigned int getNumberOfChannels() = 0;

    /** Returns the pointer to the voxel data (Abstract). */
    virtual const void* getData() = 0;

    /** Returns the pointer to the voxel data (Abstract). */
    virtual void* getDataPointer() = 0;

    /** Returns the number of voxels in the voxel data. */
    virtual unsigned long getNumberOfVoxels() = 0;

    /** Sets the min and max voxel values in the voxel data. */
    virtual void setMinMaxValues(double minValue, double maxValue) = 0;

    /** Returns the min and max voxel values in the voxel data. */
    virtual void getMinMaxValues(double& minValue, double& maxValue) = 0;
};

}   // End of vipreDICOM namespace

#endif  // End of VIPREDICOM_VOXEL_DATA_HPP
