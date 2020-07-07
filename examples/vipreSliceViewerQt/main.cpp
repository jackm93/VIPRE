//
//  main.cpp
//  vipreSliceViewerQt
//
//  Created by Christian Noon on 12/5/11.
//  Copyright (c) 2011 Christian Noon. All rights reserved.
//

#include "MainWindow.hpp"

#include <osg/Notify>
#include <osg/ref_ptr>
#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>

#include <QtGui/QApplication>

#include <vipre/Log.hpp>
#include <vipre/TexturedQuad.hpp>
#include <vipreDICOM/SeriesBuilder.hpp>

#define examplePrefix       "[vipreSliceViewerQt] "

/**
 * Returns the first unique vipreDICOM::Series it finds.
 *
 * NOTE: This uses the vipreDICOM::SeriesBuilder to do so.
 */
osg::ref_ptr<vipreDICOM::Series> createSeriesList(osg::ArgumentParser& arguments,
                                                  bool useFiles,
                                                  bool useDirectories,
                                                  bool recursive,
                                                  bool validate)
{
    // Parse out all the files and use the series builder to find all unique series objects
    vipreDICOM::SeriesList uniqueSeriesList;
    if (useFiles)
    {
        // Extract all the filepaths from the remaining command line arguments
        vipre::StringList files;
        for (int i = 1; i < arguments.argc(); ++i)
        {
            if (!arguments.isOption(i))
            {
                files.push_back(arguments[i]);
            }
        }

        // User output
        if (files.size() < 2)
        {
            vipreLogINFO(examplePrefix) << "Building unique series list from " << files.size() << " file." << std::endl;
        }
        else
        {
            vipreLogINFO(examplePrefix) << "Building unique series list from " << files.size() << " files." << std::endl;
        }

        // Use the SeriesBuilder to build a unique series list
        uniqueSeriesList = vipreDICOM::SeriesBuilder::buildSeriesListFromFilePaths(files, validate);
    }
    else // useDirectories
    {
        // Extract all the directories from the remaining command line arguments
        vipre::StringList directories;
        for (int i = 1; i < arguments.argc(); ++i)
        {
            if (!arguments.isOption(i))
            {
                directories.push_back(arguments[i]);
            }
        }

        // User output
        if (directories.size() < 2)
        {
            vipreLogINFO(examplePrefix) << "Building unique series list from " << directories.size() << " directory: "
                << directories.at(0) << "\n" << std::endl;
        }
        else
        {
            vipreLogINFO(examplePrefix) << "Building unique series list from " << directories.size() << " directories.\n" << std::endl;
            for (unsigned int i = 0; i < directories.size(); ++i)
            {
                vipreLogINFO(examplePrefix) << "\t- " << directories.at(i) << std::endl;
            }
            vipreLogINFO(examplePrefix) << std::endl;
        }

        // Use the SeriesBuilder to build a unique series list
        uniqueSeriesList = vipreDICOM::SeriesBuilder::buildSeriesListFromDirectoryPaths(directories, recursive, validate);
    }

    // Handle the different cases of how many series objects were found
    return uniqueSeriesList.empty() ? NULL : uniqueSeriesList.at(0);
}

