//
//  Object.hpp
//  vipreDICOM
//
//  Created by Christian Noon on 12/2/11.
//  Copyright (c) 2011 Christian Noon. All rights reserved.
//

#ifndef VIPREDICOM_OBJECT_HPP
#define VIPREDICOM_OBJECT_HPP

#include <boost/array.hpp>

#include <osg/ref_ptr>
#include <osg/Referenced>
#include <osg/Vec3d>
#include <osg/Vec3f>

#include <vipre/String.hpp>
#include <vipreDICOM/Export.hpp>
#include <vipreDICOM/MonoVoxelData.hpp>
#include <vipreDICOM/VoxelData.hpp>

namespace vipreDICOM {

/** Helper struct for voxel dimensions. */
struct Dimensions
{
    unsigned int width;
    unsigned int height;
    unsigned int depth;

    /** Default constructor. */
    Dimensions()
    {
        ;
    }

    /** Constructor containing all member variables. */
    Dimensions(unsigned int myWidth, unsigned int myHeight, unsigned int myDepth) :
        width(myWidth),
        height(myHeight),
        depth(myDepth)
    {
        ;
    }

    /** Convenience method converts struct to osg::Vec3d. */
    osg::Vec3d toVec3d() const
    {
        return osg::Vec3d(width, height, depth);
    }
};

/** Helper struct for voxel spacing. */
struct VoxelSpacing
{
    double width;
    double height;
    double depth;

    /** Default constructor. */
    VoxelSpacing()
    {
        ;
    }

    /** Constructor containing all member variables. */
    VoxelSpacing(double myWidth, double myHeight, double myDepth) :
        width(myWidth),
        height(myHeight),
        depth(myDepth)
    {
        ;
    }

    /** Convenience method converts struct to Vec3. */
    osg::Vec3d toVec3d() const
    {
        return osg::Vec3d(width, height, depth);
    }
};

/** Helper struct for image orientation patient. */
struct ImageOrientationPatient
{
    osg::Vec3f leftAxis;
    osg::Vec3f anteriorAxis;

    /** Default constructor. */
    ImageOrientationPatient()
    {
        ;
    }

    /** Constructor containing all member variables. */
    ImageOrientationPatient(osg::Vec3f myLeftAxis, osg::Vec3f myAnteriorAxis) :
        leftAxis(myLeftAxis),
        anteriorAxis(myAnteriorAxis)
    {
        ;
    }
};

/**
 * Abstract base class that acts as a common DICOM header information container. It also houses
 * the voxel data.
 */
class VIPREDICOM_EXPORT Object : public osg::Referenced
{
public:

    /** Destructor. */
    ~Object() {}

    /** Returns the object's study instance id. */
    inline const vipre::String& getStudyInstanceID() { return _studyInstanceID; }

    /** Returns the object's study id. */
    inline const vipre::String& getStudyID() { return _studyID; }

    /** Returns the object's study description. */
    inline const vipre::String& getStudyDescription() { return _studyDescription; }

    /** Returns the object's study date. */
    inline const vipre::String& getStudyDate() { return _studyDate; }

    /** Returns the object's series id. */
    inline const vipre::String& getSeriesID() { return _seriesID; }

    /** Returns the object's series number. */
    inline const vipre::String& getSeriesNumber() { return _seriesNumber; }

    /** Returns the object's series description. */
    inline const vipre::String& getSeriesDescription() { return _seriesDescription; }

    /** Returns the object's series date. */
    inline const vipre::String& getSeriesDate() { return _seriesDate; }

    /** Returns the object's patient id. */
    inline const vipre::String& getPatientID() { return _patientID; }

    /** Returns the object's patient name. */
    inline const vipre::String& getPatientName() { return _patientName; }

    /** Returns the object's patient age. */
    inline const vipre::String& getPatientAge() { return _patientAge; }

    /** Returns the object's patient date of birth. */
    inline const vipre::String& getPatientDateOfBirth() { return _patientDateOfBirth; }

    /** Returns the object's patient sex. */
    inline const vipre::String& getPatientSex() { return _patientSex; }

    /** Returns the object's patient weight. */
    inline const vipre::String& getPatientWeight() { return _patientWeight; }

    /** Returns the object's patient additional history. */
    inline const vipre::String& getPatientAdditionalHistory() { return _patientAdditionalHistory; }

    /** Returns the object's institution. */
    inline const vipre::String& getInstitution() { return _institution; }

