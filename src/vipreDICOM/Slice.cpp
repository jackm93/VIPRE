//
//  Slice.cpp
//  vipreDICOM
//
//  Created by Christian Noon on 12/2/11.
//  Copyright (c) 2011 Christian Noon. All rights reserved.
//
//  Original Author - Eric Foo 4/28/10.
//

#include <float.h>

// Necessary for all non-windows builds
#ifndef _MSC_VER
    #define HAVE_CONFIG_H
#endif

#include <dcmtk/dcmdata/dctk.h>
#include <dcmtk/dcmimgle/dcmimage.h>

#include <osg/Math>
#include <osgDB/FileNameUtils>

#include <vipreDICOM/Log.hpp>
#include <vipreDICOM/Slice.hpp>

using namespace vipreDICOM;

Slice::Slice(const vipre::String& filepath) :
    Object(),
    _filepath(filepath),
    _location(0.0f),
    _thickness(0.0f),
    _isValid(false),
    _headerInfoExtracted(false),
    _voxelDataExtracted(false)
{
    ;
}

Slice::~Slice()
{
    ;
}

bool Slice::extractSeriesID(const vipre::String& filepath, vipre::String& seriesID)
{
    // Load the DICOM file
    DcmFileFormat dcmFormat;
    OFCondition loadStatus = dcmFormat.loadFile(filepath.c_str());
    if (loadStatus.bad())
    {
        vipreLogWARNING(vipreDICOMPrefix) << "WARNING: Could not load the DICOM file: " << filepath << std::endl;
        return false;
    }

    // Extract the series id
    DcmDataset* imgDataset = dcmFormat.getDataset();
    OFString temp_string;
    if (imgDataset->findAndGetOFString(DCM_SeriesInstanceUID, temp_string).bad())
    {
        vipreLogWARNING(vipreDICOMPrefix) << "WARNING: the series id could not be extracted from the DICOM file: " << filepath << std::endl;
        return false;
    }
    seriesID = temp_string.c_str();

    return true;
}

vipre::String Slice::getFilename()
{
    return osgDB::getSimpleFileName(_filepath);
}

