//
//  Series.cpp
//  vipreDICOM
//
//  Created by Christian Noon on 12/3/11.
//  Copyright (c) 2011 Christian Noon. All rights reserved.
//
//  Original Author - Eric Foo 5/20/10.
//

#include <float.h>
#include <map>

#include <osg/Math>

#include <vipreDICOM/Log.hpp>
#include <vipreDICOM/MonoVoxelData.hpp>
#include <vipreDICOM/Series.hpp>

using namespace vipreDICOM;

Series::Series(const vipre::StringList& filepaths) :
    Object(),
    _filepaths(filepaths),
    _slices(),
    _isValid(false),
    _hasBeenValidated(false),
    _voxelDataHasBeenConstructed(false),
    _voxelDataHasBeenNormalized(false),
    _originalVoxelDataType(),
    _originalMinVoxel(0.0f),
    _originalMaxVoxel(0.0f)
{
    ;
}

Series::~Series()
{
    ;
}

bool Series::isValid()
{
    return validateSeries();
}

bool Series::validateSeries()
{
    // If the series has already been validated, return the previous result
    if (_hasBeenValidated)
    {
        return _isValid;
    }

    // Cannot have a series with no data
    if (_filepaths.empty())
    {
        return false;
    }

    // Extract the header info from all slices and store only valid ones
    createSlicesFromFilepaths();
    extractHeaderInfoFromSlices();
    removeInvalidSlices();

    // Check for consistent series ids on all slices
    bool success = consistentSeriesID();
    if (!success)
    {
        vipreLogWARNING(vipreDICOMPrefix) << "WARNING: the series' slices do not have a consistent series id." << std::endl;
        return false;
    }

    // Check for consistent resolutions on all slices
    success = consistentResolution();
    if (!success)
    {
        vipreLogWARNING(vipreDICOMPrefix) << "WARNING: the series' slices do not have a consistent resolution." << std::endl;
        return false;
    }

    // Sort the slices by location and remove the duplicates
    sortSlicesByLocationAndRemoveDuplicates();

    // Check for consistent slice spacing between all slices
    success = consistentSliceSpacing();
    if (!success)
    {
        vipreLogWARNING(vipreDICOMPrefix) << "WARNING: the series' slices do not have consistent spacing between each slice." << std::endl;
        return false;
    }

    // Extract the voxel data from all slices
    extractVoxelDataFromSlices();

    // Check for consistent voxel data between all slices
    success = consistentVoxelDataType();
    if (!success)
    {
        vipreLogWARNING(vipreDICOMPrefix) << "WARNING: the series' slices do not have a consistent voxel data type." << std::endl;
        return false;
    }

    // Create the series info based on all the slice info
    initializeSeriesInfoFromSlices();

    // Since some of the filepaths have been invalidated, match them up to the valid slice filepaths
    resetFilepathsToMatchSlices();

    // Update the is valid and validation checked flags
    _isValid = true;
    _hasBeenValidated = true;

    return success;
}

void Series::createSlicesFromFilepaths()
{
    vipre::StringList::iterator iter = _filepaths.begin();
    while (iter != _filepaths.end())
    {
        osg::ref_ptr<Slice> slice = new Slice(*iter);
        _slices.push_back(slice);
        ++iter;
    }
}

void Series::extractHeaderInfoFromSlices()
{
    SliceList::iterator iter = _slices.begin();
    while (iter != _slices.end())
    {
        (*iter)->extractHeaderInfo();
        ++iter;
    }
}

void Series::removeInvalidSlices()
{
    // Create a list of only valid slices
    SliceList validSlices;
    SliceList::iterator iter = _slices.begin();
    while (iter != _slices.end())
    {
        if ((*iter)->isValid())
        {
            validSlices.push_back(*iter);
        }

        ++iter;
    }

    // Set our slice list to the valid slices
    _slices = validSlices;
}

