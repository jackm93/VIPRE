//
//  Series.hpp
//  vipreDICOM
//
//  Created by Christian Noon on 12/3/11.
//  Copyright (c) 2011 Christian Noon. All rights reserved.
//
//  Original Author - Eric Foo 5/20/10.
//

#ifndef VIPREDICOM_SERIES_HPP
#define VIPREDICOM_SERIES_HPP

#define MAX_UNSIGNED_INT_8      255.0f
#define MAX_UNSIGNED_INT_16     65535.0f
#define MAX_UNSIGNED_INT_32     4294967295.0f
#define MIN_SIGNED_INT_8        -128.0f
#define MIN_SIGNED_INT_16       -32768.0f
#define MIN_SIGNED_INT_32       -2147483648.0

#include <vipreDICOM/Export.hpp>
#include <vipreDICOM/Object.hpp>
#include <vipreDICOM/Slice.hpp>

namespace vipreDICOM {

/**
 * The series class is a container for all slice header info and voxel data for a set of
 * equally spaced slices. It uses a series ( no pun intended :P ) of checks to ensure the
 * voxel data is valid.
 */
class VIPREDICOM_EXPORT Series : public Object
{
public:

    /** The series class is capable of normalizing all the slice data to the following data types. */
    enum NormalizedVoxelDataType
    {
        NORMALIZED_UNSIGNED_VERSION_OF_CURRENT_DATA_TYPE,
        NORMALIZED_UNSIGNED_INT_8,
        NORMALIZED_UNSIGNED_INT_16,
        NORMALIZED_UNSIGNED_INT_32
    };

    /** Constructor. */
    Series(const vipre::StringList& filepaths);

    /** Sets the series id. */
    inline void setSeriesID(const vipre::String& seriesID) { _seriesID = seriesID; }

    /** Returns the filepaths of all the DICOM files. */
    inline const vipre::StringList& getFilepaths() { return _filepaths; }

    /** Returns the slices associated with the series. */
    inline const SliceList& getSlices() { return _slices; }

    /** Returns whether the series is valid by validating the series if it has not already been. */
    bool isValid();

    /** Returns whether the series voxel data has been constructed. */
    inline bool voxelDataHasBeenConstructed() { return _voxelDataHasBeenConstructed; }

    /**
     * Returns whether the series is valid by undergoing a series of consistency checks.
     *
     * Here is a list of all the consistency checks involved:
     *
     * - consistentSeriesID
     * - consistentResolution
     * - consistentSliceSpacing
     * - consistentVoxelDataType
     *
     */
    bool validateSeries();

    /**
     * Builds the series voxel data as a 1D array from all the slices.
     *
     * @param releaseSliceVoxelData - releases slice voxel data as soon as possible to minimize the memory footprint.
     */
    void buildVoxelData(bool releaseSliceVoxelData);

    /**
     * Builds the series voxel data as a normalized 1D array from all the slices with the specified data type.
     *
     * @param voxelDataType - the data type to normalize to
     * @param normalizeSliceData - normalizes the original slice data in addition to the series data
     * @param releaseSliceVoxelData - releases slice voxel data as soon as possible to minimize the memory footprint
     */
    void buildNormalizedVoxelData(const NormalizedVoxelDataType& voxelDataType, bool normalizeSliceData, bool releaseSliceVoxelData);

    /** Prints out all the information about the object. */
    void printInfo();

protected:

    /** Destructor. */
    ~Series();

    /** Instantiates a new slice for each filepath. */
    void createSlicesFromFilepaths();

    /** Extracts all the header info from each slice. */
    void extractHeaderInfoFromSlices();

    /** Removes all invalid slices. */
    void removeInvalidSlices();

    /** Returns whether all slices have the same series id. */
    bool consistentSeriesID();

    /** Returns whether all slices have the same resolution. */
    bool consistentResolution();

    /** Sorts the slices in ascending order by location and removes all duplicates. */
    void sortSlicesByLocationAndRemoveDuplicates();

    /** Returns whether all slices have consistent spacing based on their location. */
    bool consistentSliceSpacing();

    /** Extracts all the voxel data from each slice. */
    void extractVoxelDataFromSlices();

    /** Returns whether all slices have consistent voxel data types. */
    bool consistentVoxelDataType();

    /** Copies over all slice header info. */
    void initializeSeriesInfoFromSlices();

    /** Resets the filepaths to match the valid slices. */
    void resetFilepathsToMatchSlices();

    /** Copies the voxel from the slice index into the series index. */
    double copySliceVoxelToSeriesVoxel(unsigned long sliceIndex, unsigned long seriesIndex, osg::ref_ptr<VoxelData> sliceVoxelData);

    /** Copies the normalized voxel from the original slice to the series and normalized slice voxel data arrays. */
    void copyNormalizedVoxelToSeriesAndNormalizedSlice(unsigned long sliceIndex,
                                                       unsigned long seriesIndex,
                                                       osg::ref_ptr<VoxelData> originalSliceVoxelData,
                                                       osg::ref_ptr<VoxelData> normalizedSliceVoxelData,
                                                       osg::ref_ptr<VoxelData> normalizedSeriesVoxelData,
                                                       double minValue,
                                                       double maxValue,
                                                       const vipreDICOM::VoxelDataType& normalizedDataType);

    /** Returns a normalized version of the variable valueToNormalize between min and max for the given data type. */
    double normalizeValue(double valueToNormalize, double minValue, double maxValue, const vipreDICOM::VoxelDataType& normalizedDataType);

    /** Returns the string version of the voxel data type. */
    vipre::String convertVoxelDataTypeToString(const VoxelDataType& voxelDataType);

    /** Instance member variables. */
    vipre::StringList _filepaths;
    vipreDICOM::SliceList _slices;
    bool _isValid;
    bool _hasBeenValidated;
    bool _voxelDataHasBeenConstructed;
    bool _voxelDataHasBeenNormalized;
    vipreDICOM::VoxelDataType _originalVoxelDataType;
    double _originalMinVoxel;
    double _originalMaxVoxel;
};

// Typedefs
typedef std::vector<osg::ref_ptr<vipreDICOM::Series> > SeriesList;

}   // End of vipreDICOM namespace

#endif  // End of VIPREDICOM_SERIES_HPP