/** Creates a textured quad from all the data stored in a slice. */
osg::ref_ptr<vipre::TexturedQuad> createTexturedQuadFromSlice(osg::ref_ptr<vipreDICOM::Slice> slice)
{
    // Get the voxel dimensions
    osg::Vec3f voxelDimensions = slice->getDimensions().toVec3d();

    // Get the raw voxel data
    osg::ref_ptr<vipreDICOM::VoxelData> voxelData = slice->getVoxelData();
    void* voxels = voxelData->getDataPointer();

    // Get the voxel data type (NOTE, if normalized, the slice data type is wrong)
    vipreDICOM::VoxelDataType dataType = voxelData->getDataType();
    GLenum voxelDataType;
    if (dataType == vipreDICOM::UNSIGNED_INT_8)
    {
        voxelDataType = GL_UNSIGNED_BYTE;
    }
    else if (dataType == vipreDICOM::SIGNED_INT_8)
    {
        voxelDataType = GL_BYTE;
    }
    else if (dataType == vipreDICOM::UNSIGNED_INT_16)
    {
        voxelDataType = GL_UNSIGNED_SHORT;
    }
    else if (dataType == vipreDICOM::SIGNED_INT_16)
    {
        voxelDataType = GL_SHORT;
    }
    else if (dataType == vipreDICOM::UNSIGNED_INT_32)
    {
        voxelDataType = GL_UNSIGNED_INT;
    }
    else // vipreDICOM::SIGNED_INT_32
    {
        voxelDataType = GL_INT;
    }

    // Get the voxel channel type
    unsigned int numberOfChannels = voxelData->getNumberOfChannels();
    GLenum voxelChannelType;
    if (numberOfChannels == 1)
    {
        voxelChannelType = GL_LUMINANCE;
    }
    else if (numberOfChannels == 3)
    {
        voxelChannelType = GL_RGB;
    }
    else // 4
    {
        voxelChannelType = GL_RGBA;
    }

    // Create the vertices
    osg::ref_ptr<osg::Vec3dArray> vertices = new osg::Vec3dArray();
    vertices->push_back(osg::Vec3d(0.0f, 0.0f, 0.0f));
    vertices->push_back(osg::Vec3d(voxelDimensions[0], 0.0f, 0.0f));
    vertices->push_back(osg::Vec3d(voxelDimensions[0], 0.0f, voxelDimensions[1]));
    vertices->push_back(osg::Vec3d(0.0f, 0.0f, voxelDimensions[1]));

    // Finally create the textured quad
    osg::ref_ptr<vipre::TexturedQuad> quad = new vipre::TexturedQuad();
    quad->build(vertices, osg::Vec2d(voxelDimensions[0], voxelDimensions[1]), voxels, voxelChannelType, voxelDataType);

    return quad;
}

/** Creates a switch containing a textured quad for each slice in the series. */
osg::ref_ptr<osg::Switch> createSeriesSwitch(osg::ref_ptr<vipreDICOM::Series> series, bool normalize)
{
    // First we need to build all the series voxel data
    if (normalize)
    {
        series->validateSeries();
        series->buildNormalizedVoxelData(vipreDICOM::Series::NORMALIZED_UNSIGNED_INT_16, true, false);
    }
    else
    {
        series->validateSeries();
        series->buildVoxelData(false);
    }

    osg::ref_ptr<osg::Switch> seriesSwitch = new osg::Switch();
    const vipreDICOM::SliceList& slices = series->getSlices();
    vipreDICOM::SliceList::const_reverse_iterator iter = slices.rbegin();
    while (iter != slices.rend())
    {
        // Create a textured quad using all the generated info
        osg::ref_ptr<vipre::TexturedQuad> quad = createTexturedQuadFromSlice(*iter);

        // Add the textured quad to the series switch
        seriesSwitch->addChild(quad.get());

        ++iter;
    }

    // Turn on only the first quad
    seriesSwitch->setSingleChildOn(0);

    return seriesSwitch;
}

/**
 * This example demonstrates how to create vipre::TexturedQuads from vipreDICOM::Slices using Qt.
 *
 * There are a series of steps involved with visualizing the data. Here goes:
 * 1) Create a list of files based on the series of command line arguments
 * 2) Use the vipreDICOM::SeriesBuilder API to find the unique series objects
 *     - NOTE: This example only uses the first one it finds if it finds more than one
 * 3) The vipreDICOM::Series voxel data is extract and normalized (if specified)
 * 4) A vipre::TexturedQuad is created for each vipreDICOM::Slice in the vipreDICOM::Series
 * 5) All the vipre::TexturedQuads are added to an osg::Switch
 * 6) A QMainWindow is initialized from the Designer UI file.
 * 7) All user interaction callback are coupled to the controller class
 * 8) The osg::Switch is set as the root of the QOSGWidget scene
 * 9) The application is launched showing the first slice
 * 10) You can switch between slices two way:
 *     - using the 'n' and 'p' keys (next/previous)
 *     - using the "Options" dropdown menu
 *
 */