bool Series::consistentSeriesID()
{
    // No point in checking for consistency if there is only a single slice
    if (_slices.size() < 2)
    {
        return true;
    }

    // Check for consistent series ids for each slice
    const vipre::String& seriesID = _slices.at(0)->getSeriesID();
    for (unsigned int i = 1; i < _slices.size(); ++i)
    {
        if (seriesID != _slices.at(i)->getSeriesID())
        {
            return false;
        }
    }

    return true;
}

bool Series::consistentResolution()
{
    // No point in checking for consistency if there is only a single slice
    if (_slices.size() < 2)
    {
        return true;
    }

    // Check for consistent resolutions for each slice
    const Dimensions& seriesDimensions = _slices.at(0)->getDimensions();
    for (unsigned int i = 1; i < _slices.size(); ++i)
    {
        const Dimensions& sliceDimensions = _slices.at(i)->getDimensions();
        if (sliceDimensions.toVec3d() != seriesDimensions.toVec3d())
        {
            return false;
        }
    }

    return true;
}

void Series::sortSlicesByLocationAndRemoveDuplicates()
{
    // Use a multimap to sort all the slices by location in ascending order
    std::map<double, osg::ref_ptr<Slice> > sliceMap;
    SliceList::iterator listiter = _slices.begin();
    while (listiter != _slices.end())
    {
        sliceMap.insert(std::pair<double, osg::ref_ptr<Slice> >((*listiter)->getLocation(), *listiter));
        ++listiter;
    }

    // Create a sorted list of slices
    SliceList sortedSlices;
    std::map<double, osg::ref_ptr<Slice> >::iterator mapiter = sliceMap.begin();
    while (mapiter != sliceMap.end())
    {
        sortedSlices.push_back(mapiter->second);
        ++mapiter;
    }

    // Set our slice list to the sorted slices
    _slices = sortedSlices;
}

bool Series::consistentSliceSpacing()
{
    // If we only have one or two slices, then the spacing is obviously consistent
    if (_slices.size() < 3)
    {
        return true;
    }

    // Check for consistent spacing between each slice
    double localSpacing;
    double seriesSpacing = fabs(_slices.at(1)->getLocation() - _slices.at(0)->getLocation());
    for (unsigned int i = 1; i < _slices.size() - 1; ++i)
    {
        localSpacing = fabs(_slices.at(i+1)->getLocation() - _slices.at(i)->getLocation());
        if (localSpacing != seriesSpacing)
        {
            return false;
        }
    }

    return true;
}

void Series::extractVoxelDataFromSlices()
{
    SliceList::iterator iter = _slices.begin();
    while (iter != _slices.end())
    {
        (*iter)->extractVoxelData();
        ++iter;
    }
}

bool Series::consistentVoxelDataType()
{
    // No point in checking for consistency if there is only a single slice
    if (_slices.size() < 2)
    {
        return true;
    }

    // Check for consistent voxel data types for each slice
    const VoxelDataType& seriesDateType = _slices.at(0)->getVoxelData()->getDataType();
    for (unsigned int i = 1; i < _slices.size(); ++i)
    {
        const VoxelDataType& sliceDateType = _slices.at(i)->getVoxelData()->getDataType();
        if (seriesDateType != sliceDateType)
        {
            return false;
        }
    }

    return true;
}

