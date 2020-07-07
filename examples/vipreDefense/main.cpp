//
//  main.cpp
//  vipreDefense
//
//  Created by Christian Noon on 2/21/12.
//  Copyright (c) 2012 Christian Noon. All rights reserved.
//
#include "windows.h"
#include <iomanip>

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

#define examplePrefix           "[vipreDefense] "

namespace osg {
    typedef std::vector<osg::ref_ptr<osgText::Text> > TextList;
}

/** Callback for the main camera pre-draw event. */
class CameraDrawCallback : public osg::Camera::DrawCallback
{
public:

    /** Constructor. */
    CameraDrawCallback(osg::ref_ptr<vipre::TexturedCuboid> cuboid, osg::TextList textList, osg::ref_ptr<osgViewer::Viewer> viewer,
                       osg::ref_ptr<osg::Light> light) :
        _cuboid(cuboid),
        _textList(textList),
        _viewer(viewer),
        _tickLastUpdated(0),
        _minIntensity(0.38f),
        _maxIntensity(0.54f),
        _octreeEnabled(false),
        _lightingEnabled(false),
        _prerenderPassEnabled(false),
        _prerenderHudEnabled(false),
        _colorTable("Muscle and Bone"),
        _opacityTable("Linear Smooth"),
        _culling("OFF"),
        _light(light),
        _lightPosition(light->getPosition()),
        _previousCameraViewMatrix(new osg::Matrix(osg::Matrix::identity()))
    {
        ;
    }

    /** Override the callback. */
    virtual void operator()(const osg::Camera& camera) const
    {
        // Compute the view to world matrix
        osg::Matrix camera_matrix;
		osg::Matrix projectionMatrix = camera.getProjectionMatrix();
        camera_matrix.postMult(camera.getViewMatrix());
        camera_matrix.postMult(projectionMatrix);
        camera_matrix.postMult(camera.getViewport()->computeWindowMatrix());
        osg::Matrix view_to_world_matrix;
        view_to_world_matrix.invert(camera_matrix);
        _cuboid->setViewToWorldMatrixUniform(view_to_world_matrix);

		_cuboid->setProjectionMatrixUniform(projectionMatrix);
		
        // Update the viewport settings in the shader
        const osg::Viewport* viewport = camera.getViewport();
        _cuboid->setWindowDimensionsUniform(osg::Vec2d(viewport->width(), viewport->height()));

        // Check to see if the camera's view matrix has changed
        if (*_previousCameraViewMatrix != camera.getViewMatrix())
        {
            // Update the light position
            osg::Matrix inverse_view_matrix = camera.getInverseViewMatrix();
            osg::Quat rotation = inverse_view_matrix.getRotate();
            osg::Vec4d new_position = _lightPosition * osg::Matrix::rotate(rotation);
            _light->setPosition(new_position);
            _cuboid->setLightPositionUniform(osg::Vec3d(new_position[0], new_position[1], new_position[2]));

            // Update the previous matrix for the next round
            _previousCameraViewMatrix->set(camera.getViewMatrix());
        }

        // Update the hud
        osg::Timer_t tick = osg::Timer::instance()->tick();
        double delta = osg::Timer::instance()->delta_m(_tickLastUpdated, tick);
        if (delta > 50)
        {
            updateDynamicHud();
        }
    }

