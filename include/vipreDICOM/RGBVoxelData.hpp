//
//  RGBVoxelData.hpp
//  vipreDICOM
//
//  Created by Christian Noon on 12/3/11.
//  Copyright (c) 2011 Christian Noon. All rights reserved.
//

#ifndef VIPREDICOM_RGB_VOXEL_DATA_HPP
#define VIPREDICOM_RGB_VOXEL_DATA_HPP

#include <osg/Object>

#include <vipreDICOM/Export.hpp>
#include <vipreDICOM/VoxelData.hpp>
#include <vipreDICOM/VoxelDataTypeTemplate.hpp>

namespace vipreDICOM {

/**
 * The RGBVoxelData class stores all triple channel (RGB) voxel data of any VoxelDataType. This allows
 * us to store voxel data abstractly regardless of type.
 */
template <class T>
class VIPREDICOM_EXPORT RGBVoxelData : public VoxelData, public VoxelDataTypeTemplate<T>
{
public:

    /** Constructor. */
    RGBVoxelData(unsigned long numberOfVoxels) :
        _numberOfVoxels(numberOfVoxels),
        _minValue(0.0f),
        _maxValue(0.0f),
        _minRedValue(0.0f),
        _maxRedValue(0.0f),
        _minBlueValue(0.0f),
        _maxBlueValue(0.0f),
        _minGreenValue(0.0f),
        _maxGreenValue(0.0f)
    {
        unsigned long numberOfVoxelsPerChannel = numberOfVoxels / 3;
        _data[0] = new T[numberOfVoxelsPerChannel];
        _data[1] = new T[numberOfVoxelsPerChannel];
        _data[2] = new T[numberOfVoxelsPerChannel];
    }

    /** Destructor. */
    ~RGBVoxelData()
    {
        delete[] _data[0];
        delete[] _data[1];
        delete[] _data[2];
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
        return 3;
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

    /** Sets the min and max voxel values for each channel in the voxel data. */
    inline void setMinMaxValuesPerChannel(double minRedValue, double maxRedValue,
                                          double minBlueValue, double maxBlueValue,
                                          double minGreenValue, double maxGreenValue)
    {
        _minRedValue = minRedValue;
        _maxRedValue = maxRedValue;
        _minBlueValue = minBlueValue;
        _maxBlueValue = maxBlueValue;
        _minGreenValue = minGreenValue;
        _maxGreenValue = maxGreenValue;
    }

    /** Returns the min and max voxel values for each channel of the voxel data. */
    inline void getMinMaxValuesPerChannel(double& minRedValue, double& maxRedValue,
                                          double& minBlueValue, double& maxBlueValue,
                                          double& minGreenValue, double& maxGreenValue)
    {
        minRedValue = _minRedValue;
        maxRedValue = _maxRedValue;
        minBlueValue = _minBlueValue;
        maxBlueValue = _maxBlueValue;
        minGreenValue = _minGreenValue;
        maxGreenValue = _maxGreenValue;
    }

protected:

    /** Instance member variables. */
    T* _data[3];
    unsigned long _numberOfVoxels;
    double _minValue;
    double _maxValue;
    double _minRedValue;
    double _maxRedValue;
    double _minBlueValue;
    double _maxBlueValue;
    double _minGreenValue;
    double _maxGreenValue;
};

}   // End of vipreDICOM namespace

#endif  // End of VIPREDICOM_RGB_VOXEL_DATA_HPP
