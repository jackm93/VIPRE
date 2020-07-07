//
//  main.cpp
//  viprePrerenderPass
//
//  Created by Christian Noon on 12/3/11.
//  Copyright (c) 2011 Christian Noon. All rights reserved.
//

#include <osg/ArgumentParser>
#include <osg/Camera>
#include <osg/CullFace>
#include <osg/Notify>
#include <osg/ref_ptr>
#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <vipre/Log.hpp>
#include <vipre/NotificationCenter.hpp>
#include <vipre/ShaderHandler.hpp>
#include <vipre/String.hpp>
#include <vipre/TexturedCuboid.hpp>
#include <vipre/TrackballManipulator.hpp>
#include <vipreDICOM/SeriesBuilder.hpp>

#define VIEWER_WIDTH			1200
#define VIEWER_HEIGHT			800

#define examplePrefix           "[viprePrerenderPass] "

/** Handles all key events coming from osg. */
class KeyEventHandler : public osgGA::GUIEventHandler
{
public:

    /** Constructor. */
    KeyEventHandler(osg::ref_ptr<vipre::TexturedCuboid> cuboid, osg::ref_ptr<osg::Group> cuboidGroup) :
        _cuboid(cuboid),
        _cuboidGroup(cuboidGroup)
    {
        _currentPresetView = 0;
        _presetViews.push_back("Inferior");
        _presetViews.push_back("Superior");
        _presetViews.push_back("Left");
        _presetViews.push_back("Right");
        _presetViews.push_back("Anterior");
        _presetViews.push_back("Posterior");
        _presetViews.push_back("Anterior_Left_Inferior");
        _presetViews.push_back("Anterior_Right_Inferior");
        _presetViews.push_back("Anterior_Left_Superior");
        _presetViews.push_back("Anterior_Right_Superior");
        _presetViews.push_back("Posterior_Left_Inferior");
        _presetViews.push_back("Posterior_Right_Inferior");
        _presetViews.push_back("Posterior_Left_Superior");
        _presetViews.push_back("Posterior_Right_Superior");
        _minIntensity = 0.38f;
        _maxIntensity = 0.54f;
        _intensityDelta = 0.005f;
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
                if (ea.getKey() == 'n')
                {
                    ++_currentPresetView;
                    if (_currentPresetView == _presetViews.size())
                    {
                        _currentPresetView = 0;
                    }
                    vipre::String preset_view = _presetViews.at(_currentPresetView);
                    vipre::NotificationCenter::instance()->postNotificationWithObject("SetTrackballManipulatorPresetView", preset_view);
                }
                else if (ea.getKey() == 'p')
                {
                    --_currentPresetView;
                    if (_currentPresetView < 0)
                    {
                        _currentPresetView = _presetViews.size() - 1;
                    }
                    vipre::String preset_view = _presetViews.at(_currentPresetView);
                    vipre::NotificationCenter::instance()->postNotificationWithObject("SetTrackballManipulatorPresetView", preset_view);
                }
                else if (ea.getKey() == 'c')
                {
                    static int culling_counter = 0;
                    static osg::ref_ptr<osg::CullFace> cull_face = new osg::CullFace();

                    if (culling_counter == 0)
                    {
                        vipreLogINFO(examplePrefix) << "Enabling front-face culling" << std::endl;
                        cull_face->setMode(osg::CullFace::FRONT);
                        _cuboidGroup->getOrCreateStateSet()->setAttributeAndModes(cull_face, osg::StateAttribute::ON);
                    }
                    else if (culling_counter == 1)
                    {
                        vipreLogINFO(examplePrefix) << "Enabling back-face culling" << std::endl;
                        cull_face->setMode(osg::CullFace::BACK);
                        _cuboidGroup->getOrCreateStateSet()->setAttributeAndModes(cull_face, osg::StateAttribute::ON);
                    }
                    else
                    {
                        vipreLogINFO(examplePrefix) << "Disabling all culling" << std::endl;
                        _cuboidGroup->getOrCreateStateSet()->setAttributeAndModes(cull_face, osg::StateAttribute::OFF);
                    }

                    // Update the counter
                    culling_counter = culling_counter == 2 ? 0 : ++culling_counter;
                }
                else if (ea.getKey() == 'r')
                {
                    vipreLogINFO(examplePrefix) << "Reloading all shaders" << std::endl;
                    vipre::ShaderHandler::instance()->reloadShaders();
                }
                else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Up || ea.getKey() == osgGA::GUIEventAdapter::KEY_Down)
                {
                    // Raise and lower the min intensity with UP/DOWN
                    if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Up)
                    {
                        _minIntensity += _intensityDelta;
                        if (_minIntensity == _maxIntensity)
                        {
                            _minIntensity -= _intensityDelta;
                        }
                    }
                    else
                    {
                        _minIntensity -= _intensityDelta;
                        if (_minIntensity < 0.0f)
                        {
                            _minIntensity = 0.0f;
                        }
                    }