    void updateDynamicHud() const
    {
        static osg::Vec4d onColor(0.0f, 0.9f, 0.0f, 1.0f);
        static osg::Vec4d offColor(0.9f, 0.0f, 0.0f, 1.0f);
        static osg::Vec4d whiteColor(1.0f, 1.0f, 1.0f, 1.0f);

        // framerate
        {
            double fps;
            _viewer->getViewerStats()->getAveragedAttribute("Frame rate", fps, true);
            std::ostringstream ss;
            ss << std::setprecision(3) << fps << std::endl;
            _textList.at(0)->setText(ss.str());

            // update the color
            if (fps > 60.0f)
            {
                _textList.at(0)->setColor(onColor);
            }
            else if (fps < 20.0f)
            {
                _textList.at(0)->setColor(offColor);
            }
            else
            {
                double normalized = (fps - 20.0f) / 40.0f;
                osg::Vec4d deltaColor = onColor - offColor;
                osg::Vec4d blendColor = offColor + (deltaColor * normalized);
                _textList.at(0)->setColor(blendColor);
            }
        }

        // min/max intensity
        {
            char tmpText[256];
            sprintf(tmpText,"%4.3f, %4.3f", _minIntensity, _maxIntensity);
            _textList.at(1)->setText(tmpText);
        }

        // octree
        {
            vipre::String enabled = _octreeEnabled ? "ON" : "OFF";
            _textList.at(2)->setText(enabled);
            _octreeEnabled ? _textList.at(2)->setColor(onColor) : _textList.at(2)->setColor(offColor);
        }

        // lighting
        {
            vipre::String enabled = _lightingEnabled ? "ON" : "OFF";
            _textList.at(3)->setText(enabled);
            _lightingEnabled ? _textList.at(3)->setColor(onColor) : _textList.at(3)->setColor(offColor);
        }

        // prerenderPass
        {
            vipre::String enabled = _prerenderPassEnabled ? "ON" : "OFF";
            _textList.at(4)->setText(enabled);
            _prerenderPassEnabled ? _textList.at(4)->setColor(onColor) : _textList.at(4)->setColor(offColor);
        }

        // prerenderHud
        {
            vipre::String enabled = _prerenderHudEnabled ? "ON" : "OFF";
            _textList.at(5)->setText(enabled);
            _prerenderHudEnabled ? _textList.at(5)->setColor(onColor) : _textList.at(5)->setColor(offColor);
        }

        // colorTable
        {
            _textList.at(6)->setText(_colorTable);
        }

        // opacityTable
        {
            _textList.at(7)->setText(_opacityTable);
        }

        // culling
        {
            if (_culling.empty())
            {
                _textList.at(8)->setText("OFF");
                _textList.at(8)->setColor(offColor);
            }
            else
            {
                _textList.at(8)->setText(_culling);
                _textList.at(8)->setColor(onColor);
            }
        }
    }

    inline void setMinIntensity(double intensity) { _minIntensity = intensity; }
    inline void setMaxIntensity(double intensity) { _maxIntensity = intensity; }
    inline void setOctreeEnabled(bool enabled) { _octreeEnabled = enabled; }
    inline void setLightingEnabled(bool enabled) { _lightingEnabled = enabled; }
    inline void setPrerenderPassEnabled(bool enabled) { _prerenderPassEnabled = enabled; }
    inline void setPrerenderHudEnabled(bool enabled) { _prerenderHudEnabled = enabled; }
    inline void setColorTable(vipre::String colorTable) { _colorTable = colorTable; }
    inline void setOpacityTable(vipre::String opacityTable) { _opacityTable = opacityTable; }
    inline void setCulling(vipre::String culling) { _culling = culling; }

protected:

    /** Destructor. */
    ~CameraDrawCallback()
    {
        delete _previousCameraViewMatrix;
    }

    /** Instance member variables. */
    osg::ref_ptr<vipre::TexturedCuboid> _cuboid;
    osg::TextList _textList;
    osg::ref_ptr<osgViewer::Viewer> _viewer;
    mutable osg::Timer_t _tickLastUpdated;
    double _minIntensity;
    double _maxIntensity;
    bool _octreeEnabled;
    bool _lightingEnabled;
    bool _prerenderPassEnabled;
    bool _prerenderHudEnabled;
    vipre::String _colorTable;
    vipre::String _opacityTable;
    vipre::String _culling;
    osg::ref_ptr<osg::Light> _light;
    osg::Vec4d _lightPosition;
    osg::Matrix* _previousCameraViewMatrix;
};

class PreRenderCamera : public osg::Camera
{
public:
    