void Series::initializeSeriesInfoFromSlices()
{
    // Set all the series object properties
    osg::ref_ptr<Slice> slice = _slices.at(0);
    _studyInstanceID = slice->getStudyInstanceID();
    _studyID = slice->getStudyID();
    _studyDescription = slice->getStudyDescription();
    _studyDate = slice->getStudyDate();
    _seriesID = slice->getSeriesID();
    _seriesNumber = slice->getSeriesNumber();
    _seriesDescription = slice->getSeriesDescription();
    _seriesDate = slice->getSeriesDate();
    _patientID = slice->getPatientID();
    _patientName = slice->getPatientName();
    _patientAge = slice->getPatientAge();
    _patientDateOfBirth = slice->getPatientDateOfBirth();
    _patientSex = slice->getPatientSex();
    _patientWeight = slice->getPatientWeight();
    _patientAdditionalHistory = slice->getPatientAdditionalHistory();
    _institution = slice->getInstitution();
    _modality = slice->getModality();
    _imageType = slice->getImageType();
    _contrastType = slice->getContrastType();
    _manufacturer = slice->getManufacturer();
    _imageOrientationPatient = slice->getImageOrientationPatient();

    // Set the series dimensions
    const Dimensions& dimensions = _slices.at(0)->getDimensions();
    _dimensions.width = dimensions.width;
    _dimensions.height = dimensions.height;
    _dimensions.depth = _slices.size();

    // Set the series voxel spacing
    const VoxelSpacing& voxelSpacing = _slices.at(0)->getVoxelSpacing();
    _voxelSpacing.width = voxelSpacing.width;
    _voxelSpacing.height = voxelSpacing.height;
    _voxelSpacing.depth = 0.0f;
    if (_slices.size() > 1)
    {
        _voxelSpacing.depth = fabs(_slices.at(1)->getLocation() - _slices.at(0)->getLocation());
    }
}

void Series::resetFilepathsToMatchSlices()
{
    vipre::StringList filepaths;
    SliceList::iterator iter = _slices.begin();
    while (iter != _slices.end())
    {
        filepaths.push_back((*iter)->getFilepath());
        ++iter;
    }
    _filepaths = filepaths;
}

void Series::buildVoxelData(bool releaseSliceVoxelData)
{
    // Cannot build the series voxel data without any slices
    if (_slices.empty())
    {
        vipreLogWARNING(vipreDICOMPrefix) << "WARNING: cannot build voxel data without any valid slices." << std::endl;
        return;
    }

    vipreLogINFO(vipreDICOMPrefix) << "Starting to extract series voxel data: " << _seriesID << std::endl;

    // Instantiate the voxel data subclass
    const VoxelDataType& voxelDataType = _slices.at(0)->getVoxelData()->getDataType();
    _voxelData = initializeVoxelData(voxelDataType, _dimensions.width * _dimensions.height * _dimensions.depth);

    // Set the minHU and maxHU equal to the min and max limits of an int
    double minHU = DBL_MAX;
    double maxHU = DBL_MIN;

    // Copy all the slice voxel data into the series voxel data
    unsigned long sliceIndex;
    unsigned long seriesIndex;
    double voxelValue;
    for (unsigned int i = 0; i < _slices.size(); ++i)
    {
        vipreLogINFO(vipreDICOMPrefix) << "Extracting slice: " << _slices.at(i)->getFilepath() << "\tfor Series: " << _seriesID << std::endl;

        osg::ref_ptr<VoxelData> sliceVoxelData = _slices.at(i)->getVoxelData();
        unsigned long sliceNumberOfVoxels = sliceVoxelData->getNumberOfVoxels();
        for (unsigned long j = 0; j < sliceNumberOfVoxels; ++j)
        {
            // Compute the slice and series indices
            sliceIndex = j;
            seriesIndex = i * sliceNumberOfVoxels + j;

            // Copy over the slice voxel value to the series
            voxelValue = copySliceVoxelToSeriesVoxel(sliceIndex, seriesIndex, sliceVoxelData);

            // Update the min and max HU values
            if (voxelValue < minHU)
            {
                minHU = voxelValue;
            }
            if (voxelValue > maxHU)
            {
                maxHU = voxelValue;
            }
        }

        // Release the slice voxel data if specified
        if (releaseSliceVoxelData)
        {
            _slices.at(i)->releaseVoxelData();
        }
    }

    // Set the min and max voxel values
    _voxelData->setMinMaxValues(minHU, maxHU);

    // Update the construction flag
    _voxelDataHasBeenConstructed = true;
}