bool Slice::extractHeaderInfo()
{
    // Make sure the isValid flag is false before we start
    _isValid = false;

    // Set up some temp variables for data extraction
    OFString temp_string;
    Float64 temp_float;
    Uint16 temp_int;

    //=============================================================================================
    // Load the DICOM file
    DcmFileFormat dcmFormat;
    OFCondition loadStatus = dcmFormat.loadFile(_filepath.c_str());
    if (loadStatus.bad())
    {
        vipreLogWARNING(vipreDICOMPrefix) << "WARNING: Could not load the DICOM file: " << _filepath << std::endl;
        return false;
    }

    //=============================================================================================
    // Ignore datasets with more than a single channel of pixel data
    DicomImage dImage(&dcmFormat, dcmFormat.getDataset()->getOriginalXfer());
    const DiPixel* pixelData = dImage.getInterData();
    if (pixelData->getPlanes() > 1)
    {
        vipreLogWARNING(vipreDICOMPrefix) << "WARNING: the DICOM file is not valid because the voxel data has "
            "more than one channel of data which is not supported by vipreDICOM: " << _filepath << std::endl;
        return false;
    }

    //=============================================================================================
    // Ignore compressed DICOM files since currently compressed datasets are not supported
    if (dcmFormat.getMetaInfo()->findAndGetOFString(DCM_TransferSyntaxUID, temp_string).bad())
    {
        vipreLogWARNING(vipreDICOMPrefix) << "WARNING: DICOM file is not valid because it is compressed: " << _filepath << std::endl;
        return false;
    }
    else
    {
        // Files that contain these tags are compressed. Each tag represents a different type of
        // compression and free DCMTK cannot read these.
        if (temp_string.compare("1.2.840.10008.1.2.4.50") == 0 ||
            temp_string.compare("1.2.840.10008.1.2.4.51") == 0 ||
            temp_string.compare("1.2.840.10008.1.2.4.52") == 0 ||
            temp_string.compare("1.2.840.10008.1.2.4.53") == 0 ||
            temp_string.compare("1.2.840.10008.1.2.4.54") == 0 ||
            temp_string.compare("1.2.840.10008.1.2.4.55") == 0 ||
            temp_string.compare("1.2.840.10008.1.2.4.56") == 0 ||
            temp_string.compare("1.2.840.10008.1.2.4.57") == 0 ||
            temp_string.compare("1.2.840.10008.1.2.4.58") == 0 ||
            temp_string.compare("1.2.840.10008.1.2.4.59") == 0 ||
            temp_string.compare("1.2.840.10008.1.2.4.60") == 0 ||
            temp_string.compare("1.2.840.10008.1.2.4.61") == 0 ||
            temp_string.compare("1.2.840.10008.1.2.4.62") == 0 ||
            temp_string.compare("1.2.840.10008.1.2.4.63") == 0 ||
            temp_string.compare("1.2.840.10008.1.2.4.64") == 0 ||
            temp_string.compare("1.2.840.10008.1.2.4.65") == 0 ||
            temp_string.compare("1.2.840.10008.1.2.4.66") == 0 ||
            temp_string.compare("1.2.840.10008.1.2.4.70") == 0 ||
            temp_string.compare("1.2.840.10008.1.2.4.80") == 0 ||
            temp_string.compare("1.2.840.10008.1.2.4.81") == 0)
        {
            vipreLogWARNING(vipreDICOMPrefix) << "WARNING: DICOM file is compressed in a format that the free "
                "version of DCMTK cannot read: " << _filepath << std::endl;
            return false;
        }
    }

    /*

     //==================================================================================================
     // Below is a list of all the information we are going to attempt to extract out of the DICOM file.
     // If any of the "Required" tags are not found, then the Slice is consider to be invalid.
     ==================================================================================================

     vipre::String _studyInstanceID;                        DCM_StudyInstanceUID                // Required
     vipre::String _studyID;                                DCM_StudyID                         // Required
     vipre::String _studyDescription;                       DCM_StudyDescription                // No
     vipre::String _studyDate;                              DCM_StudyDate                       // No

     vipre::String _seriesID;                               DCM_SeriesInstanceUID               // Required
     vipre::String _seriesNumber;                           DCM_SeriesNumber                    // Required
     vipre::String _seriesDescription;                      DCM_SeriesDescription               // No
     vipre::String _seriesDate;                             DCM_SeriesDate                      // No

     vipre::String _patientID;                              DCM_PatientID                       // No
     vipre::String _patientName;                            DCM_PatientName                     // No
     vipre::String _patientAge;                             DCM_PatientAge                      // No
     vipre::String _patientDateOfBirth;                     DCM_PatientBirthDate                // No
     vipre::String _patientSex;                             DCM_PatientSex                      // No
     vipre::String _patientWeight;                          DCM_PatientWeight                   // No
     vipre::String _patientAdditionalHistory;               DCM_AdditionalPatientHistory        // No
     vipre::String _institution;                            DCM_InstitutionName                 // No

     vipre::String _modality;                               DCM_Modality                        // No
     vipre::String _imageType;                              DCM_ImageType                       // No
     vipre::String _contrastType;                           DCM_ContrastBolusAgent              // No
     vipre::String _manufacturer;                           DCM_Manufacturer                    // No

     boost::array<double, 6> _imageOrientationPatient;      DCM_ImageOrientationPatient         // Required
     double _location;                                      DCM_SliceLocation                   // Required
     double _thickness;                                     DCM_SlabThickness                   // Required
     VoxelSpacing _voxelSpacing;                            DCM_PixelSpacing                    // Required
     Dimensions _dimensions;                                DCM_Columns & DCM_Rows              // Required

     */

    // Extract the image dataset (most info comes out of this)
    DcmDataset* imgDataset = dcmFormat.getDataset();

    //=============================================================================================
    // Extract the study instance id
    if (imgDataset->findAndGetOFString(DCM_StudyInstanceUID, temp_string).bad())
    {
        vipreLogWARNING(vipreDICOMPrefix) << "WARNING: the study instance id could not be extracted from the DICOM file: " << _filepath << std::endl;
        return false;
    }
    _studyInstanceID = temp_string.c_str();

    //=============================================================================================
    // Extract the study id
    if (imgDataset->findAndGetOFString(DCM_StudyID, temp_string).bad())
    {
        vipreLogWARNING(vipreDICOMPrefix) << "WARNING: the study id could not be extracted from the DICOM file: " << _filepath << std::endl;
        return false;
    }
    _studyID = temp_string.c_str();

    //=============================================================================================
    // Extract the study description
    if (imgDataset->findAndGetOFString(DCM_StudyDescription, temp_string).good())
    {
        _studyDescription = temp_string.c_str();
    }

    //=============================================================================================
    // Extract the study date
    if (imgDataset->findAndGetOFString(DCM_StudyDate, temp_string).good())
    {
        _studyDate = temp_string.c_str();
    }

    //=============================================================================================
    // Extract the series instance id
    if (imgDataset->findAndGetOFString(DCM_SeriesInstanceUID, temp_string).bad())
    {
        vipreLogWARNING(vipreDICOMPrefix) << "WARNING: the series instance id could not be extracted from the DICOM file: " << _filepath << std::endl;
        return false;
    }
    _seriesID = temp_string.c_str();

    //=============================================================================================
    // Extract the series number
    if (imgDataset->findAndGetOFString(DCM_SeriesNumber, temp_string).bad())
    {
        vipreLogWARNING(vipreDICOMPrefix) << "WARNING: the series number could not be extracted from the DICOM file: " << _filepath << std::endl;
        return false;
    }
    _seriesNumber = temp_string.c_str();

    //=============================================================================================
    // Extract the series description
    if (imgDataset->findAndGetOFString(DCM_SeriesDescription, temp_string).good())
    {
        _seriesDescription = temp_string.c_str();
    }

    //=============================================================================================
    // Extract the series date
    if (imgDataset->findAndGetOFString(DCM_SeriesDate, temp_string).good())
    {
        _seriesDate = temp_string.c_str();
    }

    //=============================================================================================
    // Extract the patient id
    if (imgDataset->findAndGetOFString(DCM_PatientID, temp_string).good())
    {
        _patientID = temp_string.c_str();
    }

    //=============================================================================================
    // Extract the patient name
    if (imgDataset->findAndGetOFString(DCM_PatientName, temp_string).good())
    {
        _patientName = temp_string.c_str();
    }

    //=============================================================================================
    // Extract the patient age
    if (imgDataset->findAndGetOFString(DCM_PatientAge, temp_string).good())
    {
        _patientAge = temp_string.c_str();
    }

    //=============================================================================================
    // Extract the patient date of birth
    if (imgDataset->findAndGetOFString(DCM_PatientBirthDate, temp_string).good())
    {
        _patientDateOfBirth = temp_string.c_str();
    }

    //=============================================================================================
    // Extract the patient sex
    if (imgDataset->findAndGetOFString(DCM_PatientSex, temp_string).good())
    {
        _patientSex = temp_string.c_str();
    }

    //=============================================================================================
    // Extract the patient weight
    if (imgDataset->findAndGetOFString(DCM_PatientWeight, temp_string).good())
    {
        _patientWeight = temp_string.c_str();
    }

    //=============================================================================================
    // Extract the patient additional history
    if (imgDataset->findAndGetOFString(DCM_AdditionalPatientHistory, temp_string).good())
    {
        _patientAdditionalHistory = temp_string.c_str();
    }

    //=============================================================================================
    // Extract the institution
    if (imgDataset->findAndGetOFString(DCM_InstitutionName, temp_string).good())
    {
        _institution = temp_string.c_str();
    }

    //=============================================================================================
    // Extract the modality
    if (imgDataset->findAndGetOFString(DCM_Modality, temp_string).good())
    {
        _modality = temp_string.c_str();
    }

    //=============================================================================================
    // Extract the image type
    if (imgDataset->findAndGetOFString(DCM_ImageType, temp_string).good())
    {
        _imageType = temp_string.c_str();
    }

    //=============================================================================================
    // Extract the contrast type
    if (imgDataset->findAndGetOFString(DCM_ContrastBolusAgent, temp_string).good())
    {
        _contrastType = temp_string.c_str();
    }

    //=============================================================================================
    // Extract the manufacturer
    if (imgDataset->findAndGetOFString(DCM_Manufacturer, temp_string).good())
    {
        _manufacturer = temp_string.c_str();
    }

    //=============================================================================================
    // Extract the patient image orientation
    bool orientationSuccess = true;
    if (imgDataset->findAndGetFloat64(DCM_ImageOrientationPatient, temp_float, 0).bad())
    {
        orientationSuccess = false;
    }
    _imageOrientationPatient.leftAxis[0] = temp_float;

    if (imgDataset->findAndGetFloat64(DCM_ImageOrientationPatient, temp_float, 1).bad())
    {
        orientationSuccess = false;
    }
    _imageOrientationPatient.leftAxis[1] = temp_float;

    if (imgDataset->findAndGetFloat64(DCM_ImageOrientationPatient, temp_float, 2).bad())
    {
        orientationSuccess = false;
    }
    _imageOrientationPatient.leftAxis[2] = temp_float;

    if (imgDataset->findAndGetFloat64(DCM_ImageOrientationPatient, temp_float, 3).bad())
    {
        orientationSuccess = false;
    }
    _imageOrientationPatient.anteriorAxis[0] = temp_float;

    if (imgDataset->findAndGetFloat64(DCM_ImageOrientationPatient, temp_float, 4).bad())
    {
        orientationSuccess = false;
    }
    _imageOrientationPatient.anteriorAxis[1] = temp_float;

    if (imgDataset->findAndGetFloat64(DCM_ImageOrientationPatient, temp_float, 5).bad())
    {
        orientationSuccess = false;
    }
    _imageOrientationPatient.anteriorAxis[2] = temp_float;

    if (orientationSuccess == false)
    {
        vipreLogWARNING(vipreDICOMPrefix) << "WARNING: the image orientation patient could not be extracted from the "
                          "DICOM file: " << _filepath << std::endl;
        return false;
    }

    //=============================================================================================
    // Extract the slice location
    if (imgDataset->findAndGetFloat64(DCM_SliceLocation, temp_float).bad())
    {
        vipreLogWARNING(vipreDICOMPrefix) << "WARNING: the slice location could not be extracted from the DICOM file: " << _filepath << std::endl;
        return false;
    }
    _location = fabs(temp_float);

    //=============================================================================================
    // Extract the slice thickness
    const char* thickness = "--e";
    if (imgDataset->findAndGetString(DCM_SliceThickness, thickness).bad())
    {
        vipreLogWARNING(vipreDICOMPrefix) << "WARNING: the slice thickness could not be extracted from the DICOM file: " << _filepath << std::endl;
        return false;
    }
    _thickness = atof(thickness);

    //=============================================================================================
    // Extract the pixel spacing
    bool pixelSpacingSuccess = true;
    if (imgDataset->findAndGetFloat64(DCM_PixelSpacing, temp_float, 0).bad())
    {
        pixelSpacingSuccess = false;
    }
    _voxelSpacing.width = fabs(temp_float);

    if (imgDataset->findAndGetFloat64(DCM_PixelSpacing, temp_float, 1).bad())
    {
        pixelSpacingSuccess = false;
    }
    _voxelSpacing.height = fabs(temp_float);
    _voxelSpacing.depth = 1.0f;

    if (pixelSpacingSuccess == false)
    {
        vipreLogWARNING(vipreDICOMPrefix) << "WARNING: the pixel spacing could not be extracted from the DICOM file: " << _filepath << std::endl;
        return false;
    }

    //=============================================================================================
    // Extract the slice resolution
    bool resolutionSuccess = true;
    if (imgDataset->findAndGetUint16(DCM_Rows, temp_int).bad())
    {
        resolutionSuccess = false;
    }
    _dimensions.width = temp_int;

    if (imgDataset->findAndGetUint16(DCM_Columns, temp_int).bad())
    {
        resolutionSuccess = false;
    }
    _dimensions.height = temp_int;
    _dimensions.depth = 1;

    if (resolutionSuccess == false)
    {
        vipreLogWARNING(vipreDICOMPrefix) << "WARNING: the slice resolution (DCM_Rows & DCM_Columns) could not be "
            "extracted from the DICOM file: " << _filepath << std::endl;
        return false;
    }

    // Update the header extraction and isValid flags
    _headerInfoExtracted = true;
    _isValid = true;

    return true;
}

