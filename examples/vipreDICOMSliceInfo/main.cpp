//
//  main.cpp
//  vipreDICOMSliceInfo
//
//  Created by Christian Noon on 12/3/11.
//  Copyright (c) 2011 Christian Noon. All rights reserved.
//

#include <osg/ArgumentParser>
#include <osg/Notify>
#include <osgDB/FileNameUtils>

#include <vipre/Log.hpp>
#include <vipre/String.hpp>
#include <vipreDICOM/Slice.hpp>

#define examplePrefix   "[vipreDICOMSliceInfo] "

/**
 * This example demonstrates how to extract DICOM information from DICOM files.
 *
 * To get the header and voxel information out of a DICOM file, all that is required is usage of the
 * vipreDICOM::Slice class. Follow this example to see how to print out the information.
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
    appUsage->setDescription(osgDB::getSimpleFileName(arguments.getApplicationName()) + " is an example which demonstrates how to extract DICOM information from DICOM files.");
    appUsage->setCommandLineUsage(osgDB::getSimpleFileName(arguments.getApplicationName()) + " DICOMfile1 DICOMfile2 ...");
    appUsage->addCommandLineOption("-h or --help","Display this information");

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
            vipre::String filename = arguments[i];
            vipreLogINFO(examplePrefix) << "\n==================================================================================" << std::endl;
            vipreLogINFO(examplePrefix) << "Loading file: " << filename << "...\n" << std::endl;
            osg::ref_ptr<vipreDICOM::Slice> slice = new vipreDICOM::Slice(filename);
            slice->extractHeaderInfo();
            slice->extractVoxelData();
            slice->printInfo();
        }
    }

    return 0;
}
