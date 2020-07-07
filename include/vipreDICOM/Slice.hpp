//
//  Slice.hpp
//  vipreDICOM
//
//  Created by Christian Noon on 12/2/11.
//  Copyright (c) 2011 Christian Noon. All rights reserved.
//
//  Original Author - Eric Foo 4/28/10.
//

#ifndef VIPREDICOM_SLICE_HPP
#define VIPREDICOM_SLICE_HPP

#include <vipre/String.hpp>
#include <vipreDICOM/Export.hpp>
#include <vipreDICOM/Object.hpp>

// Forward declarations
class DiPixel;

namespace vipreDICOM {

/**
 * The slice class is a container for all the DICOM header info and voxel data for
 * a single DICOM image. It makes extensive use of DCMTK to do the data extraction.
 */
class VIPREDICOM_EXPORT Slice : public Object
{
public:

    /** Constructor. */
    Slice(const vipre::String& filepath);

    /** Helper method to quickly extract the series id and nothing else. */
    static bool extractSeriesID(const vipre::String& filepath, vipre::String& seriesID);

    /** Returns whether the slice is a valid DICOM object. */
    inline bool isValid() { return _isValid; }

    /** Returns the slice's location. */
    inline double getLocation() { return _location; }

    /** Returns the slice's thickness. */
    inline double getThickness() { return _thickness; }

    /** Returns the slice's filepath to the original DICOM file. */
    inline const vipre::String& getFilepath() { return _filepath; }

    /** Returns the slice's filename of the original DICOM file. */
    vipre::String getFilename();

    /**
     * Extracts all the header info from the given filepath if it is a valid DICOM file.
     *
     * @return - whether the header info was successfully extracted.
     */
    bool extractHeaderInfo();

    /**
     * Extracts all the voxel data info from the given filepath if it is a valid DICOM file.
     *
     * @return - whether the voxel data was successfully extracted.
     */
    bool extractVoxelData();

    /** Prints out all the header info and voxel data (not voxel values) for the slice. */
    virtual void printInfo();

protected:

    /** Destructor. */
    ~Slice();

    /**
     * Copies the pixel index value to the voxel index value.
     *
     * @return - the copied voxel value
     */
    double copyPixelVoxelToSliceVoxel(unsigned long pixelIndex, unsigned long sliceVoxelIndex, const DiPixel* pixelData);

    /** Instance member variables. */
    vipre::String _filepath;
    double _location;
    double _thickness;
    bool _isValid;
    bool _headerInfoExtracted;
    bool _voxelDataExtracted;
};

// Typedefs
typedef std::vector<osg::ref_ptr<vipreDICOM::Slice> > SliceList;

}   // End of vipreDICOM namespace

#endif  // End of VIPREDICOM_SLICE_HPP
