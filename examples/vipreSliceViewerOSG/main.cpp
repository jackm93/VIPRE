//
//  main.cpp
//  vipreSliceViewerOSG
//
//  Created by Christian Noon on 12/3/11.
//  Copyright (c) 2011 Christian Noon. All rights reserved.
//

#include <osg/ArgumentParser>
#include <osg/Notify>
#include <osg/ref_ptr>
#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <vipre/Log.hpp>
#include <vipre/String.hpp>
#include <vipre/TexturedQuad.hpp>
#include <vipre/TrackballManipulator.hpp>
#include <vipreDICOM/SeriesBuilder.hpp>

#define VIEWER_WIDTH			1200
#define VIEWER_HEIGHT			800

#define examplePrefix           "[vipreSliceViewerOSG] "

/**
 * Handles all key events for the slice viewer. The 'n' key flips to the next slice while
 * the 'p' key flips back to the previous slice.
 */
class KeyEventHandler : public osgGA::GUIEventHandler
{
public:

    /** Constructor. */
    KeyEventHandler(osg::ref_ptr<osg::Switch> sliceSwitch, osg::ref_ptr<osg::Switch> infoSwitch, osg::ref_ptr<osgText::Text> sliceCountText) :
		_sliceSwitch(sliceSwitch),
		_infoSwitch(infoSwitch),
		_sliceCountText(sliceCountText),
		_infoSwitchVisible(false),
        _visibleSliceIndex(0)
    {
        ;
    }

    /** Handles specific key presses. */
    virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, osg::Object* /*object*/, osg::NodeVisitor* /*nv*/)
    {
        // Ignore event if already handled
        if (ea.getHandled())
        {
            return false;
        }

        // Make sure we can access the osgViewer
        osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
        if (!viewer)
        {
            return false;
        }

        switch (ea.getEventType())
        {
            case osgGA::GUIEventAdapter::KEYDOWN:
            {
				unsigned int numSlices = _sliceSwitch->getNumChildren();
                if (ea.getKey() == 'n')
                {
                    if (_visibleSliceIndex < numSlices-1)
                    {
                        ++_visibleSliceIndex;
                        _sliceSwitch->setSingleChildOn(_visibleSliceIndex);

						std::ostringstream outString;
						outString << "Slice " << (_visibleSliceIndex + 1) << " of " << numSlices;
						_sliceCountText->setText(outString.str());
                    }
                }
                else if (ea.getKey() == 'p')
                {
                    if (_visibleSliceIndex > 0)
                    {
                        --_visibleSliceIndex;
                        _sliceSwitch->setSingleChildOn(_visibleSliceIndex);

						std::ostringstream outString;
						outString << "Slice " << (_visibleSliceIndex + 1) << " of " << numSlices;
						_sliceCountText->setText(outString.str());
                    }
                }
                else if (ea.getKey() == 'i')
                {
                    if (_infoSwitchVisible == true)
					{
						_infoSwitch->setAllChildrenOff();
					}
					else
					{
						_infoSwitch->setAllChildrenOn();
					}
					_infoSwitchVisible = !_infoSwitchVisible;
                }
                break;
            }
            default:
                break;
        }

        return false;
    }

    /** Add the key bindings to the app usage. */
    void getUsage(osg::ApplicationUsage& usage) const
    {
        usage.addKeyboardMouseBinding("n", "next slice");
        usage.addKeyboardMouseBinding("p", "previous slice");
		usage.addKeyboardMouseBinding("i", "show DICOM info");
    }

protected:

    /** Destructor. */
    ~KeyEventHandler()
    {
        ;
    }

    /** Instance member variables. */
    osg::ref_ptr<osg::Switch> _sliceSwitch;
	osg::ref_ptr<osg::Switch> _infoSwitch;
	osg::ref_ptr<osgText::Text> _sliceCountText;
	bool _infoSwitchVisible;
    unsigned int _visibleSliceIndex;
};

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