                    _cuboid->setMinIntensityUniform(_minIntensity);
                    vipreLogDEBUG(examplePrefix) << "New intensities: (" << _minIntensity << ", " << _maxIntensity << ")" << std::endl;
                }
                else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Left || ea.getKey() == osgGA::GUIEventAdapter::KEY_Right)
                {
                    // Raise and lower the max intensity with LEFT/RIGHT
                    if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Right)
                    {
                        _maxIntensity += _intensityDelta;
                        if (_maxIntensity > 1.0f)
                        {
                            _maxIntensity = 1.0f;
                        }
                    }
                    else
                    {
                        _maxIntensity -= _intensityDelta;
                        if (_maxIntensity <= _minIntensity)
                        {
                            _maxIntensity += _intensityDelta;
                        }
                    }

                    _cuboid->setMaxIntensityUniform(_maxIntensity);
                    vipreLogDEBUG(examplePrefix) << "New intensities: (" << _minIntensity << ", " << _maxIntensity << ")" << std::endl;
                }
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
    }

protected:

    /** Destructor. */
    ~KeyEventHandler()
    {
        ;
    }

    /** Instance member variables. */
    osg::ref_ptr<vipre::TexturedCuboid> _cuboid;
    osg::ref_ptr<osg::Group> _cuboidGroup;
    osg::ref_ptr<osg::CullFace> _backFaceCull;
    osg::ref_ptr<osg::CullFace> _frontFaceCull;
    std::vector<vipre::String> _presetViews;
    int _currentPresetView;
    float _minIntensity;
    float _maxIntensity;
    float _intensityDelta;
};

/** Callback for the main camera pre-draw event. */
class CameraDrawCallback : public osg::Camera::DrawCallback
{
public:

    /** Constructor. */
    CameraDrawCallback(osg::ref_ptr<vipre::TexturedCuboid> cuboid) :
        _cuboid(cuboid)
    {
        ;
    }

    /** Override the callback. */
    virtual void operator()(const osg::Camera& camera) const
    {
        // Compute the view to world matrix
        osg::Matrix camera_matrix;
        camera_matrix.postMult(camera.getViewMatrix());
        camera_matrix.postMult(camera.getProjectionMatrix());
        camera_matrix.postMult(camera.getViewport()->computeWindowMatrix());
        osg::Matrix view_to_world_matrix;
        view_to_world_matrix.invert(camera_matrix);
        _cuboid->setViewToWorldMatrixUniform(view_to_world_matrix);

        // Update the viewport settings in the shader
        const osg::Viewport* viewport = camera.getViewport();
        _cuboid->setWindowDimensionsUniform(osg::Vec2d(viewport->width(), viewport->height()));
    }

protected:

    /** Destructor. */
    ~CameraDrawCallback()
    {
        ;
    }

    /** Instance member variables. */
    osg::ref_ptr<vipre::TexturedCuboid> _cuboid;
};

class PreRenderCamera : public osg::Camera
{
public:

    /** Constructor. */
	PreRenderCamera(osg::ref_ptr<osg::Texture2D> texture) : osg::Camera()
    {
        // Set to be a prerender camera
        setRenderOrder(osg::Camera::PRE_RENDER);

        // Set a custom viewport
        int width = texture->getTextureWidth();
        int height = texture->getTextureHeight();
        setViewport(0, 0, width, height);

        // Don't clear any of the buffers each time
        setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        setClearColor(osg::Vec4d(0.0f, 0.0f, 0.0f, 1.0f));

        // Tell the camera to use OpenGL frame buffer object where supported
        setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);

        // Attach the color buffer render to the texture
        attach(osg::Camera::COLOR_BUFFER0, texture.get());

        // Cull out the front faces
        osg::ref_ptr<osg::CullFace> cull_face = new osg::CullFace();
        cull_face->setMode(osg::CullFace::FRONT);
        getOrCreateStateSet()->setAttributeAndModes(cull_face, osg::StateAttribute::ON);
    }

protected:

	/** Destructor. */
	~PreRenderCamera()
    {
        ;
    }
};

