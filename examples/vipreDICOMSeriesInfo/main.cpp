//
//  main.cpp
//  vipreDICOMSeriesInfo
//
//  Created by Christian Noon on 12/3/11.
//  Copyright (c) 2011 Christian Noon. All rights reserved.
//

#include <osg/ArgumentParser>
#include <osg/Notify>
#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>

#include <vipre/Log.hpp>
#include <vipre/String.hpp>
#include <vipreDICOM/Series.hpp>

#define examplePrefix       "[vipreDICOMSeriesInfo] "

/**
 * This example demonstrates how to create a DICOM series from a directory of DICOM files.
 *
 * By specifying a directory full of DICOM data files, a vipreDICOM::Series will be created
 * from them. If the Series is valid, all the information will be displayed. This example
 * builds 4 different versions of the Series:
 *
 * 1) A Regular Series
 * 2) An UNSIGNED_INT_8 Normalized Series that dynamically releases slice voxel data to minimize memory footprint
 * 3) An UNSIGNED_INT_16 Normalized Series
 * 4) An UNSIGNED_INT_32 Normalized Series
 */
int main(int argc, char **argv)
{
    // Set the logging levels for both vipre and osg
    vipre::Log::instance()->setLogLevel(vipre::INFO_LVL);
    osg::setNotifyLevel(osg::WARN);

    // Use an ArgumentParser object to manage the program arguments.
    osg::ArgumentParser arguments(&argc, argv);
    osg::ApplicationUsage* appUsage = arguments.getApplicationUsage();

    // Set up the usage document, in case we need to print out how to use this program.
    appUsage->setDescription(osgDB::getSimpleFileName(arguments.getApplicationName()) + " is an example which demonstrates how to create a DICOM series from a directory of DICOM files.");
    appUsage->setCommandLineUsage(osgDB::getSimpleFileName(arguments.getApplicationName()) + " DICOMDir1 DICOMDir2 ...");
    appUsage->addCommandLineOption("-h or --help", "Display this information");

    // If user request help write it out to std::cout
    if (arguments.read("-h") || arguments.read("--help") || arguments.argc() < 2)
    {
        appUsage->write(std::cout);
        return 1;
    }

    // Any options passed in as --something are going to be reported as errors
    arguments.reportRemainingOptionsAsUnrecognized();
    if (arguments.errors())
    {
        arguments.writeErrorMessages(std::cout);
        return 1;
    }

    // Parse each DICOM file passed in and print out all its extracted information
    for (int i = 1; i < arguments.argc(); ++i)
    {
        if (!arguments.isOption(i))
        {
            vipre::String directoryPath = arguments[i];
            osgDB::FileType fileType = osgDB::fileType(directoryPath);
            if (fileType == osgDB::DIRECTORY)
            {
                vipre::StringList files;
                osgDB::DirectoryContents directoryContents = osgDB::getDirectoryContents(directoryPath);
                for (unsigned int i = 0; i < directoryContents.size(); ++i)
                {
                    vipre::String potentialFile = osgDB::concatPaths(directoryPath, directoryContents.at(i));
                    fileType = osgDB::fileType(potentialFile);
                    if (fileType == osgDB::REGULAR_FILE)
                    {
                        files.push_back(potentialFile);
                    }
                }

                // Create a series from the list of files
                vipreLogINFO(examplePrefix) << "\n==================================================================================" << std::endl;
                vipreLogINFO(examplePrefix) << "Loading directory: " << directoryPath << " containing " << directoryContents.size() << " files\n" << std::endl;

                // Create a regular series
                vipreLogINFO(examplePrefix) << "Creating regular series.....\n" << std::endl;
                osg::ref_ptr<vipreDICOM::Series> series = new vipreDICOM::Series(files);
                bool isValid = series->validateSeries();
                if (isValid)
                {
                    series->buildVoxelData(false);
                    series->printInfo();
                }

                vipreLogINFO(examplePrefix) << std::endl;

                // Create a normalized UNSIGNED_INT_8 series
                vipreLogINFO(examplePrefix) << "Creating normalized UNSIGNED_INT_8 series.....\n" << std::endl;
                series = new vipreDICOM::Series(files);
                isValid = series->validateSeries();
                if (isValid)
                {
                    // Build the voxel data as a normalized dataset as an unsigned char. This can be very useful
                    // on mobile devices. In iOS 5, OpenGLES 2 only supported GL_UNSIGNED_BYTE textures. Additionally,
                    // the example is releasing the slice voxel data as it is building the series voxel data array
                    // to minimize the memory footprint.
                    series->buildNormalizedVoxelData(vipreDICOM::Series::NORMALIZED_UNSIGNED_INT_8, false, true);
                    series->printInfo();
                }

                vipreLogINFO(examplePrefix) << std::endl;

                // Create a normalized UNSIGNED_INT_16 series
                vipreLogINFO(examplePrefix) << "Creating normalized UNSIGNED_INT_16 series.....\n" << std::endl;
                series = new vipreDICOM::Series(files);
                isValid = series->validateSeries();
                if (isValid)
                {
                    series->buildNormalizedVoxelData(vipreDICOM::Series::NORMALIZED_UNSIGNED_INT_16, true, false);
                    series->printInfo();
                }

                vipreLogINFO(examplePrefix) << std::endl;

                // Create a normalized UNSIGNED_INT_32 series
                vipreLogINFO(examplePrefix) << "Creating normalized UNSIGNED_INT_32 series.....\n" << std::endl;
                series = new vipreDICOM::Series(files);
                isValid = series->validateSeries();
                if (isValid)
                {
                    series->buildNormalizedVoxelData(vipreDICOM::Series::NORMALIZED_UNSIGNED_INT_32, true, false);
                    series->printInfo();
                }
            }
        }
    }

    return 0;
}