/** Sets up a basic osgViewer::Viewer instance. */
osg::ref_ptr<osgViewer::Viewer> createOsgViewer()
{
	// Create our graphics context directly
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits();

    // Setup the traits parameters
	traits->x = 100;
	traits->y = 100;
	traits->width = VIEWER_WIDTH;
	traits->height = VIEWER_HEIGHT;
	traits->windowDecoration = true;
	traits->doubleBuffer = true;
	traits->sharedContext = 0;
    traits->windowName = "vipreSliceViewerOSG";

    // Create the Graphics Context
    osg::ref_ptr<osg::GraphicsContext> graphicsContext = osg::GraphicsContext::createGraphicsContext(traits.get());

    // Create the osgViewer instance
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer();
	viewer->getCamera()->setGraphicsContext(graphicsContext);
	viewer->getCamera()->setViewport(new osg::Viewport(0, 0, traits->width, traits->height));
    viewer->setThreadingModel(osgViewer::Viewer::SingleThreaded);

    // Add stats
    viewer->addEventHandler(new osgViewer::StatsHandler());

    // Add a trackball manipulator
    viewer->setCameraManipulator(new vipre::TrackballManipulator());

	return viewer;
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

osg::ref_ptr<osg::Switch> createSeriesText(osg::ref_ptr<vipreDICOM::Series> series)
{
	osg::ref_ptr<osg::Switch> textSwitch = new osg::Switch();
	textSwitch->setAllChildrenOff();

	// Safety check to make sure we have some slices
    if (series->getSlices().size() == 0)
    {
        vipreLogWARNING(examplePrefix) << "The series does not contain any valid slices" << std::endl;
        return textSwitch;
    }
	osg::ref_ptr<vipreDICOM::Slice> firstSlice = series->getSlices().front();

	osg::ref_ptr<osg::Geode> textGeode = new osg::Geode();
	osg::ref_ptr<osg::StateSet> stateSet = textGeode->getOrCreateStateSet();
	stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

	osg::ref_ptr<osgText::Text> text = new osgText::Text;
	osg::ref_ptr<osgText::Font> font = osgText::readFontFile("fonts/VeraMono.ttf");
	text->setFont(font);
	text->setColor(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	text->setCharacterSize(14.0f);
	text->setFontResolution(40, 40);
	text->setLayout(osgText::Text::LEFT_TO_RIGHT);
	text->setAlignment(osgText::Text::LEFT_BASE_LINE);
	text->setPosition(osg::Vec3(20.0f, VIEWER_HEIGHT - 40.0f, 0.0f));

	std::ostringstream outString;

	outString << "(R) Study Instance ID:                    " << firstSlice->getStudyInstanceID() << std::endl;
	outString << "(R) Study ID:                             " << firstSlice->getStudyID() << std::endl;
	outString << "(N) Study Description:                    " << firstSlice->getStudyDescription() << std::endl;
	outString << "(N) Study Date:                           " << firstSlice->getStudyDate() << std::endl;
	outString << "(R) Series Instance ID:                   " << firstSlice->getSeriesID() << std::endl;
	outString << "(R) Series Number:                        " << firstSlice->getSeriesNumber() << std::endl;
	outString << "(N) Series Description:                   " << firstSlice->getSeriesDescription() << std::endl;
	outString << "(N) Series Date:                          " << firstSlice->getSeriesDate() << std::endl;
	outString << "(N) Patient ID:                           " << firstSlice->getPatientID() << std::endl;
	outString << "(N) Patient Name:                         " << firstSlice->getPatientName() << std::endl;
	outString << "(N) Patient Age:                          " << firstSlice->getPatientAge() << std::endl;
	outString << "(N) Patient Date of Birth:                " << firstSlice->getPatientDateOfBirth() << std::endl;
	outString << "(N) Patient Sex:                          " << firstSlice->getPatientSex() << std::endl;
	outString << "(N) Patient Weight:                       " << firstSlice->getPatientWeight() << std::endl;
	outString << "(N) Patient Additional History:           " << firstSlice->getPatientAdditionalHistory() << std::endl;
	outString << "(N) Institution Name:                     " << firstSlice->getInstitution() << std::endl;
	outString << "(N) Modality:                             " << firstSlice->getModality() << std::endl;
	outString << "(N) Image Type:                           " << firstSlice->getImageType() << std::endl;
	outString << "(N) Contrast:                             " << firstSlice->getContrastType() << std::endl;
	outString << "(N) Manufacturer:                         " << firstSlice->getManufacturer() << std::endl;
    osg::Vec3f leftAxis = firstSlice->getImageOrientationPatient().leftAxis;
    osg::Vec3f anteriorAxis = firstSlice->getImageOrientationPatient().anteriorAxis;
	outString << "(R) Image Orientation Patient:            (" << leftAxis[0] << ", " << leftAxis[1] << ", " << leftAxis[2]
        << ") (" << anteriorAxis[0] << ", " << anteriorAxis[1] << ", " << anteriorAxis[2] << ")" << std::endl;
	outString << "(R) Voxel Spacing (width, height, depth): (" << series->getVoxelSpacing().width << ", "
        << series->getVoxelSpacing().height << ", " << series->getVoxelSpacing().depth << ")" << std::endl;
	outString << "(R) Dimensions (width, height, count)     (" << series->getDimensions().width << ", "
        << series->getDimensions().height << ", " << series->getDimensions().depth << ")" << std::endl;
	outString << "(R) Number of Slices:                     " << series->getSlices().size() << std::endl;

	text->setText(outString.str());

	textGeode->addDrawable(text);

	textSwitch->addChild(textGeode.get());

	return textSwitch;
}

osg::ref_ptr<osg::Geode> createSliceCountGeode(osg::ref_ptr<osgText::Text> sliceCountText)
{
	osg::ref_ptr<osg::Geode> textGeode = new osg::Geode();
	osg::ref_ptr<osg::StateSet> stateSet = textGeode->getOrCreateStateSet();
	stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

	osg::ref_ptr<osgText::Font> font = osgText::readFontFile("fonts/VeraMono.ttf");
	sliceCountText->setFont(font);
	sliceCountText->setColor(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	sliceCountText->setCharacterSize(14.0f);
	sliceCountText->setFontResolution(40, 40);
	sliceCountText->setLayout(osgText::Text::LEFT_TO_RIGHT);
	sliceCountText->setAlignment(osgText::Text::RIGHT_BASE_LINE);
	sliceCountText->setPosition(osg::Vec3(VIEWER_WIDTH - 40.0f, VIEWER_HEIGHT - 40.0f, 0.0f));

	textGeode->addDrawable(sliceCountText);

	return textGeode;
}

osg::ref_ptr<osg::Camera> createHudCamera()
{
	osg::ref_ptr<osg::Camera> camera = new osg::Camera;

	camera->setProjectionMatrix(osg::Matrix::ortho2D(0, VIEWER_WIDTH, 0, VIEWER_HEIGHT));
	camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	camera->setViewMatrix(osg::Matrix::identity());
	camera->setClearMask(GL_DEPTH_BUFFER_BIT);
	camera->setRenderOrder(osg::Camera::POST_RENDER);
	camera->setAllowEventFocus(false);

	return camera;
}

/**
 * This example demonstrates how to create vipre::TexturedQuads from vipreDICOM::Slices.
 *
 * There are a series of steps involved with visualizing the data. Here goes:
 * 1) Create a list of files based on the series of command line arguments
 * 2) Use the vipreDICOM::SeriesBuilder API to find the unique series objects
 *     - NOTE: This example only uses the first one it finds if it finds more than one
 * 3) The vipreDICOM::Series voxel data is extract and normalized (if specified)
 * 4) A vipre::TexturedQuad is created for each vipreDICOM::Slice in the vipreDICOM::Series
 * 5) All the vipre::TexturedQuads are added to an osg::Switch
 * 6) The application is launched showing the first slice
 * 7) You can switch between slices using the 'n' and 'p' keys (next/previous)
 * 8) You can toggle the series info on/off using the 'i' key
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
        vipreLogERROR(examplePrefix) << "Usage Error: you cannot launch this application without specifying "
            "either --files or --directories.\n" << std::endl;
        appUsage->write(std::cout);
    }

    // Make sure they don't launch with both files and directories active
    if (useFiles && useDirectories)
    {
        vipreLogERROR(examplePrefix) << "Usage Error: you cannot launch this application with both --files "
            "and --directories.\n" << std::endl;
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

    // Create the osg viewer instance
    osg::ref_ptr<osgViewer::Viewer> viewer = createOsgViewer();

    // Set up the scenegraph structure
    osg::ref_ptr<osg::Group> root = new osg::Group();
    osg::ref_ptr<osg::Switch> seriesSwitch = createSeriesSwitch(series, normalize);
    root->addChild(seriesSwitch.get());

	// Set up the hud camera
	osg::ref_ptr<osg::Camera> hudCamera = createHudCamera();

	osgViewer::Viewer::Windows windows;
	viewer->getWindows(windows);
	hudCamera->setGraphicsContext(windows[0]);
	hudCamera->setViewport(0,0,windows[0]->getTraits()->width, windows[0]->getTraits()->height);

	// Set up the text for series information
	osg::ref_ptr<osg::Switch> textSwitch = createSeriesText(series);
	hudCamera->addChild(textSwitch.get());
	viewer->addSlave(hudCamera, false);

	// Set up text for the current slice display
	osg::ref_ptr<osgText::Text> sliceText = new osgText::Text();

	std::ostringstream outString;
	outString << "Slice 1 of " << seriesSwitch->getNumChildren();
	sliceText->setText(outString.str());

	osg::ref_ptr<osg::Geode> sliceCountGeode = createSliceCountGeode(sliceText);
	hudCamera->addChild(sliceCountGeode.get());

    // Create a key event handler to process key events
    viewer->addEventHandler(new KeyEventHandler(seriesSwitch, textSwitch, sliceText));

    // Add the root node to the viewer
    viewer->setSceneData(root.get());

    // Start up the rendering
    return viewer->run();
}