void Series::buildNormalizedVoxelData(const NormalizedVoxelDataType& voxelDataType, bool normalizeSliceData, bool releaseSliceVoxelData)
{
    // Cannot build the series voxel data without any slices
    if (_slices.empty())
    {
        vipreLogWARNING(vipreDICOMPrefix) << "WARNING: cannot build voxel data without any valid slices." << std::endl;
        return;
    }

    vipreLogINFO(vipreDICOMPrefix) << "Starting to extract series voxel data: " << _seriesID << std::endl;

    // Figure out what the series voxel data type will be
    VoxelDataType seriesVoxelDataType;
    const VoxelDataType& sliceVoxelDataType = _slices.at(0)->getVoxelData()->getDataType();
    if (voxelDataType == NORMALIZED_UNSIGNED_VERSION_OF_CURRENT_DATA_TYPE)
    {
        if (sliceVoxelDataType == UNSIGNED_INT_8 || sliceVoxelDataType == SIGNED_INT_8)
        {
            seriesVoxelDataType = UNSIGNED_INT_8;
        }
        else if (sliceVoxelDataType == UNSIGNED_INT_16 || sliceVoxelDataType == SIGNED_INT_16)
        {
            seriesVoxelDataType = UNSIGNED_INT_16;
        }
        else
        {
            seriesVoxelDataType = UNSIGNED_INT_32;
        }
    }
    else if (voxelDataType == NORMALIZED_UNSIGNED_INT_8)
    {
        seriesVoxelDataType = UNSIGNED_INT_8;
    }
    else if (voxelDataType == NORMALIZED_UNSIGNED_INT_16)
    {
        seriesVoxelDataType = UNSIGNED_INT_16;
    }
    else // NORMALIZED_UNSIGNED_INT_32
    {
        seriesVoxelDataType = UNSIGNED_INT_32;
    }

    // Instantiate the voxel data subclass
    _voxelData = initializeVoxelData(seriesVoxelDataType, _dimensions.width * _dimensions.height * _dimensions.depth);

    // Compute the min and max HU values from all the slices
    double seriesMinHU = DBL_MAX;
    double seriesMaxHU = DBL_MIN;
    double sliceMinHU;
    double sliceMaxHU;
    for (unsigned int i = 0; i < _slices.size(); ++i)
    {
        _slices.at(i)->getVoxelData()->getMinMaxValues(sliceMinHU, sliceMaxHU);
        if (sliceMinHU < seriesMinHU)
        {
            seriesMinHU = sliceMinHU;
        }
        if (sliceMaxHU > seriesMaxHU)
        {
            seriesMaxHU = sliceMaxHU;
        }
    }

    // Copy all the normalized slice voxel data into the series voxel data
    unsigned long sliceIndex;
    unsigned long seriesIndex;
    for (unsigned int i = 0; i < _slices.size(); ++i)
    {
        vipreLogINFO(vipreDICOMPrefix) << "Normalizing slice: " << _slices.at(i)->getFilepath() << "\tfor Series: " << _seriesID << std::endl;

        // Grab the voxel data info from the current slice
        osg::ref_ptr<VoxelData> sliceVoxelData = _slices.at(i)->getVoxelData();
        unsigned long sliceNumberOfVoxels = sliceVoxelData->getNumberOfVoxels();

        // Create a new voxel data array for the normalized slice data
        osg::ref_ptr<VoxelData> normalizedSliceVoxelData = initializeVoxelData(seriesVoxelDataType, sliceNumberOfVoxels);

        // Iterate through all the original slice voxel data and normalize it. Once it is normalized,
        // place it in the series voxel array as well as the new normalized slice data data array.
        for (unsigned long j = 0; j < sliceNumberOfVoxels; ++j)
        {
            // Compute the slice and series indices
            sliceIndex = j;
            seriesIndex = i * sliceNumberOfVoxels + j;

            // Extract the original slice voxel, normalize it, then set the normalized value to
            // the series voxel data index as well as the new normalized slice voxel data array
            copyNormalizedVoxelToSeriesAndNormalizedSlice(sliceIndex,
                                                          seriesIndex,
                                                          sliceVoxelData,
                                                          normalizedSliceVoxelData,
                                                          _voxelData,
                                                          seriesMinHU,
                                                          seriesMaxHU,
                                                          seriesVoxelDataType);
        }

        // Release the slice voxel data if specified
        if (releaseSliceVoxelData)
        {
            _slices.at(i)->releaseVoxelData();
        }
        else // Delete the original slice voxel data and replace with the normalized slice voxel data
        {
            _slices.at(i)->releaseVoxelData();
            _slices.at(i)->setVoxelData(normalizedSliceVoxelData);
        }
    }

    // Set the min and max voxel values based on the series voxel data type
    if (seriesVoxelDataType == UNSIGNED_INT_8)
    {
        _voxelData->setMinMaxValues(0.0f, MAX_UNSIGNED_INT_8);
    }
    else if (seriesVoxelDataType == UNSIGNED_INT_16)
    {
        _voxelData->setMinMaxValues(0.0f, MAX_UNSIGNED_INT_16);
    }
    else // UNSIGNED_INT_32
    {
        _voxelData->setMinMaxValues(0.0f, MAX_UNSIGNED_INT_32);
    }

    // Update normalization state ivars
    _voxelDataHasBeenNormalized = true;
    _voxelDataHasBeenConstructed = true;
    _originalVoxelDataType = sliceVoxelDataType;
    _originalMinVoxel = seriesMinHU;
    _originalMaxVoxel = seriesMaxHU;
}