/** Returns the first unique vipreDICOM::Series it finds using the vipreDICOM::SeriesBuilder. */
osg::ref_ptr<vipreDICOM::Series> createSeriesList(vipre::String datasetPath, bool recursive)
{
    // User output
    if (recursive)
    {
        vipreLogINFO(examplePrefix) << "Building unique series list recursively from " << datasetPath << "\n" << std::endl;
    }
    else
    {
        vipreLogINFO(examplePrefix) << "Building unique series list from " << datasetPath << "\n" << std::endl;
    }

    // Use the SeriesBuilder to build a unique series list
    vipreDICOM::SeriesList uniqueSeriesList = vipreDICOM::SeriesBuilder::buildSeriesListFromDirectoryPath(datasetPath, recursive, true);

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
    traits->windowName = "viprePrerenderPass";

    // Create the Graphics Context
    osg::ref_ptr<osg::GraphicsContext> graphicsContext = osg::GraphicsContext::createGraphicsContext(traits.get());

    // Create the osgViewer instance
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer();
	viewer->getCamera()->setGraphicsContext(graphicsContext);
	viewer->getCamera()->setViewport(new osg::Viewport(0, 0, traits->width, traits->height));
    viewer->setThreadingModel(osgViewer::Viewer::SingleThreaded);

    // Set the projection matrix so we render accurately sized objects with a 30 degree field of view
    double aspectRatio = float(VIEWER_WIDTH) / float(VIEWER_HEIGHT);
    float near = 0.01f;
    float far = 2000.0f;
    viewer->getCamera()->setProjectionMatrixAsPerspective(30.0f, aspectRatio, near, far);

    // Add stats
    viewer->addEventHandler(new osgViewer::StatsHandler());

    // Add a trackball manipulator
    osg::ref_ptr<vipre::TrackballManipulator> manipulator = new vipre::TrackballManipulator();
    manipulator->setCamera(viewer->getCamera());
    viewer->setCameraManipulator(manipulator.get());

    // Set the max frame rate on the viewer
    viewer->setRunMaxFrameRate(400.0f);

	return viewer;
}

/** TODO. */
osg::ref_ptr<vipre::TexturedCuboid> createTexturedCuboidFromSeries(osg::ref_ptr<vipreDICOM::Series> series, bool normalize)
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

    // Get the raw voxel data
    osg::ref_ptr<vipreDICOM::VoxelData> voxelData = series->getVoxelData();
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

    // Create the textured cuboid
    osg::Vec3d voxelDimensions = series->getDimensions().toVec3d();
    osg::Vec3d voxelSpacing = series->getVoxelSpacing().toVec3d();
    osg::ref_ptr<vipre::TexturedCuboid> cuboid = new vipre::TexturedCuboid();
    cuboid->build(voxelDimensions, voxelSpacing, voxels, voxelChannelType, voxelDataType);

    return cuboid;
}

void buildScenegraph(osg::ref_ptr<osg::Group> root, osg::ref_ptr<osg::Group> cuboidGroup, osg::ref_ptr<vipre::TexturedCuboid> cuboid)
{
    // Create the texture for storing the back face positions
    osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
	texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::NEAREST);
	texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::NEAREST);
    texture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
    texture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
    texture->setInternalFormat(GL_RGBA32F_ARB);
    texture->setSourceFormat(GL_RGBA);
    texture->setSourceType(GL_FLOAT);
	texture->setTextureSize(VIEWER_WIDTH, VIEWER_HEIGHT);

    // Create the pre-render camera
    osg::ref_ptr<PreRenderCamera> preRenderCamera = new PreRenderCamera(texture);
    preRenderCamera->addChild(cuboid.get());

    // Create a post-render camera quad
    osg::ref_ptr<osg::Camera> postRenderCamera = new osg::Camera();
    {
        postRenderCamera->setProjectionMatrix(osg::Matrix::ortho2D(0, 1000, 0, 1000));
        postRenderCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
        postRenderCamera->setViewMatrix(osg::Matrix::identity());
        postRenderCamera->setRenderOrder(osg::Camera::POST_RENDER);
        postRenderCamera->setClearMask(GL_DEPTH_BUFFER_BIT);

        // Create a quad to pump the framebuffer texture onto
        osg::ref_ptr<osg::Geode> geode = new osg::Geode();
        osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();
        geode->addDrawable(geometry.get());
        postRenderCamera->addChild(geode.get());

        osg::ref_ptr<osg::Vec3dArray> vertices = new osg::Vec3dArray();
        vertices->push_back(osg::Vec3d(20.0f, 20.0f, 0.0f));
        vertices->push_back(osg::Vec3d(280.0f, 20.0f, 0.0f));
        vertices->push_back(osg::Vec3d(280.0f, 280.0f, 0.0f));
        vertices->push_back(osg::Vec3d(20.0f, 280.0f, 0.0f));
        geometry->setVertexArray(vertices.get());
        geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, vertices->size()));

        osg::ref_ptr<osg::Vec2dArray> texCoords = new osg::Vec2dArray();
        texCoords->push_back(osg::Vec2d(0.0f, 0.0f));
        texCoords->push_back(osg::Vec2d(1.0f, 0.0f));
        texCoords->push_back(osg::Vec2d(1.0f, 1.0f));
        texCoords->push_back(osg::Vec2d(0.0f, 1.0f));
        geometry->setTexCoordArray(0, texCoords);

        postRenderCamera->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture.get());
        postRenderCamera->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    }

    // Set up the scenegraph structure
    root->addChild(preRenderCamera.get());
    root->addChild(cuboidGroup.get());
    root->addChild(postRenderCamera.get());

    // Add the shaders to all the nodes
    vipre::ShaderHandler::instance()->addShadersToNode("prerendered_backfaces", preRenderCamera.get());
    vipre::ShaderHandler::instance()->addShadersToNode("composite_raycasting", cuboidGroup.get());

    // Add the cuboid dimensions uniform to the prerendered backfaces shader
    osg::Vec3d cuboidDimensions = cuboid->getCuboidDimensions();
    osg::ref_ptr<osg::Uniform> uniform = new osg::Uniform("cuboidDimensions", cuboidDimensions);
    cuboidGroup->getOrCreateStateSet()->addUniform(uniform.get());

    // Set the backfaces texture on the cuboid
    cuboid->setBackfacesTextureUniform(texture);
}