    /** Constructor. */
	PreRenderCamera() : osg::Camera()
    {
        // Set to be a prerender camera
        setRenderOrder(osg::Camera::PRE_RENDER);
        
        // Don't clear any of the buffers each time
        setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        setClearColor(osg::Vec4d(0.0f, 0.0f, 0.0f, 1.0f));
        
        // Cull out the front faces
        osg::ref_ptr<osg::CullFace> cull_face = new osg::CullFace();
        cull_face->setMode(osg::CullFace::FRONT);
        getOrCreateStateSet()->setAttributeAndModes(cull_face, osg::StateAttribute::ON);
    }
    
    void setTexture(osg::ref_ptr<osg::Texture2D> texture)
    {
        // Set a custom viewport
        int width = texture->getTextureWidth();
        int height = texture->getTextureHeight();
        setViewport(0, 0, width, height);
        
        // Tell the camera to use OpenGL frame buffer object where supported
        setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
        
        // Attach the color buffer render to the texture
        attach(osg::Camera::COLOR_BUFFER0, texture.get());
    }
    
protected:
    
	/** Destructor. */
	~PreRenderCamera()
    {
        ;
    }
};

/** Handles all key events coming from osg. */
class KeyEventHandler : public osgGA::GUIEventHandler
{
public:

    /** Constructor. */
    KeyEventHandler(osg::ref_ptr<vipre::TexturedCuboid> cuboid, osg::ref_ptr<osg::Group> cuboidGroup,
                    osg::ref_ptr<CameraDrawCallback> callback, osg::ref_ptr<PreRenderCamera> preRenderCamera,
                    osg::ref_ptr<osg::Camera> postRenderCamera) :
        _cuboid(cuboid),
        _cuboidGroup(cuboidGroup),
        _cameraDrawCallback(callback),
        _preRenderCamera(preRenderCamera),
        _postRenderCamera(postRenderCamera)
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

        _opacityTables.push_back("Linear Smooth");
        _opacityTables.push_back("Linear Sharp");