int main(int argc, char **argv)
{
    // Set the logging levels for both vipre and osg
    vipre::Log::instance()->setLogLevel(vipre::DEBUG_LVL);
    osg::setNotifyLevel(osg::FATAL);

    // Use an ArgumentParser object to manage the program arguments.
    osg::ArgumentParser arguments(&argc, argv);
    osg::ApplicationUsage* appUsage = arguments.getApplicationUsage();

    // Set up the usage document, in case we need to print out how to use this program.
    vipre::String description(vipre::String() << osgDB::getSimpleFileName(arguments.getApplicationName()) <<
                              " is an example which demonstrates how to create unique, validated series objects "
                              "from a list of DICOM files or directories. Please note that you must specify either "
                              "the --files or --directories command line argument when launching this example.");
    appUsage->setDescription(description);
    appUsage->setCommandLineUsage(osgDB::getSimpleFileName(arguments.getApplicationName()) + " [options] fileOrDir1 fileOrDir2 ...");
    appUsage->addCommandLineOption("-h or --help", "Display this information");
    appUsage->addCommandLineOption("--files", "Uses the buildSeriesListFromFilePaths() implementation");
    appUsage->addCommandLineOption("--directories", "Uses the buildSeriesListFromDirectoryPaths() implementation");
    appUsage->addCommandLineOption("--recursive", "Searches all directory path subdirectories for additional files");
    appUsage->addCommandLineOption("--validate", "SeriesBuilder will only returns validated Series object");
    appUsage->addCommandLineOption("--normalize", "Normalizes the data to the unsigned version of the current data type");

    // If user request help write it out to std::cout
    if (arguments.read("-h") || arguments.read("--help") || arguments.argc() < 2)
    {
        appUsage->write(std::cout);
        return 1;
    }

    // Extract the command line arguments
    bool useFiles = false;
    bool useDirectories = false;
    bool recursive = false;
    bool validate = false;
    bool normalize = false;
    while (arguments.read("--files"))
    {
        useFiles = true;
    }
    while (arguments.read("--directories"))
    {
        useDirectories = true;
    }
    while (arguments.read("--recursive"))
    {
        recursive = true;
    }
    while (arguments.read("--validate"))
    {
        validate = true;
    }
    while (arguments.read("--normalize"))
    {
        normalize = true;
    }

    // Make sure they selected either files or directories
    if (!useFiles && !useDirectories)
    {
        vipreLogERROR(examplePrefix) << "Usage Error: you cannot launch this application without specifying either --files or --directories.\n" << std::endl;
        appUsage->write(std::cout);
    }

    // Make sure they don't launch with both files and directories active
    if (useFiles && useDirectories)
    {
        vipreLogERROR(examplePrefix) << "Usage Error: you cannot launch this application with both --files and --directories.\n" << std::endl;
        appUsage->write(std::cout);
    }

    // Any options passed in as --something are going to be reported as errors
    arguments.reportRemainingOptionsAsUnrecognized();
    if (arguments.errors())
    {
        arguments.writeErrorMessages(std::cout);
        return 1;
    }

    // Create a series object using the vipreDICOM::SeriesBuilder API. NOTE: if more than one unique series
    // is found, then only the first series is going to be displayed
    osg::ref_ptr<vipreDICOM::Series> series = createSeriesList(arguments, useFiles, useDirectories, recursive, validate);
    if (!series.valid())
    {
        vipreLogERROR(examplePrefix) << "ERROR: could not find a valid series." << std::endl;
        return 1;
    }

    // Create a switch of textured quads from the series
    osg::ref_ptr<osg::Switch> seriesSwitch = createSeriesSwitch(series, normalize);

	// Create the Qt application
	QApplication app(argc, argv);

    // Set the logging levels for both vipre and osg
    vipre::Log::instance()->setLogLevel(vipre::INFO_LVL);
    osg::setNotifyLevel(osg::FATAL);

	// Initialize the main window controller
	MainWindowController controller(seriesSwitch);
	controller.showWindow();

	// Start up the application run loop
	return app.exec();
}