/**
 * TODO - Need a good description of this once I am almost finished.
 */
int main(int argc, char **argv)
{
    // Set the logging levels for both vipre and osg
    vipre::Log::instance()->setLogLevel(vipre::DEBUG_LVL);
    osg::setNotifyLevel(osg::FATAL);
//    osg::setNotifyLevel(osg::INFO);

    // Use an ArgumentParser object to manage the program arguments.
    osg::ArgumentParser arguments(&argc, argv);
    osg::ApplicationUsage* appUsage = arguments.getApplicationUsage();

    // Set up the usage document, in case we need to print out how to use this program.
    vipre::String description(vipre::String() << osgDB::getSimpleFileName(arguments.getApplicationName()) << " TODO");
    appUsage->setDescription(description);
    appUsage->setCommandLineUsage(osgDB::getSimpleFileName(arguments.getApplicationName()) + " [options] directory");
    appUsage->addCommandLineOption("--dataset <path>", "The directory of the dataset to load");
    appUsage->addCommandLineOption("--recursive", "Searches directories under the dataset path");
    appUsage->addCommandLineOption("--normalize", "Normalizes the dataset to the unsigned version of the current data type");
    appUsage->addCommandLineOption("-h or --help", "Display this information");

    // If user request help write it out to std::cout
    if (arguments.read("-h") || arguments.read("--help") || arguments.argc() < 2)
    {
        appUsage->write(std::cout);
        return 1;
    }

    // Extract the command line arguments
    vipre::String dataset;
    bool normalize = false;
    bool recursive = false;
    while (arguments.read("--dataset", dataset))
    {
        ;
    }
    while (arguments.read("--recursive"))
    {
        recursive = true;
    }
    while (arguments.read("--normalize"))
    {
        normalize = true;
    }

    // Make sure they chose a dataset
    if (dataset.empty())
    {
        vipreLogERROR(examplePrefix) << "Usage Error: you have to specify a dataset using the --dataset argument.\n" << std::endl;
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
    osg::ref_ptr<vipreDICOM::Series> series = createSeriesList(dataset, recursive);
    if (!series.valid())
    {
        vipreLogERROR(examplePrefix) << "ERROR: could not find a valid series." << std::endl;
        return 1;
    }

    // Create the osg viewer instance
    osg::ref_ptr<osgViewer::Viewer> viewer = createOsgViewer();

    // Set up the scenegraph structure
    osg::ref_ptr<osg::Group> root = new osg::Group();
    osg::ref_ptr<vipre::TexturedCuboid> cuboid = createTexturedCuboidFromSeries(series, normalize);
    osg::ref_ptr<osg::Group> cuboidGroup = new osg::Group();
    cuboidGroup->addChild(cuboid.get());
    buildScenegraph(root, cuboidGroup, cuboid);

    // Set up a pre-draw callback on the camera so we can set the view-to-world matrix uniform on the cuboid
    viewer->getCamera()->setPreDrawCallback(new CameraDrawCallback(cuboid));

    // Create a key event handler to process key events
    viewer->addEventHandler(new KeyEventHandler(cuboid, cuboidGroup));

    // Add the root node to the viewer
    viewer->setSceneData(root.get());

    // Start up the rendering
    return viewer->run();
}