bool Slice::extractVoxelData()
{
    // First make sure the header info has been extracted
    if (_headerInfoExtracted == false)
    {
        extractHeaderInfo();
    }

    // Early out if the DICOM file is not valid
    if (_isValid == false)
    {
        return false;
    }

    // Debug output
    vipreLogDEBUG(vipreDICOMPrefix) << "Extracting slice voxel data for file: " << _filepath << std::endl;

    // Get access to the pixel data
    DcmFileFormat dcmFormat;
    dcmFormat.loadFile(_filepath.c_str());
    DicomImage dImage(&dcmFormat, dcmFormat.getDataset()->getOriginalXfer());
    const DiPixel* pixelData = dImage.getInterData();

    // Compute the total voxels
    unsigned long numberOfVoxels = _dimensions.width * _dimensions.height;

    // Initialize the abstract voxel data array
    if (pixelData->getRepresentation() == EPR_Uint8)
    {
        _voxelData = initializeVoxelData(UNSIGNED_INT_8, numberOfVoxels);
    }
    else if (pixelData->getRepresentation() == EPR_Sint8)
    {
        _voxelData = initializeVoxelData(SIGNED_INT_8, numberOfVoxels);
    }
    else if (pixelData->getRepresentation() == EPR_Uint16)
    {
        _voxelData = initializeVoxelData(UNSIGNED_INT_16, numberOfVoxels);
    }
    else if (pixelData->getRepresentation() == EPR_Sint16)
    {
        _voxelData = initializeVoxelData(SIGNED_INT_16, numberOfVoxels);
    }
    else if (pixelData->getRepresentation() == EPR_Uint32)
    {
        _voxelData = initializeVoxelData(UNSIGNED_INT_32, numberOfVoxels);
    }
    else // EPR_Sint32
    {
        _voxelData = initializeVoxelData(SIGNED_INT_32, numberOfVoxels);
    }

    // Set the minHU and maxHU equal to the min and max limits of an int
    double minHU = DBL_MAX;
    double maxHU = DBL_MIN;

    // Copy all the DICOM pixel data into the voxel data array
    unsigned long pixelIndex;
    unsigned long sliceVoxelIndex;
    double voxelValue;
    for (unsigned int i = 0; i < _dimensions.height; ++i)
    {
        for (unsigned int j = 0; j < _dimensions.width; ++j)
        {
            // Compute the pixel and voxel indices for copying.
            // NOTE: The reason we need to do this is that the DCTMK image needs to be horizontally
            // flipped in order to match up correctly to OSG. Therefore, we automatically flip the indexing
            // here so we don't have to do it later.
            pixelIndex = i * _dimensions.width + j;
            sliceVoxelIndex = (_dimensions.height - 1 - i) * _dimensions.width + j;

            // Copy over the pixel value
            voxelValue = copyPixelVoxelToSliceVoxel(pixelIndex, sliceVoxelIndex, pixelData);

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
    }

    // Set the min and max voxel values
    _voxelData->setMinMaxValues(minHU, maxHU);

    // Update the voxel data extraction flag
    _voxelDataExtracted = true;

    return true;
}

double Slice::copyPixelVoxelToSliceVoxel(unsigned long pixelIndex, unsigned long sliceVoxelIndex, const DiPixel* pixelData)
{
    double voxelValue = 0.0f;
    if (pixelData->getRepresentation() == EPR_Uint8)
    {
        const Uint8* rawData = static_cast<const Uint8 *>(pixelData->getData());
        unsigned char* voxels = static_cast<unsigned char *>(_voxelData->getDataPointer());
        voxelValue = rawData[pixelIndex];
        voxels[sliceVoxelIndex] = voxelValue;
    }
    else if (pixelData->getRepresentation() == EPR_Sint8)
    {
        const Sint8* rawData = static_cast<const Sint8 *>(pixelData->getData());
        char* voxels = static_cast<char *>(_voxelData->getDataPointer());
        voxelValue = rawData[pixelIndex];
        voxels[sliceVoxelIndex] = voxelValue;
    }
    else if (pixelData->getRepresentation() == EPR_Uint16)
    {
        const Uint16* rawData = static_cast<const Uint16 *>(pixelData->getData());
        unsigned short* voxels = static_cast<unsigned short *>(_voxelData->getDataPointer());
        voxelValue = rawData[pixelIndex];
        voxels[sliceVoxelIndex] = voxelValue;
    }
    else if (pixelData->getRepresentation() == EPR_Sint16)
    {
        const Sint16* rawData = static_cast<const Sint16 *>(pixelData->getData());
        short* voxels = static_cast<short *>(_voxelData->getDataPointer());
        voxelValue = rawData[pixelIndex];
        voxels[sliceVoxelIndex] = voxelValue;
    }
    else if (pixelData->getRepresentation() == EPR_Uint32)
    {
        const Uint32* rawData = static_cast<const Uint32 *>(pixelData->getData());
        unsigned int* voxels = static_cast<unsigned int *>(_voxelData->getDataPointer());
        voxelValue = rawData[pixelIndex];
        voxels[sliceVoxelIndex] = voxelValue;
    }
    else if (pixelData->getRepresentation() == EPR_Sint32)
    {
        const Sint32* rawData = static_cast<const Sint32 *>(pixelData->getData());
        int* voxels = static_cast<int *>(_voxelData->getDataPointer());
        voxelValue = rawData[pixelIndex];
        voxels[sliceVoxelIndex] = voxelValue;
    }
    else
    {
        vipreLogWARNING(vipreDICOMPrefix) << "WARNING: The pixel data representation is not handled properly: "
            << pixelData->getRepresentation() << std::endl;
    }

    return voxelValue;
}

void Slice::printInfo()
{
    vipreLogALWAYS(vipreDICOMPrefix) << "vipreDICOM::Slice Info: " << getFilename() << std::endl;

    // Print out the header info
    if (!_headerInfoExtracted)
    {
        vipreLogALWAYS(vipreDICOMPrefix) << "\tHeader Info: has not been extracted, please see extractHeaderInfo() method" << std::endl;
    }
    else
    {
        vipreLogALWAYS(vipreDICOMPrefix) << "\tHeader Info:" << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(R) Study Instance ID:          " << _studyInstanceID << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(R) Study ID:                   " << _studyID << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(N) Study Description:          " << _studyDescription << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(N) Study Date:                 " << _studyDate << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(R) Series Instance ID:         " << _seriesID << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(R) Series Number:              " << _seriesNumber << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(N) Series Description:         " << _seriesDescription << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(N) Series Date:                " << _seriesDate << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(N) Patient ID:                 " << _patientID << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(N) Patient Name:               " << _patientName << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(N) Patient Age:                " << _patientAge << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(N) Patient Date of Birth:      " << _patientDateOfBirth << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(N) Patient Sex:                " << _patientSex << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(N) Patient Weight:             " << _patientWeight << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(N) Patient Additional History: " << _patientAdditionalHistory << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(N) Institution Name:           " << _institution << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(N) Modality:                   " << _modality << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(N) Image Type:                 " << _imageType << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(N) Contrast:                   " << _contrastType << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(N) Manufacturer:               " << _manufacturer << std::endl;
        osg::Vec3f leftAxis = _imageOrientationPatient.leftAxis;
        osg::Vec3f anteriorAxis = _imageOrientationPatient.anteriorAxis;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(R) Image Orientation Patient:  ("
            << leftAxis[0] << ", " << leftAxis[1] << ", " << leftAxis[2] << ") ("
            << anteriorAxis[0] << ", " << anteriorAxis[1] << ", " << anteriorAxis[2] << ")" << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(R) Slice Location:             " << _location << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(R) Slice Thickness:            " << _thickness << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(R) Voxel Spacing               ("
            << _voxelSpacing.width << ", " << _voxelSpacing.height << ", " << _voxelSpacing.depth << ")" << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(R) Resolution                  ("
            << _dimensions.width << ", " << _dimensions.height << ", " << _dimensions.depth << ")" << std::endl;
    }

    // Print out the voxel data info
    if (!_voxelDataExtracted)
    {
        vipreLogALWAYS(vipreDICOMPrefix) << "\tVoxel Info: has not been extracted, please see extractVoxelData() method" << std::endl;
    }
    else
    {
        vipreLogALWAYS(vipreDICOMPrefix) << "\tVoxel Info:" << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(R) Data Type:                  " << _voxelData->getDataTypeAsString() << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(R) Data Type Size in Bytes:    " << _voxelData->getDataTypeSizeInBytes() << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(R) Number of Channels:         " << _voxelData->getNumberOfChannels() << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(R) Number of Voxels:           " << _voxelData->getNumberOfVoxels() << std::endl;
        double minValue, maxValue;
        _voxelData->getMinMaxValues(minValue, maxValue);
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(R) Min Voxel Value:            " << minValue << std::endl;
        vipreLogALWAYS(vipreDICOMPrefix) << "\t\t(R) Max Voxel Value:            " << maxValue << std::endl;
    }
}