void Series::printInfo()
{
    vipreLogALWAYS(vipreDICOMPrefix) << "vipreDICOM::Series Info: " << _seriesID << std::endl;

    if (!_hasBeenValidated)
    {
        vipreLogALWAYS(vipreDICOMPrefix) << "\tWARNING: cannot print series info because it has not been validated, "
            "please see the validateSeries() method." << std::endl;
    }
    else if (!_isValid)
    {
        vipreLogALWAYS(vipreDICOMPrefix) << "\tWARNING: cannot print series info because it is not a valid series." << std::endl;
    }
    else // valid series, dump info
    {
        // Print header info
        vipreLogALWAYS(vipreDICOMPrefix) << "\tHeader Info:" << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(R) Study Instance ID:                    " << _studyInstanceID << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(R) Study ID:                             " << _studyID << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(N) Study Description:                    " << _studyDescription << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(N) Study Date:                           " << _studyDate << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(R) Series Instance ID:                   " << _seriesID << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(R) Series Number:                        " << _seriesNumber << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(N) Series Description:                   " << _seriesDescription << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(N) Series Date:                          " << _seriesDate << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(N) Patient ID:                           " << _patientID << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(N) Patient Name:                         " << _patientName << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(N) Patient Age:                          " << _patientAge << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(N) Patient Date of Birth:                " << _patientDateOfBirth << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(N) Patient Sex:                          " << _patientSex << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(N) Patient Weight:                       " << _patientWeight << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(N) Patient Additional History:           " << _patientAdditionalHistory << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(N) Institution Name:                     " << _institution << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(N) Modality:                             " << _modality << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(N) Image Type:                           " << _imageType << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(N) Contrast:                             " << _contrastType << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(N) Manufacturer:                         " << _manufacturer << std::endl;
        osg::Vec3f leftAxis = _imageOrientationPatient.leftAxis;
        osg::Vec3f anteriorAxis = _imageOrientationPatient.anteriorAxis;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(R) Image Orientation Patient:            ("
            << leftAxis[0] << ", " << leftAxis[1] << ", " << leftAxis[2] << ") ("
            << anteriorAxis[0] << ", " << anteriorAxis[1] << ", " << anteriorAxis[2] << ")" << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(R) Voxel Spacing (width, height, depth): ("
            << _voxelSpacing.width << ", " << _voxelSpacing.height << ", " << _voxelSpacing.depth << ")" << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(R) Dimensions (width, height)            ("
            << _dimensions.width << ", " << _dimensions.height << ", " << _dimensions.depth << ")" << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(R) Number of Slices:                     " << _slices.size() << std::endl;

        // Print voxel info
        if (_voxelDataHasBeenConstructed)
        {
            vipreLogALWAYS(vipreDICOMPrefix) << "\tVoxel Info:" << std::endl;
            if (_voxelDataHasBeenNormalized)
            {
                vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(R) Original Data Type:                   " << convertVoxelDataTypeToString(_originalVoxelDataType) << std::endl;
                vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(R) Original Min Voxel Value:             " << _originalMinVoxel << std::endl;
                vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(R) Original Max Voxel Value:             " << _originalMaxVoxel << std::endl;
                vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(R) Normalized Data Type:                 " << _voxelData->getDataTypeAsString() << std::endl;
                vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(R) Normalized Data Type Size in Bytes:   " << _voxelData->getDataTypeSizeInBytes() << std::endl;
            }
            else
            {
                vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(R) Data Type:                            " << _voxelData->getDataTypeAsString() << std::endl;
                vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(R) Data Type Size in Bytes:              " << _voxelData->getDataTypeSizeInBytes() << std::endl;
            }
            vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(R) Number of Channels:                   " << _voxelData->getNumberOfChannels() << std::endl;
            vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(R) Number of Voxels:                     " << _voxelData->getNumberOfVoxels() << std::endl;
            double minValue, maxValue;
            _voxelData->getMinMaxValues(minValue, maxValue);
            vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(R) Min Voxel Value:                      " << minValue << std::endl;
            vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(R) Max Voxel Value:                      " << maxValue << std::endl;
        }
    }
}

double Series::copySliceVoxelToSeriesVoxel(unsigned long sliceIndex, unsigned long seriesIndex, osg::ref_ptr<VoxelData> sliceVoxelData)
{
    double voxelValue = 0.0f;
    const VoxelDataType& voxelDataType = sliceVoxelData->getDataType();
    if (voxelDataType == UNSIGNED_INT_8)
    {
        const unsigned char* sliceVoxels = static_cast<const unsigned char *>(sliceVoxelData->getDataPointer());
        unsigned char* seriesVoxels = static_cast<unsigned char *>(_voxelData->getDataPointer());
        voxelValue = sliceVoxels[sliceIndex];
        seriesVoxels[seriesIndex] = voxelValue;
    }
    else if (voxelDataType == SIGNED_INT_8)
    {
        const char* sliceVoxels = static_cast<const char *>(sliceVoxelData->getDataPointer());
        char* seriesVoxels = static_cast<char *>(_voxelData->getDataPointer());
        voxelValue = sliceVoxels[sliceIndex];
        seriesVoxels[seriesIndex] = voxelValue;
    }
    else if (voxelDataType == UNSIGNED_INT_16)
    {
        const unsigned short* sliceVoxels = static_cast<const unsigned short *>(sliceVoxelData->getDataPointer());
        unsigned short* seriesVoxels = static_cast<unsigned short *>(_voxelData->getDataPointer());
        voxelValue = sliceVoxels[sliceIndex];
        seriesVoxels[seriesIndex] = voxelValue;
    }
    else if (voxelDataType == SIGNED_INT_16)
    {
        const short* sliceVoxels = static_cast<const short *>(sliceVoxelData->getDataPointer());
        short* seriesVoxels = static_cast<short *>(_voxelData->getDataPointer());
        voxelValue = sliceVoxels[sliceIndex];
        seriesVoxels[seriesIndex] = voxelValue;
    }
    else if (voxelDataType == UNSIGNED_INT_32)
    {
        const unsigned int* sliceVoxels = static_cast<const unsigned int *>(sliceVoxelData->getDataPointer());
        unsigned int* seriesVoxels = static_cast<unsigned int *>(_voxelData->getDataPointer());
        voxelValue = sliceVoxels[sliceIndex];
        seriesVoxels[seriesIndex] = voxelValue;
    }
    else // SIGNED_INT_32
    {
        const int* sliceVoxels = static_cast<const int *>(sliceVoxelData->getDataPointer());
        int* seriesVoxels = static_cast<int *>(_voxelData->getDataPointer());
        voxelValue = sliceVoxels[sliceIndex];
        seriesVoxels[seriesIndex] = voxelValue;
    }

    return voxelValue;
}

void Series::copyNormalizedVoxelToSeriesAndNormalizedSlice(unsigned long sliceIndex,
                                                           unsigned long seriesIndex,
                                                           osg::ref_ptr<VoxelData> originalSliceVoxelData,
                                                           osg::ref_ptr<VoxelData> normalizedSliceVoxelData,
                                                           osg::ref_ptr<VoxelData> normalizedSeriesVoxelData,
                                                           double minValue,
                                                           double maxValue,
                                                           const vipreDICOM::VoxelDataType& normalizedDataType)
{
    // Compute the normalized voxel
    double normalizedVoxel;
    const VoxelDataType& sliceVoxelDataType = originalSliceVoxelData->getDataType();
    if (sliceVoxelDataType == UNSIGNED_INT_8)
    {
        unsigned char* originalSliceVoxels = static_cast<unsigned char *>(originalSliceVoxelData->getDataPointer());
        double originalSliceVoxel = originalSliceVoxels[sliceIndex];
        normalizedVoxel = osg::round(normalizeValue(originalSliceVoxel, minValue, maxValue, normalizedDataType));
    }
    else if (sliceVoxelDataType == SIGNED_INT_8)
    {
        char* originalSliceVoxels = static_cast<char *>(originalSliceVoxelData->getDataPointer());
        double originalSliceVoxel = originalSliceVoxels[sliceIndex];
        normalizedVoxel = osg::round(normalizeValue(originalSliceVoxel, minValue, maxValue, normalizedDataType));
    }
    else if (sliceVoxelDataType == UNSIGNED_INT_16)
    {
        unsigned short* originalSliceVoxels = static_cast<unsigned short *>(originalSliceVoxelData->getDataPointer());
        double originalSliceVoxel = originalSliceVoxels[sliceIndex];
        normalizedVoxel = osg::round(normalizeValue(originalSliceVoxel, minValue, maxValue, normalizedDataType));
    }
    else if (sliceVoxelDataType == SIGNED_INT_16)
    {
        short* originalSliceVoxels = static_cast<short *>(originalSliceVoxelData->getDataPointer());
        double originalSliceVoxel = originalSliceVoxels[sliceIndex];
        normalizedVoxel = osg::round(normalizeValue(originalSliceVoxel, minValue, maxValue, normalizedDataType));
    }
    else if (sliceVoxelDataType == UNSIGNED_INT_32)
    {
        unsigned int* originalSliceVoxels = static_cast<unsigned int *>(originalSliceVoxelData->getDataPointer());
        double originalSliceVoxel = originalSliceVoxels[sliceIndex];
        normalizedVoxel = osg::round(normalizeValue(originalSliceVoxel, minValue, maxValue, normalizedDataType));
    }
    else // SIGNED_INT_32
    {
        int* originalSliceVoxels = static_cast<int *>(originalSliceVoxelData->getDataPointer());
        double originalSliceVoxel = originalSliceVoxels[sliceIndex];
        normalizedVoxel = osg::round(normalizeValue(originalSliceVoxel, minValue, maxValue, normalizedDataType));
    }

    // Copy the normalized voxel into the series voxel data array and the normalized slice voxel data array
    if (normalizedDataType == UNSIGNED_INT_8)
    {
        unsigned char* seriesVoxels = static_cast<unsigned char *>(_voxelData->getDataPointer());
        seriesVoxels[seriesIndex] = normalizedVoxel;
        unsigned char* normalizedSliceVoxels = static_cast<unsigned char *>(normalizedSliceVoxelData->getDataPointer());
        normalizedSliceVoxels[sliceIndex] = normalizedVoxel;
    }
    else if (normalizedDataType == SIGNED_INT_8)
    {
        char* seriesVoxels = static_cast<char *>(_voxelData->getDataPointer());
        seriesVoxels[seriesIndex] = normalizedVoxel;
        char* normalizedSliceVoxels = static_cast<char *>(normalizedSliceVoxelData->getDataPointer());
        normalizedSliceVoxels[sliceIndex] = normalizedVoxel;
    }
    else if (normalizedDataType == UNSIGNED_INT_16)
    {
        unsigned short* seriesVoxels = static_cast<unsigned short *>(_voxelData->getDataPointer());
        seriesVoxels[seriesIndex] = normalizedVoxel;
        unsigned short* normalizedSliceVoxels = static_cast<unsigned short *>(normalizedSliceVoxelData->getDataPointer());
        normalizedSliceVoxels[sliceIndex] = normalizedVoxel;
    }
    else if (normalizedDataType == SIGNED_INT_16)
    {
        short* seriesVoxels = static_cast<short *>(_voxelData->getDataPointer());
        seriesVoxels[seriesIndex] = normalizedVoxel;
        short* normalizedSliceVoxels = static_cast<short *>(normalizedSliceVoxelData->getDataPointer());
        normalizedSliceVoxels[sliceIndex] = normalizedVoxel;
    }
    else if (normalizedDataType == UNSIGNED_INT_32)
    {
        unsigned int* seriesVoxels = static_cast<unsigned int *>(_voxelData->getDataPointer());
        seriesVoxels[seriesIndex] = normalizedVoxel;
        unsigned int* normalizedSliceVoxels = static_cast<unsigned int *>(normalizedSliceVoxelData->getDataPointer());
        normalizedSliceVoxels[sliceIndex] = normalizedVoxel;
    }
    else // SIGNED_INT_32
    {
        int* seriesVoxels = static_cast<int *>(_voxelData->getDataPointer());
        seriesVoxels[seriesIndex] = normalizedVoxel;
        int* normalizedSliceVoxels = static_cast<int *>(normalizedSliceVoxelData->getDataPointer());
        normalizedSliceVoxels[sliceIndex] = normalizedVoxel;
    }
}

double Series::normalizeValue(double valueToNormalize, double minValue, double maxValue, const VoxelDataType& normalizedDataType)
{
    double range = maxValue - minValue;
    double normalizedValue = fabs(valueToNormalize - minValue) / range;

    switch (normalizedDataType)
    {
        case (UNSIGNED_INT_8):
            normalizedValue = normalizedValue * MAX_UNSIGNED_INT_8;
            break;
        case (SIGNED_INT_8):
            normalizedValue = MIN_SIGNED_INT_8 + (normalizedValue * MAX_UNSIGNED_INT_8);
            break;
        case (UNSIGNED_INT_16):
            normalizedValue = normalizedValue * MAX_UNSIGNED_INT_16;
            break;
        case (SIGNED_INT_16):
            normalizedValue = MIN_SIGNED_INT_16 + (normalizedValue * MAX_UNSIGNED_INT_16);
            break;
        case (UNSIGNED_INT_32):
            normalizedValue = normalizedValue * MAX_UNSIGNED_INT_32;
            break;
        case (SIGNED_INT_32):
            normalizedValue = MIN_SIGNED_INT_32 + (normalizedValue * MAX_UNSIGNED_INT_32);
            break;
        default:
            break;
    }

    return normalizedValue;
}

vipre::String Series::convertVoxelDataTypeToString(const VoxelDataType& voxelDataType)
{
    if (voxelDataType == UNSIGNED_INT_8)
    {
        return "UNSIGNED_INT_8";
    }
    else if (voxelDataType == SIGNED_INT_8)
    {
        return "SIGNED_INT_8";
    }
    else if (voxelDataType == UNSIGNED_INT_16)
    {
        return "UNSIGNED_INT_16";
    }
    else if (voxelDataType == SIGNED_INT_16)
    {
        return "SIGNED_INT_16";
    }
    else if (voxelDataType == UNSIGNED_INT_32)
    {
        return "UNSIGNED_INT_32";
    }
    else // SIGNED_INT_32
    {
        return "SIGNED_INT_32";
    }
}
