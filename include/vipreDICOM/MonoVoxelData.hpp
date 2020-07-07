//
//  MonoVoxelData.hpp
//  vipreDICOM
//
//  Created by Christian Noon on 12/2/11.
//  Copyright (c) 2011 Christian Noon. All rights reserved.
//

#ifndef VIPREDICOM_MONO_VOXEL_DATA_HPP
#define VIPREDICOM_MONO_VOXEL_DATA_HPP

#include <osg/Object>

#include <vipreDICOM/Export.hpp>
#include <vipreDICOM/VoxelData.hpp>
#include <vipreDICOM/VoxelDataTypeTemplate.hpp>

namespace vipreDICOM {

/**
 * The MonoVoxelData class stores all single channel voxel data of any VoxelDataType. This allows
 * us to store voxel data abstractly regardless of type.
 */
template <class T>
class VIPREDICOM_EXPORT MonoVoxelData : public VoxelData, public VoxelDataTypeTemplate<T>
{
public:

    /** Constructor. */
    MonoVoxelData(unsigned long numberOfVoxels) :
        _numberOfVoxels(numberOfVoxels),
        _minValue(0.0f),
        _maxValue(0.0f)
    {
        _data = new T[_numberOfVoxels];
    }

    /** Destructor. */
    ~MonoVoxelData()
    {
        delete[] _data;
    }

    /** Returns the voxel data type. */
    inline VoxelDataType getDataType()
    {
        return VoxelDataTypeTemplate<T>::getTemplateDataType();
    }

    /** Returns the voxel data type as a vipre::String. */
    inline vipre::String getDataTypeAsString()
    {
        return VoxelDataTypeTemplate<T>::getTemplateDataTypeAsString();
    }

    /** Returns the data type size in bytes. */
    inline unsigned int getDataTypeSizeInBytes()
    {
        return VoxelDataTypeTemplate<T>::getTemplateDataTypeSizeInBytes();
    }

    /** Returns the number of channels in the voxel data. */
    inline unsigned int getNumberOfChannels()
    {
        return 1;
    }

    /** Returns the pointer to the voxel data (Abstract). */
    inline const void* getData()
    {
        return _data;
    }

    /** Returns the pointer to the voxel data (Abstract). */
    inline void* getDataPointer()
    {
        return _data;
    }

    /** Returns the number of voxels in the voxel data. */
    inline unsigned long getNumberOfVoxels()
    {
        return _numberOfVoxels;
    }

    /** Sets the min and max voxel values in the voxel data. */
    inline void setMinMaxValues(double minValue, double maxValue)
    {
        _minValue = minValue;
        _maxValue = maxValue;
    }

    /** Returns the min and max voxels values in the voxel data. */
    inline void getMinMaxValues(double& minValue, double& maxValue)
    {
        minValue = _minValue;
        maxValue = _maxValue;
    }

protected:

    /** Instance member variables. */
    T* _data;
    unsigned long _numberOfVoxels;
    double _minValue;
    double _maxValue;
};

}   // End of vipreDICOM namespace

#endif  // End of VIPREDICOM_MONO_VOXEL_DATA_HPP