        _colorTables.push_back("Bone");
        _colorTables.push_back("Cardiac");
        _colorTables.push_back("GE");
        _colorTables.push_back("Grayscale");
        _colorTables.push_back("Muscle and Bone");
        _colorTables.push_back("NIH");
        _colorTables.push_back("Red Vessels");
        _colorTables.push_back("Stern");
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
                else if (ea.getKey() == 't')
                {
                    static unsigned int current_opacity_table_index = 0;
                    current_opacity_table_index = current_opacity_table_index == _opacityTables.size()-1 ? 0 : ++current_opacity_table_index;
                    _cuboid->setOpacityTable(_opacityTables.at(current_opacity_table_index));
                    _cameraDrawCallback->setOpacityTable(_opacityTables.at(current_opacity_table_index));
                }
                else if (ea.getKey() == 'c')
                {
                    static unsigned int current_color_table_index = 4;
                    current_color_table_index = current_color_table_index == _colorTables.size()-1 ? 0 : ++current_color_table_index;
                    _cuboid->setColorTable(_colorTables.at(current_color_table_index));
                    _cameraDrawCallback->setColorTable(_colorTables.at(current_color_table_index));
                }
                else if (ea.getKey() == 'a')
                {
                    static int culling_counter = 0;
                    static osg::ref_ptr<osg::CullFace> cull_face = new osg::CullFace();

                    if (culling_counter == 0)
                    {
                        vipreLogINFO(examplePrefix) << "Enabling front-face culling" << std::endl;
                        cull_face->setMode(osg::CullFace::FRONT);
                        _cuboidGroup->getOrCreateStateSet()->setAttributeAndModes(cull_face, osg::StateAttribute::ON);
                        _cameraDrawCallback->setCulling("Front-face");
                    }
                    else if (culling_counter == 1)
                    {
                        vipreLogINFO(examplePrefix) << "Enabling back-face culling" << std::endl;
                        cull_face->setMode(osg::CullFace::BACK);
                        _cuboidGroup->getOrCreateStateSet()->setAttributeAndModes(cull_face, osg::StateAttribute::ON);
                        _cameraDrawCallback->setCulling("Back-face");
                    }
                    else
                    {
                        vipreLogINFO(examplePrefix) << "Disabling all culling" << std::endl;
                        _cuboidGroup->getOrCreateStateSet()->setAttributeAndModes(cull_face, osg::StateAttribute::OFF);
                        _cameraDrawCallback->setCulling("");
                    }

                    // Update the counter
                    culling_counter = culling_counter == 2 ? 0 : ++culling_counter;
                }
                else if (ea.getKey() == 'r')
                {
                    vipreLogINFO(examplePrefix) << "Reloading all shaders" << std::endl;
                    vipre::ShaderHandler::instance()->reloadShaders();
                }
                else if (ea.getKey() == 'o')
                {
                    static bool octreeEnabled = false;
                    octreeEnabled = !octreeEnabled;
                    _cameraDrawCallback->setOctreeEnabled(octreeEnabled);
                    _cuboid->setOctreeRenderingEnabledUniform(octreeEnabled);
                }
                else if (ea.getKey() == 'l')
                {
                    static bool lightingEnabled = false;
                    lightingEnabled = !lightingEnabled;
                    _cameraDrawCallback->setLightingEnabled(lightingEnabled);
                    _cuboid->setLightingEnabledUniform(lightingEnabled);
                }
                else if (ea.getKey() == 'v')
                {
                    static bool prerenderPassEnabled = false;
                    prerenderPassEnabled = !prerenderPassEnabled;
                    _cameraDrawCallback->setPrerenderPassEnabled(prerenderPassEnabled);
                    _cuboid->setBackfaceTextureEnabledUniform(prerenderPassEnabled);
                    prerenderPassEnabled ? _preRenderCamera->setNodeMask(1) : _preRenderCamera->setNodeMask(0);
                }
                else if (ea.getKey() == 'h')
                {
                    static bool prerenderHudEnabled = false;
                    prerenderHudEnabled = !prerenderHudEnabled;
                    _cameraDrawCallback->setPrerenderHudEnabled(prerenderHudEnabled);
                    prerenderHudEnabled ? _postRenderCamera->setNodeMask(1) : _postRenderCamera->setNodeMask(0);
                }
                else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Left || ea.getKey() == osgGA::GUIEventAdapter::KEY_Right)
                {
                    // Raise and lower the min intensity with LEFT/RIGHT
                    if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Right)
                    {
                        _minIntensity += _intensityDelta;
                        if (_minIntensity >= _maxIntensity)
                        {
                            _minIntensity = _maxIntensity - _intensityDelta;
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

                    _cameraDrawCallback->setMinIntensity(_minIntensity);
                    _cuboid->setMinIntensityUniform(_minIntensity);
                    vipreLogDEBUG(examplePrefix) << "New intensities: (" << _minIntensity << ", " << _maxIntensity << ")" << std::endl;
                }
                else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Up || ea.getKey() == osgGA::GUIEventAdapter::KEY_Down)
                {
                    // Raise and lower the max intensity with UP/DOWN
                    if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Up)
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
                            _maxIntensity = _minIntensity + _intensityDelta;
                        }
                    }

                    _cameraDrawCallback->setMaxIntensity(_maxIntensity);
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
    osg::ref_ptr<CameraDrawCallback> _cameraDrawCallback;
    osg::ref_ptr<PreRenderCamera> _preRenderCamera;
    osg::ref_ptr<osg::Camera> _postRenderCamera;
    std::vector<vipre::String> _opacityTables;
    std::vector<vipre::String> _colorTables;
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
    traits->windowName = "vipreDefense";

    // Create the Graphics Context
    osg::ref_ptr<osg::GraphicsContext> graphicsContext = osg::GraphicsContext::createGraphicsContext(traits.get());

    // Create the osgViewer instance
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer();
	viewer->getCamera()->setGraphicsContext(graphicsContext);
	viewer->getCamera()->setViewport(new osg::Viewport(0, 0, traits->width, traits->height));
    viewer->setThreadingModel(osgViewer::Viewer::SingleThreaded);

    // Set the projection matrix so we render accurately sized objects with a 30 degree field of view
    double aspectRatio = float(VIEWER_WIDTH) / float(VIEWER_HEIGHT);
    float near1 = 0.01f;
    float far1 = 2000.0f;
    viewer->getCamera()->setProjectionMatrixAsPerspective(30.0f, aspectRatio, near1, far1);

    // Set the max frame rate on the viewer and collect the frame rate
    viewer->setRunMaxFrameRate(400.0f);
    viewer->getViewerStats()->collectStats("frame_rate", true);

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