    /** Returns the object's modality. */
    inline const vipre::String& getModality() { return _modality; }

    /** Returns the object's image type. */
    inline const vipre::String& getImageType() { return _imageType; }

    /** Returns the object's contrast type. */
    inline const vipre::String& getContrastType() { return _contrastType; }

    /** Returns the object's manufacturer. */
    inline const vipre::String& getManufacturer() { return _manufacturer; }

    /** Returns the object's image patient orientation. */
    inline const ImageOrientationPatient& getImageOrientationPatient() { return _imageOrientationPatient; }

    /** Returns the object's voxel spacing (width, height, depth). */
    inline const VoxelSpacing& getVoxelSpacing() { return _voxelSpacing; }

    /** Returns the object's dimensions (width, height, depth). */
    inline const Dimensions& getDimensions() { return _dimensions; }

    /** Sets the object's voxel data manually. */
    inline void setVoxelData(osg::ref_ptr<VoxelData> voxelData) { _voxelData = voxelData; }

    /** Returns the object's voxel data. */
    inline osg::ref_ptr<VoxelData> getVoxelData() { return _voxelData; }

    /**
     * Initializes the abstract voxel data object.
     *
     * @param voxelDataType - the data type of each voxel (i.e. UNSIGNED_INT_16)
     * @param numberOfVoxels - the total number of voxels going into the data array
     */
    inline osg::ref_ptr<VoxelData> initializeVoxelData(const VoxelDataType& voxelDataType, unsigned long numberOfVoxels)
    {
        // Instantiate a voxel data array of the voxel data type
        osg::ref_ptr<VoxelData> voxelData;
        if (voxelDataType == UNSIGNED_INT_8)
        {
            voxelData = new MonoVoxelData<unsigned char>(numberOfVoxels);
        }
        else if (voxelDataType == SIGNED_INT_8)
        {
            voxelData = new MonoVoxelData<char>(numberOfVoxels);
        }
        else if (voxelDataType == UNSIGNED_INT_16)
        {
            voxelData = new MonoVoxelData<unsigned short>(numberOfVoxels);
        }
        else if (voxelDataType == SIGNED_INT_16)
        {
            voxelData = new MonoVoxelData<short>(numberOfVoxels);
        }
        else if (voxelDataType == UNSIGNED_INT_32)
        {
            voxelData = new MonoVoxelData<unsigned int>(numberOfVoxels);
        }
        else // SIGNED_INT_32
        {
            voxelData = new MonoVoxelData<int>(numberOfVoxels);
        }

        return voxelData;
    }

    /** Releases the voxel data from memory. */
    void releaseVoxelData()
    {
        _voxelData = NULL;
    }

    /** Prints out all the information about the object. */
    virtual void printInfo() = 0;

protected:

    /** Constructor. */
    Object() :
        _studyInstanceID(),
        _studyID(),
        _studyDescription(),
        _studyDate(),
        _seriesID(),
        _seriesNumber(),
        _seriesDescription(),
        _seriesDate(),
        _patientID(),
        _patientName(),
        _patientAge(),
        _patientDateOfBirth(),
        _patientSex(),
        _patientWeight(),
        _patientAdditionalHistory(),
        _institution(),
        _modality(),
        _imageType(),
        _contrastType(),
        _manufacturer(),
        _imageOrientationPatient(),
        _voxelSpacing(),
        _dimensions(),
        _voxelData(NULL)
    {
        ;
    }

    /** Instance member variables. */
    vipre::String _studyInstanceID;
    vipre::String _studyID;
    vipre::String _studyDescription;
    vipre::String _studyDate;

    vipre::String _seriesID;
    vipre::String _seriesNumber;
    vipre::String _seriesDescription;
    vipre::String _seriesDate;

    vipre::String _patientID;
    vipre::String _patientName;
    vipre::String _patientAge;
    vipre::String _patientDateOfBirth;
    vipre::String _patientSex;
    vipre::String _patientWeight;
    vipre::String _patientAdditionalHistory;
    vipre::String _institution;

    vipre::String _modality;
    vipre::String _imageType;
    vipre::String _contrastType;
    vipre::String _manufacturer;

    ImageOrientationPatient _imageOrientationPatient;
    VoxelSpacing _voxelSpacing;
    Dimensions _dimensions;
    osg::ref_ptr<VoxelData> _voxelData;
};

}   // End of vipreDICOM namespace

#endif  // End of VIPREDICOM_OBJECT_HPP