osg::TextList buildDynamicHudGeode(double frameRate,
                                   bool octree,
                                   bool lighting,
                                   bool prerenderPass,
                                   bool prerenderHud,
                                   vipre::String colorTable,
                                   vipre::String opacityTable,
                                   double minIntensity,
                                   double maxIntensity,
                                   osg::ref_ptr<osg::Switch> parentSwitch)
{
    osg::ref_ptr<osg::Geode> dynamicHudGeode = new osg::Geode;
    parentSwitch->addChild(dynamicHudGeode.get());

    osg::TextList textList;
    osg::ref_ptr<osgText::Font> font = osgText::readFontFile("fonts/VeraMono.ttf");
    osg::Vec4d onColor(0.0f, 0.9f, 0.0f, 1.0f);
    osg::Vec4d offColor(0.9f, 0.0f, 0.0f, 1.0f);
    osg::Vec4d whiteColor(1.0f, 1.0f, 1.0f, 1.0f);
    float leftStartPosition(232.0f);

    // framerate
    {
        osg::ref_ptr<osgText::Text> text = new osgText::Text;
        textList.push_back(text);
        text->setFont(font);
        text->setColor(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
        text->setCharacterSize(16.0f);
        text->setFontResolution(40, 40);
        text->setLayout(osgText::Text::LEFT_TO_RIGHT);
        text->setAlignment(osgText::Text::LEFT_BASE_LINE);
        text->setPosition(osg::Vec3d(leftStartPosition, VIEWER_HEIGHT - 29.0f, 0.0f));

        // Set the frame rate precision and text
        std::ostringstream ss;
        ss << std::setprecision(4) << frameRate << std::endl;
        text->setText(ss.str());

        dynamicHudGeode->addDrawable(text.get());
    }

    // min/max intensity
    {
        osg::ref_ptr<osgText::Text> text = new osgText::Text;
        textList.push_back(text);
        text->setFont(font);
        text->setColor(whiteColor);
        text->setCharacterSize(16.0f);
        text->setFontResolution(40, 40);
        text->setLayout(osgText::Text::LEFT_TO_RIGHT);
        text->setAlignment(osgText::Text::LEFT_BASE_LINE);
        text->setPosition(osg::Vec3d(leftStartPosition, VIEWER_HEIGHT - 45.0f, 0.0f));

        // Set the min/max intensity precision and text
        std::ostringstream ss;
        ss << std::setprecision(4) << minIntensity << ", " << maxIntensity << std::endl;
        text->setText(ss.str());
        dynamicHudGeode->addDrawable(text.get());
    }

    // octree
    {
        osg::ref_ptr<osgText::Text> text = new osgText::Text;
        textList.push_back(text);
        text->setFont(font);
        octree ? text->setColor(onColor) : text->setColor(offColor);
        text->setCharacterSize(16.0f);
        text->setFontResolution(40, 40);
        text->setLayout(osgText::Text::LEFT_TO_RIGHT);
        text->setAlignment(osgText::Text::LEFT_BASE_LINE);
        text->setPosition(osg::Vec3d(leftStartPosition, VIEWER_HEIGHT - 61.0f, 0.0f));
        vipre::String octreeStr = octree ? "ON" : "OFF";
        text->setText(octreeStr);
        dynamicHudGeode->addDrawable(text.get());
    }

    // lighting
    {
        osg::ref_ptr<osgText::Text> text = new osgText::Text;
        textList.push_back(text);
        text->setFont(font);
        lighting ? text->setColor(onColor) : text->setColor(offColor);
        text->setCharacterSize(16.0f);
        text->setFontResolution(40, 40);
        text->setLayout(osgText::Text::LEFT_TO_RIGHT);
        text->setAlignment(osgText::Text::LEFT_BASE_LINE);
        text->setPosition(osg::Vec3d(leftStartPosition, VIEWER_HEIGHT - 77.0f, 0.0f));
        vipre::String lightingStr = lighting ? "ON" : "OFF";
        text->setText(lightingStr);
        dynamicHudGeode->addDrawable(text.get());
    }

    // prerenderPass
    {
        osg::ref_ptr<osgText::Text> text = new osgText::Text;
        textList.push_back(text);
        text->setFont(font);
        prerenderPass ? text->setColor(onColor) : text->setColor(offColor);
        text->setCharacterSize(16.0f);
        text->setFontResolution(40, 40);
        text->setLayout(osgText::Text::LEFT_TO_RIGHT);
        text->setAlignment(osgText::Text::LEFT_BASE_LINE);
        text->setPosition(osg::Vec3d(leftStartPosition, VIEWER_HEIGHT - 93.0f, 0.0f));
        vipre::String prerenderPassStr = prerenderPass ? "ON" : "OFF";
        text->setText(prerenderPassStr);
        dynamicHudGeode->addDrawable(text.get());
    }

    // prerenderHud
    {
        osg::ref_ptr<osgText::Text> text = new osgText::Text;
        textList.push_back(text);
        text->setFont(font);
        prerenderHud ? text->setColor(onColor) : text->setColor(offColor);
        text->setCharacterSize(16.0f);
        text->setFontResolution(40, 40);
        text->setLayout(osgText::Text::LEFT_TO_RIGHT);
        text->setAlignment(osgText::Text::LEFT_BASE_LINE);
        text->setPosition(osg::Vec3d(leftStartPosition, VIEWER_HEIGHT - 109.0f, 0.0f));
        vipre::String prerenderHudStr = prerenderHud ? "ON" : "OFF";
        text->setText(prerenderHudStr);
        dynamicHudGeode->addDrawable(text.get());
    }

    // colorTable
    {
        osg::ref_ptr<osgText::Text> text = new osgText::Text;
        textList.push_back(text);
        text->setFont(font);
        text->setColor(whiteColor);
        text->setCharacterSize(16.0f);
        text->setFontResolution(40, 40);
        text->setLayout(osgText::Text::LEFT_TO_RIGHT);
        text->setAlignment(osgText::Text::LEFT_BASE_LINE);
        text->setPosition(osg::Vec3d(leftStartPosition, VIEWER_HEIGHT - 125.0f, 0.0f));
        text->setText(colorTable);
        dynamicHudGeode->addDrawable(text.get());
    }

    // opacityTable
    {
        osg::ref_ptr<osgText::Text> text = new osgText::Text;
        textList.push_back(text);
        text->setFont(font);
        text->setColor(whiteColor);
        text->setCharacterSize(16.0f);
        text->setFontResolution(40, 40);
        text->setLayout(osgText::Text::LEFT_TO_RIGHT);
        text->setAlignment(osgText::Text::LEFT_BASE_LINE);
        text->setPosition(osg::Vec3d(leftStartPosition, VIEWER_HEIGHT - 141.0f, 0.0f));
        text->setText(opacityTable);
        dynamicHudGeode->addDrawable(text.get());
    }

    // culling
    {
        osg::ref_ptr<osgText::Text> text = new osgText::Text;
        textList.push_back(text);
        text->setFont(font);
        text->setColor(offColor);
        text->setCharacterSize(16.0f);
        text->setFontResolution(40, 40);
        text->setLayout(osgText::Text::LEFT_TO_RIGHT);
        text->setAlignment(osgText::Text::LEFT_BASE_LINE);
        text->setPosition(osg::Vec3d(leftStartPosition, VIEWER_HEIGHT - 157.0f, 0.0f));
        text->setText("OFF");
        dynamicHudGeode->addDrawable(text.get());
    }

    return textList;
}

osg::TextList buildScenegraph(osg::ref_ptr<osg::Group> root, osg::ref_ptr<osg::Group> cuboidGroup,
                              osg::ref_ptr<vipre::TexturedCuboid> cuboid, osg::ref_ptr<PreRenderCamera> preRenderCamera,
                              osg::ref_ptr<osg::Camera> postRenderCamera)
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
    preRenderCamera->setTexture(texture);
    preRenderCamera->setNodeMask(0);
    preRenderCamera->addChild(cuboid.get());

    // Create a post-render camera quad
    {
        postRenderCamera->setProjectionMatrix(osg::Matrix::ortho2D(0, 1000, 0, 1000));
        postRenderCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
        postRenderCamera->setViewMatrix(osg::Matrix::identity());
        postRenderCamera->setRenderOrder(osg::Camera::POST_RENDER);
        postRenderCamera->setClearMask(GL_DEPTH_BUFFER_BIT);
        postRenderCamera->setNodeMask(0);

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

    // Create the display hud camera
    osg::TextList textList;
    osg::ref_ptr<osg::Camera> displayHudCamera = new osg::Camera();
    {
        displayHudCamera->setProjectionMatrix(osg::Matrix::ortho2D(0, VIEWER_WIDTH, 0, VIEWER_HEIGHT));
        displayHudCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
        displayHudCamera->setViewMatrix(osg::Matrix::identity());
        displayHudCamera->setRenderOrder(osg::Camera::POST_RENDER);
        displayHudCamera->setClearMask(GL_DEPTH_BUFFER_BIT);
        displayHudCamera->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

        // Create the display HUD
        osg::ref_ptr<osg::Switch> displayHudSwitch = new osg::Switch();
        osg::ref_ptr<osg::Geode> displayHudGeode = new osg::Geode();
        osg::ref_ptr<osgText::Text> text = new osgText::Text;
        osg::ref_ptr<osgText::Font> font = osgText::readFontFile("fonts/VeraMono.ttf");
        text->setFont(font);
        text->setColor(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
        text->setCharacterSize(16.0f);
        text->setFontResolution(40, 40);
        text->setLayout(osgText::Text::LEFT_TO_RIGHT);
        text->setAlignment(osgText::Text::LEFT_BASE_LINE);
        text->setPosition(osg::Vec3d(20.0f, VIEWER_HEIGHT - 28.0f, 0.0f));

        // Create the text
    	std::ostringstream hudString;
        hudString << "Frame Rate          :" << std::endl;
        hudString << "Intensities         :" << std::endl;
        hudString << "Octree Traversal (o):" << std::endl;
        hudString << "Lighting         (l):" << std::endl;
        hudString << "Pre-render Pass  (v):" << std::endl;
        hudString << "Pre-render HUD   (h):" << std::endl;
        hudString << "Color Table      (c):" << std::endl;
        hudString << "Opacity Table    (t):" << std::endl;
        hudString << "Culling          (a):" << std::endl;
        hudString << "Next View        (n)" << std::endl;
        hudString << "Previous View    (p)" << std::endl;
        text->setText(hudString.str());
        displayHudGeode->addDrawable(text.get());
        displayHudSwitch->addChild(displayHudGeode.get());
        displayHudCamera->addChild(displayHudSwitch.get());

        // Create the dynamic hud geode
        textList = buildDynamicHudGeode(59.98f, false, false, false, false, "Muscle and Bone",
                                        "Linear Smooth", 0.38, 0.54, displayHudSwitch);
    }

    // Set up the scenegraph structure
    root->addChild(preRenderCamera.get());
    root->addChild(cuboidGroup.get());
    root->addChild(postRenderCamera.get());
    root->addChild(displayHudCamera.get());

    // Add the shaders to all the nodes
    vipre::ShaderHandler::instance()->addShadersToNode("prerendered_backfaces", preRenderCamera.get());
    vipre::ShaderHandler::instance()->addShadersToNode("defense_raycasting2", cuboidGroup.get());

    // Add the cuboid dimensions uniform to the prerendered backfaces shader
    osg::Vec3d cuboidDimensions = cuboid->getCuboidDimensions();
    osg::ref_ptr<osg::Uniform> uniform = new osg::Uniform("cuboidDimensions", cuboidDimensions);
    cuboidGroup->getOrCreateStateSet()->addUniform(uniform.get());

    // Set the backfaces texture on the cuboid
    cuboid->setBackfacesTextureUniform(texture);

    return textList;
}

/** Creates a light and light source from the scenegraph. */
osg::ref_ptr<osg::LightSource> createLightSource()
{
    // Set up the lighting parameters
	float diffuseLight = 1.0f;
    float ambientLight = 0.3f;
	float specularLight = 0.1f;
    osg::Vec4d lightPosition(0.3f, 0.3f, 1.0f, 0.0f);
    
    // Create the light
    osg::ref_ptr<osg::Light> light = new osg::Light();
	light->setLightNum(0);
	light->setPosition(lightPosition);
	light->setDiffuse(osg::Vec4d(diffuseLight, diffuseLight, diffuseLight, 1.0f));
	light->setAmbient(osg::Vec4d(ambientLight, ambientLight, ambientLight, 1.0f));
	light->setSpecular(osg::Vec4d(specularLight, specularLight, specularLight, 1.0f));
    
	// Create the light source
	osg::ref_ptr<osg::LightSource> lightSource = new osg::LightSource();
	lightSource->setLight(light.get());
	lightSource->setLocalStateSetModes(osg::StateAttribute::ON);
    
    return lightSource;
}

/**
 * This is a demo application for my PhD defense showing off octrees, lighting and
 * pre-rendered backface computation.
 */
int main(int argc, char **argv)
{
    // Set the logging levels for both vipre and osg
    vipre::Log::instance()->setLogLevel(vipre::DEBUG_LVL);
//    osg::setNotifyLevel(osg::FATAL);
    osg::setNotifyLevel(osg::INFO);

    // Use an ArgumentParser object to manage the program arguments.
    osg::ArgumentParser arguments(&argc, argv);
	osg::ApplicationUsage *appUsage = (arguments.getApplicationUsage());
	
    //osg::ApplicationUsage* appUsage = arguments.getApplicationUsage();
	//appUsage = *arguments.getApplicationUsage();
	
    // Set up the usage document, in case we need to print out how to use this program.
	//vipre::String appName = new vipre::String(osgDB::getSimpleFileName(arguments.getApplicationName()));

	std::string appName, appName2;
	appName = "";
	appName2 = "";
	appName = arguments.getApplicationName();
	appName2 = osgDB::getSimpleFileName(arguments.getApplicationName());
	


	vipre::String description = appName2;
	//vipre::String description(vipre::String() << osgDB::getSimpleFileName(arguments.getApplicationName()) << " TODO");
	//vipre::String description = "ff";
	//std::string description = "gdg";
	printf("String: %s\n", description.c_str());

	appUsage->setDescription(description.c_str());
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
    osg::ref_ptr<PreRenderCamera> preRenderCamera = new PreRenderCamera();
    osg::ref_ptr<osg::Camera> postRenderCamera = new osg::Camera();
    osg::TextList textList = buildScenegraph(root, cuboidGroup, cuboid, preRenderCamera, postRenderCamera);

    // Set up a light in the scene
    osg::ref_ptr<osg::LightSource> lightSource = createLightSource();
	lightSource->setStateSetModes(*cuboidGroup->getOrCreateStateSet(), osg::StateAttribute::ON);

    // Set up a pre-draw callback on the camera so we can set the view-to-world matrix uniform on the cuboid
    osg::ref_ptr<CameraDrawCallback> callback = new CameraDrawCallback(cuboid, textList, viewer, lightSource->getLight());
    viewer->getCamera()->setPreDrawCallback(callback.get());

    // Add the root node to the viewer
    viewer->setSceneData(root.get());

    // Add a trackball manipulator
    osg::ref_ptr<vipre::TrackballManipulator> manipulator = new vipre::TrackballManipulator();
    manipulator->setCamera(viewer->getCamera());
    viewer->setCameraManipulator(manipulator.get());
    manipulator->setNode(cuboidGroup.get());

    // Create a key event handler to process key events
    viewer->addEventHandler(new KeyEventHandler(cuboid, cuboidGroup, callback, preRenderCamera, postRenderCamera));

    // Start up the rendering
    return viewer->run();
}
