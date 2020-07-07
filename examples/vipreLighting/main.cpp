//
//  main.cpp
//  vipreLighting
//
//  Created by Christian Noon on 12/3/11.
//  Copyright (c) 2011 Christian Noon. All rights reserved.
//
#include <stdio.h>
#include <stddef.h>
#include <osg/ArgumentParser>
#include <osg/Camera>
#include <osg/CullFace>
#include <osg/Notify>
#include <osg/ref_ptr>
#include <osg/TexMat>
#include <osgDB/ReadFile>
#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>
#include <osgViewer/Viewer>
#include <osg/ShapeDrawable>
//#include "C:\SDK\X64\OpenSceneGraph-3.4.0\install\include\osgViewer\GraphicsWindow"
#include <osgViewer/ViewerEventHandlers>

#include <vipre/Log.hpp>
#include <vipre/NotificationCenter.hpp>
#include <vipre/ShaderHandler.hpp>
#include <vipre/String.hpp>
#include <vipre/TexturedCuboid.hpp>
#include <vipre/TrackballManipulator.hpp>
#include <vipreDICOM/SeriesBuilder.hpp>


#include <osg/Quat>
#include "openvrviewer.h"
#include "openvrdevice.h"
#include "openvreventhandler.h"
#include <osg/io_utils>
#include <osg/Depth>
#include <algorithm>
#include <iostream>

#include <osg/LineWidth>
#include <osg/Quat>
#include <osg/Node>
#include <osg/Geometry>
#include <osg/Notify>
#include <osg/Texture3D>
#include <osg/Texture1D>
#include <osg/ImageSequence>
#include <osg/TexGen>
#include <osg/Geode>
#include <osg/Billboard>
#include <osg/PositionAttitudeTransform>
#include <osg/ClipNode>
#include <osg/AlphaFunc>
#include <osg/TexGenNode>
#include <osg/TexEnv>
#include <osg/TexEnvCombine>
#include <osg/Material>
#include <osg/PrimitiveSet>
#include <osg/Endian>
#include <osg/BlendFunc>
#include <osg/BlendEquation>
#include <osg/TransferFunction>
#include <osg/MatrixTransform>
#include <osgGA/EventVisitor>
#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/StateSetManipulator>
#include "vrUtils.h"
#include <chrono>
#include <bitset>

#include <osgText/Font>
#include <osgText/Text3D>
//#include <osgText/TextNode>
//#include <osg/Vi>

//#include "osgvolume.h"
//#include <chrono>

#define VIEWER_WIDTH			1200
#define VIEWER_HEIGHT			800

#define examplePrefix           "[vipreLighting] "


//ovr code-------------------------------------------------------



class RootUpdate : public osg::NodeCallback {
public:
	virtual void operator() (
		osg::Node * node,
		osg::NodeVisitor * nv
		);


};
void RootUpdate::operator() (osg::Node* node,
	osg::NodeVisitor* nv) {
	osg::PositionAttitudeTransform *root = static_cast<osg::PositionAttitudeTransform*> (node);
	//printf("Update root.\n");
	osg::Quat f = root->getAttitude();
	osg::Quat rot;
	rot.makeRotate(osg::DegreesToRadians(0.5f), osg::Vec3(0, 0, 1));

	f = f * rot;
	//root->setAttitude(f);

	traverse(node, nv);

}

//end ovr--------------------------------------------------------
//node world position visitor
class getWorldCoords : public osg::NodeVisitor {
public:
	getWorldCoords() : osg::NodeVisitor(NodeVisitor::TRAVERSE_PARENTS), done(false) {
		worldMatrix = new osg::Matrixf();
	}
	virtual void apply(osg::Node &node) {
		if (!done) {
			if (node.getNumParents() == 0) {
				worldMatrix->set(osg::computeLocalToWorld(this->getNodePath()));
				done = true;
			}
		}
	}
	osg::Matrixf* getMatrix() {
		return worldMatrix;
	}
private:
	bool done;
	osg::Matrixf* worldMatrix;
};

osg::Matrixf* getWorldMatrix(osg::Node* node) {
	getWorldCoords* visitor = new getWorldCoords();
	if (node && visitor) {
		node->accept(*visitor);
		return visitor->getMatrix();
	}
	else {
		return NULL;
	}
}
/*
osg::Matrixf getWorldTransform(osg::Node *node) {
	osg::Matrixf worldTrans;
	osg::MatrixList ml = node->getWorldMatrices();
	for (int i = ml.size() - 1; i >= 0; i--) {
		worldTrans.preMult(ml[i]);
	}

	return worldTrans;
}
*/
osg::Vec3 getWorldPosition(osg::Node *node) {
	osg::Matrixf worldTrans;
	osg::MatrixList ml = node->getWorldMatrices();
	for (int i = ml.size() - 1; i >= 0; i--) {
		worldTrans.preMult(ml[i]);
	}

	return osg::Vec3(worldTrans(3, 0), worldTrans(3, 1), worldTrans(3, 2));
}
/** Handles all key events coming from osg. */
class KeyEventHandler : public osgGA::GUIEventHandler
{
public:
	float storedValue;
    /** Constructor. */
    KeyEventHandler(osg::ref_ptr<vipre::TexturedCuboid> cuboid) :
        _cuboid(cuboid)
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
		storedValue = 0;
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
                        _cuboid->getOrCreateStateSet()->setAttributeAndModes(cull_face, osg::StateAttribute::ON);
                    }
                    else if (culling_counter == 1)
                    {
                        vipreLogINFO(examplePrefix) << "Enabling back-face culling" << std::endl;
                        cull_face->setMode(osg::CullFace::BACK);
                        _cuboid->getOrCreateStateSet()->setAttributeAndModes(cull_face, osg::StateAttribute::ON);
                    }
                    else
                    {
                        vipreLogINFO(examplePrefix) << "Disabling all culling" << std::endl;
                        _cuboid->getOrCreateStateSet()->setAttributeAndModes(cull_face, osg::StateAttribute::OFF);
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

						_cuboid->storedValue++;

						printf("Stored value = %f\n", _cuboid->storedValue);
                    }
                    else
                    {
                        _minIntensity -= _intensityDelta;
                        if (_minIntensity < 0.0f)
                        {
                            _minIntensity = 0.0f;
                        }

						_cuboid->storedValue--;

						printf("Stored value = %f\n", _cuboid->storedValue);
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

						_cuboid->storedValue2++;

						printf("Stored value = %f\n", _cuboid->storedValue2);
                    }
                    else
                    {
                        _maxIntensity -= _intensityDelta;
                        if (_maxIntensity <= _minIntensity)
                        {
                            _maxIntensity += _intensityDelta;
                        }

						_cuboid->storedValue2--;

						printf("Stored value = %f\n", _cuboid->storedValue2);
                    }

                    _cuboid->setMaxIntensityUniform(_maxIntensity);
                    vipreLogDEBUG(examplePrefix) << "New intensities: (" << _minIntensity << ", " << _maxIntensity << ")" << std::endl;
                }
                else if (ea.getKey() == 'l')
                {
                    static bool light_enabled = false;
                    if (light_enabled)
                    {
                        vipreLogDEBUG(examplePrefix) << "Light rendering [DISABLED]" << std::endl;
                        //vipre::ShaderHandler::instance()->addShadersToNode("composite_raycasting", _cuboid.get());
						_cuboid->setLightingEnabledUniform(false);
                    }
                    else
                    {
                        vipreLogDEBUG(examplePrefix) << "Light rendering [ENABLED]" << std::endl;
                        //vipre::ShaderHandler::instance()->addShadersToNode("light_raycasting", _cuboid.get());
						//vipre::ShaderHandler::instance()->addShadersToNode("defense_raycasting2", _cuboid.get());
						_cuboid->setLightingEnabledUniform(true);
                    }

                    light_enabled = !light_enabled;
                }
				else if (ea.getKey() == 'o')
				{
					static bool octree_enabled = false;
					if (octree_enabled)
					{
						vipreLogDEBUG(examplePrefix) << "octree rendering [DISABLED]" << std::endl;
						_cuboid->setOctreeRenderingEnabledUniform(false);
					}
					else
					{
						vipreLogDEBUG(examplePrefix) << "octree rendering [ENABLED]" << std::endl;

						_cuboid->setOctreeRenderingEnabledUniform(true);
					}

					octree_enabled = !octree_enabled;

				}
                else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Up)
				{
					
				}
				else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Down)
				{
					
				}
				else
                {
                    vipreLogDEBUG(examplePrefix) << "There's a keydown event going on!!!" << std::endl;
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
    osg::ref_ptr<osg::CullFace> _backFaceCull;
    osg::ref_ptr<osg::CullFace> _frontFaceCull;
    std::vector<vipre::String> _presetViews;
    int _currentPresetView;
    float _minIntensity;
    float _maxIntensity;
    float _intensityDelta;

	
};
class VRDrawCallback : public osg::Camera::DrawCallback {

public:
	VRDrawCallback(osg::Camera* camera, OpenVRTextureBuffer* textureBuffer,
		osg::ref_ptr<vipre::TexturedCuboid> cuboid, osg::ref_ptr<osg::Light> light, osg::ref_ptr<osg::PositionAttitudeTransform> pat, osg::Camera* othercam, OpenVRDevice* ovd)
		: m_camera(camera)
		, m_othercam(othercam)
		, m_ovd(ovd)
		, m_textureBuffer(textureBuffer)
		, _cuboid(cuboid)
		, _light(light)
		, _lightPosition(light->getPosition())
		, _pat(pat)
		, _previousCameraViewMatrix(new osg::Matrix(osg::Matrix::identity()))
	{
	}
	void VRDrawCallback::operator()(osg::RenderInfo& renderInfo) const
	{

		
		//m_textureBuffer->onPreRender(renderInfo);
		//m_textureBuffer->onPostRender(renderInfo);
		osg::Camera* camera = renderInfo.getCurrentCamera();
		//osg::Camera* camera = m_othercam;
		
		//printf("\n working?\n");
		// Compute the view to world matrix

		std::string cName = camera->getName();
		osg::Matrixf camera_matrix;
		camera_matrix.postMult(camera->getViewMatrix());


		//camera_matrix.postMult(camera->getProjectionMatrix());

		osg::Matrixf projMat = camera->getProjectionMatrix();

		
		//osg::Matrixf projMat2 = osg::Matrixf::identity();

		/*
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				projMat2(i, j) = projectionMatrix->m[i][j];
			}
		}
		*/
		//osg::Matrixf projectionMatrix;

		//projectionMatrix = osg::Matrixf::identity();
		//projectionMatrix(0, 0) = 0.965116f;
		//projectionMatrix(1, 1) = 0.965116f;
		//projectionMatrix(2, 2) = -1.000001f;
		//projectionMatrix(3, 2) = -0.010000f;
		//projectionMatrix(2, 3) = -1.00000f;

		//projMat(0, 0) = 1.0f + (_cuboid->storedValue / 10.f);
		//projMat(1, 1) = 1.0f + (_cuboid->storedValue2 / 10.f);

		//vr::IVRSystem* g = openvrDevice->getVrSystem();
		//vr::HmdMatrix44_t hL = g->GetProjectionMatrix(vr::EVREye::Eye_Left, 0, 100);
		//vr::HmdMatrix44_t hR = g->GetProjectionMatrix(vr::EVREye::Eye_Right, 0, 100);

		camera_matrix.postMult(projMat);


		//camera_matrix.postMult(projMat2);
		//original
		camera_matrix.postMult(camera->getViewport()->computeWindowMatrix());
		
		osg::Matrixf view_to_world_matrix;
		//camera_matrix.postMult(*_cuboid->getCuboidMatrix());

		view_to_world_matrix.invert(camera_matrix);
		view_to_world_matrix.postMult(osg::Matrix::inverse(*_cuboid->getCuboidMatrix()));

		
		
		
		osg::Matrix f = osg::Matrix::scale(_pat->getScale()) * osg::Matrix::rotate(_pat->getAttitude()) * osg::Matrix::translate(_pat->getPosition());
		osg::Matrix camMat = m_othercam->getInverseViewMatrix();

		camMat.postMult(osg::Matrix::inverse(f));

		float d[] = { camMat(3, 0) / _cuboid->getCuboidDimensions()[0], camMat(3, 1) / _cuboid->getCuboidDimensions()[1], camMat(3, 2) / _cuboid->getCuboidDimensions()[2] };
		//float d[] = { camMat(3, 0) / _cuboid->getCuboidDimensions()[0], camMat(3, 1) / _cuboid->getCuboidDimensions()[1], camMat(3, 2) / 512 };
		if ((d[0] < 0.5f && d[0] > -0.5f) &&
			(d[1] < 0.5f && d[1] > -0.5f) &&
			(d[2] < 0.5f && d[2] > -0.5f)) {
			_cuboid->setCameraInsideVolumeUniform(true);
		}
		else {
			_cuboid->setCameraInsideVolumeUniform(false);
		}
		

		_cuboid->setViewToWorldMatrixUniform(view_to_world_matrix);
		_cuboid->setProjectionMatrixUniform(projMat);
		
		// Update the light position
		osg::Matrix inverse_view_matrix = camera->getInverseViewMatrix();
		osg::Quat rotation = inverse_view_matrix.getRotate();
		osg::Vec4d new_position = _lightPosition * osg::Matrix::rotate(rotation);
		//_light->setPosition(new_position);
		//_cuboid->setLightPositionUniform(osg::Vec3d(new_position[0], new_position[1], new_position[2]));

		// Update the previous matrix for the next round
		_previousCameraViewMatrix->set(camera->getViewMatrix());
		
	}


protected:
	~VRDrawCallback() {}
	osg::Camera* m_camera;
	osg::Camera* m_othercam;
	OpenVRDevice* m_ovd;
	OpenVRTextureBuffer* m_textureBuffer;
	vr::HmdMatrix44_t* projectionMatrix;
	/** Instance member variables. */
	osg::ref_ptr<vipre::TexturedCuboid> _cuboid;
	osg::ref_ptr<osg::Light> _light;
	osg::Vec4d _lightPosition;
	osg::Matrix* _previousCameraViewMatrix;
	osg::ref_ptr<osg::PositionAttitudeTransform> _pat;
	//osg::Matrix* cuboidMatrix;

};
/** Callback for the main camera pre-draw event. */
class CameraDrawCallback : public osg::Camera::DrawCallback
{
public:

    /** Constructor. */
    CameraDrawCallback(osg::ref_ptr<vipre::TexturedCuboid> cuboid, osg::ref_ptr<osg::Light> light) :
        _cuboid(cuboid),
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
        osg::Matrixf camera_matrix;
        camera_matrix.postMult(camera.getViewMatrix());
        camera_matrix.postMult(camera.getProjectionMatrix());
        camera_matrix.postMult(camera.getViewport()->computeWindowMatrix());
        osg::Matrixf view_to_world_matrix;
        view_to_world_matrix.invert(camera_matrix);

		//view_to_world_matrix.transla

        _cuboid->setViewToWorldMatrixUniform(view_to_world_matrix);
		//_cuboid->setViewToWorldMatrixUniform(camera_matrix);
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
    }

protected:

    /** Destructor. */
    ~CameraDrawCallback()
    {
        delete _previousCameraViewMatrix;
    }

    /** Instance member variables. */
    osg::ref_ptr<vipre::TexturedCuboid> _cuboid;
    osg::ref_ptr<osg::Light> _light;
    osg::Vec4d _lightPosition;
    osg::Matrix* _previousCameraViewMatrix;
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
    traits->windowName = "vipreLighting";
	traits->vsync = false;

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
	
	
	//vsync



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

/** Creates a light and light source from the scenegraph. */
osg::ref_ptr<osg::LightSource> createLightSource()
{
    // Set up the lighting parameters
	float diffuseLight = 0.0f;
    float ambientLight = 0.3f;
	float specularLight = 0.1f;
    osg::Vec4d lightPosition(0.7f, 0.7f, 0.7f, 0.0f);
    
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
 * TODO - Need a good description of this once I am almost finished.
 */
int main(int argc, char **argv)
{
	float minIntensity = 0.0f;//0.38f;
	float maxIntensity = 0.1f;//0.54f;
	float intensityDelta = 0.001f;

	osg::ref_ptr<osg::Material> wandmaterial = new osg::Material;

	wandmaterial->setColorMode(osg::Material::AMBIENT_AND_DIFFUSE);
	wandmaterial->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(0.3f, 0.3f, 0.3f, 1.f));
	wandmaterial->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0.01f, 0.01f, 0.01f, 1.0f));
	wandmaterial->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(0.9f, 0.9f, 0.9f, 0.9f));
	wandmaterial->setShininess(osg::Material::FRONT_AND_BACK, 1.0f);
	//load obj
	//osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFile("C:/Users/Jordan/Desktop/defense-rhel6/VIPRE/models/lc.obj");
	osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFile("../../../models/lc.obj");
	osg::ref_ptr<osg::Node> loadedModel2 = osgDB::readNodeFile("../../../models/rc.obj");

	loadedModel->getOrCreateStateSet()->setAttributeAndModes(wandmaterial.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	loadedModel2->getOrCreateStateSet()->setAttributeAndModes(wandmaterial.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	//osg::TexMat* tm = new osg::TexMat(osg::Matrix::scale(osg::Vec3(0.1f, 0.1f, 0.1f)));
	//loadedModel2->getOrCreateStateSet()->setTextureAttribute(0, tm, osg::StateAttribute::ON);
	osg::ref_ptr<osg::MatrixTransform> wandTrans = new osg::MatrixTransform();
	osg::ref_ptr<osg::MatrixTransform> wandTrans2 = new osg::MatrixTransform();
	wandTrans->addChild(loadedModel);
	wandTrans2->addChild(loadedModel2);
	wandTrans->setMatrix(osg::Matrix::rotate(3.1415926535, osg::Vec3(0.0, 1.0, 0.0))
		//* osg::Matrix::scale(osg::Vec3(0.1f, 0.1f, 0.1f)) 
		//* osg::Matrix::translate(osg::Vec3(0.0, -0.05, 0.075)) 
		* osg::Matrix::translate(osg::Vec3(0.0, -0.0f, 0.03f))
		* wandTrans->getMatrix());
	wandTrans2->setMatrix(osg::Matrix::rotate(-3.1415926535, osg::Vec3(1.0, 0.0, 0.0))
		//* osg::Matrix::scale(osg::Vec3(0.1f, 0.1f, 0.1f))
		//* osg::Matrix::translate(osg::Vec3(0.0, -0.05, 0.075))
		* osg::Matrix::translate(osg::Vec3(0.0, 0.0f, 0.03f))
		* wandTrans2->getMatrix());

	printf("start\n");

	osg::ref_ptr<osg::Geode> fontGeode = new osg::Geode;

	osg::ref_ptr<osgText::Text3D> text3d = new osgText::Text3D;
	text3d->setCharacterSize(0.01f);
	text3d->setCharacterDepth(0.001f);
	text3d->setText("Hola");
	text3d->setFont("fonts/arial.ttf");
	
	fontGeode->addDrawable(text3d);

	//bool isChild = true;
	//time_t timeStart = time(0);
	//float simTime = 0;

	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
	

	// Set the logging levels for both vipre and osg
	vipre::Log::instance()->setLogLevel(vipre::DEBUG_LVL);
	//    osg::setNotifyLevel(osg::FATAL);
	osg::setNotifyLevel(osg::INFO);

	// Use an ArgumentParser object to manage the program arguments.
	osg::ArgumentParser arguments(&argc, argv);
	osg::ApplicationUsage* appUsage = arguments.getApplicationUsage();

	// OVR --------------------------------------------------------------------
	//-------------------------------------------------------------------------
	osg::Vec3 center = osg::Vec3(0, -10, 0);
	osg::Vec3 eyePos = osg::Vec3(0, 0, 0);

	//cameraManipulator->setHomePosition(center, eyePos, osg::Vec3(0, 1, 0));

	// Exit if we do not have an HMD present
	if (!OpenVRDevice::hmdPresent())
	{
		osg::notify(osg::FATAL) << "Error: No valid HMD present!" << std::endl;
		return 1;
	}

	// Open the HMD
	float nearClip = 0.01f;
	//float farClip = 10000.0f;
	float farClip = 2000.0f;
	float worldUnitsPerMetre = 1.0f;
	int samples = 4;

	osg::ref_ptr<OpenVRDevice> openvrDevice = new OpenVRDevice(nearClip, farClip, worldUnitsPerMetre, samples);
	//viewer.openvrDevice = new OpenVRDevice(nearClip, farClip, worldUnitsPerMetre, samples);
	// Exit if we fail to initialize the HMD device
	if (!openvrDevice->hmdInitialized())
	{
		// The reason for failure was reported by the constructor.
		return 1;
	}

	// Get the suggested context traits
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = openvrDevice->graphicsContextTraits();
	traits->windowName = "Vipre VR";

	// Create a graphic context based on our desired traits
	osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits);

	if (!gc)
	{
		osg::notify(osg::NOTICE) << "Error, GraphicsWindow has not been created successfully" << std::endl;
		return 1;
	}

	if (gc.valid())
	{
		//gc->setClearColor(osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
		gc->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	int ar = 1;
	int *arc = &ar;
	char *arv[1] = { " " };
	osg::ArgumentParser argp = osg::ArgumentParser(arc, arv);

	GraphicsWindowViewer gViewer(argp, dynamic_cast<osgViewer::GraphicsWindow*>(gc.get()));

	gViewer.rootTransform = new osg::MatrixTransform;
	gViewer.snapClip = new osg::MatrixTransform(osg::Matrix::identity());
	gViewer.freeClip = new osg::MatrixTransform(osg::Matrix::identity());

	gViewer.rootTransform->setMatrix(osg::Matrix::identity());
	//gc->setClearColor(osg::Vec4(0.2f, 0.2f, 0.4f, 1.0f));
	gViewer.getCamera()->setClearColor(osg::Vec4(0.06f, 0.06f, 0.06f, 1.0f));
	// Force single threaded to make sure that no other thread can use the GL context
	gViewer.setThreadingModel(osgViewer::Viewer::SingleThreaded);
	//viewer.setThreadingModel(osgViewer::Viewer::ThreadPerCamera);
	gViewer.getCamera()->setGraphicsContext(gc);
	gViewer.getCamera()->setViewport(100, 0, traits->width, traits->height);

	// Disable automatic computation of near and far plane
	gViewer.getCamera()->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
	//viewer.setCameraManipulator(keyswitchManipulator.get());
	//gViewer.setCameraManipulator(keyswitchManipulator.get());

	// Things to do when viewer is realized
	osg::ref_ptr<OpenVRRealizeOperation> openvrRealizeOperation = new OpenVRRealizeOperation(openvrDevice);
	gViewer.setRealizeOperation(openvrRealizeOperation.get());

	osg::ref_ptr<OpenVRViewer> openvrViewer = new OpenVRViewer(&gViewer, openvrDevice, openvrRealizeOperation);

	//gViewer.getCamera()->getView()->setLightingMode(osg::View::NO_LIGHT);
	gViewer.getCamera()->getView()->setLightingMode(osg::View::HEADLIGHT);
	osg::Light* glight = gViewer.getCamera()->getView()->getLight();
	glight->setAmbient(osg::Vec4(0, 0, 0, 1));
	glight->setDiffuse(osg::Vec4(0, 0, 0, 1));
	glight->setSpecular(osg::Vec4(0, 0, 0, 1));

	gViewer.realize();

	gViewer.openvrDevice = openvrDevice;
	// end oculus viewer----------------------------------------------------


	osg::ref_ptr<OpenVRRealizeOperation> testSupportOperation = new OpenVRRealizeOperation(openvrDevice);
	gViewer.setRealizeOperation(testSupportOperation.get());




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
    //osg::ref_ptr<osgViewer::Viewer> viewer = createOsgViewer();

    // Set up the scenegraph structure
    //osg::ref_ptr<osg::Group> root = new osg::Group();

    osg::ref_ptr<vipre::TexturedCuboid> cuboid = createTexturedCuboidFromSeries(series, normalize);
	//osg::ref_ptr<vipre::TexturedCuboid> cuboid2 = createTexturedCuboidFromSeries(series, normalize);

	
	cuboid->setHMDBool(true);

	gViewer.patParent = new osg::PositionAttitudeTransform;
	gViewer.pat = new osg::PositionAttitudeTransform;
	
	gViewer.rootTransform->addChild(gViewer.pat);
	gViewer.pat->addChild(gViewer.patParent);
	gViewer.patParent->addChild(cuboid.get());


	int paddedAmount = 2;
	while (paddedAmount < cuboid->getCuboidDimensions()[2]) {

		paddedAmount *= 2;
	}
	//osg::Vec3 cuboidShift = (osg::Vec3(cuboid->getCuboidDimensions()[0] / -2, cuboid->getCuboidDimensions()[1] / -2, paddedAmount / -2));
	gViewer.patParent->setPosition(osg::Vec3(cuboid->getCuboidDimensions()[0] / -2, cuboid->getCuboidDimensions()[1] / -2, cuboid->getCuboidDimensions()[2] / -2));
	//gViewer.patParent->setPosition(cuboidShift);
	//gViewer.pat->addChild(cuboid.get());

	gViewer.pat->getOrCreateStateSet()->setRenderBinDetails(1, "RenderBin");

	gViewer.pat->setScale(osg::Vec3(0.003f, 0.003f, 0.003f));
	gViewer.pat->setPosition(osg::Vec3(0, 1.1, -1));


	//pivot point transform
	//gViewer.pat->setPivotPoint(osg::Vec3(100.f, 0.0f, 0.0f));
	
	//gViewer.pat->addChild(cuboid2.get());
	cuboid->setCuboidPosition(gViewer.pat->getPosition());

	gViewer.rootGroup = new osg::Group();
	gViewer.rootGroup->addChild(gViewer.rootTransform);
	gViewer.rootTransform->addChild(gViewer.axes);
	gViewer.axes->setPosition(osg::Vec3(0.0f, 0.4f, 0.0f));
    // Set up a light in the scene
	osg::ref_ptr<osg::LightSource> lightSource = createLightSource();
	lightSource->setStateSetModes(*cuboid->getOrCreateStateSet(), osg::StateAttribute::ON);
    
    // Add the shaders to the cuboid
    vipre::ShaderHandler::instance()->addShadersToNode("defense_raycasting2", cuboid.get());
	//vipre::ShaderHandler::instance()->addShadersToNode("defense_raycasting", cuboid2.get());
	//vipre::ShaderHandler::instance()->addShadersToNode("light_raycasting", cuboid.get());

    // Set up a pre-draw callback on the camera so we can set the view-to-world matrix uniform on the cuboid
    //viewer->getCamera()->setPreDrawCallback(new CameraDrawCallback(cuboid, lightSource->getLight()));
	//gViewer.getCamera()->setPreDrawCallback(new CameraDrawCallback(cuboid, lightSource->getLight()));

	//show clipping plane angle
	osg::ref_ptr<osg::Geode> yAngle = new osg::Geode;
	osg::ref_ptr<osg::PositionAttitudeTransform> yAngleTrans = new osg::PositionAttitudeTransform;

	int maxdots = 0; //10
	for (int i = 0; i < maxdots + 1; i++) {
		float xVal = cuboid->getCuboidDimensions()[0] * ((float)(i) / maxdots);
		
		yAngle->addDrawable(new osg::ShapeDrawable(new osg::Box(osg::Vec3(xVal, 0.0f, 0.0f), 5.f, 0.4f, 0.4f)));
		osg::Quat yQuat = yAngleTrans->getAttitude();
		osg::Quat rotY;
		rotY.makeRotate(0.1f, osg::Vec3(0.0f, 1.0f, 0.0f));
		yAngleTrans->setAttitude(rotY * yQuat);

		//yAngle->addDrawable(new osg::ShapeDrawable(new osg::Box(osg::Vec3(xVal, 0.0f, 0.0f), 5.f, 5.f, 5.f)));

	}

	yAngleTrans->addChild(yAngle);
	gViewer.pat->addChild(yAngleTrans);
	//yAngleTrans->setScale(osg::Vec3(200.0f, 200.0f, 200.0f));
	yAngleTrans->setPosition(osg::Vec3(0.0f, cuboid->getCuboidDimensions()[2] * 2.2f, 0.0f));

	osg::ref_ptr<osg::PositionAttitudeTransform> ptL = new osg::PositionAttitudeTransform;
	osg::ref_ptr<osg::PositionAttitudeTransform> boxPtL = new osg::PositionAttitudeTransform;
	osg::ref_ptr<osg::PositionAttitudeTransform> ptR = new osg::PositionAttitudeTransform;
	osg::ref_ptr<osg::PositionAttitudeTransform> boxPtR = new osg::PositionAttitudeTransform;
	//osg::ref_ptr<osg::PositionAttitudeTransform> pointerTransform = new osg::PositionAttitudeTransform;

	osg::ref_ptr<osg::MatrixTransform> pointerTransform = new osg::MatrixTransform;
	//osg::ref_ptr<osg::MatrixTransform> pointerTransform2 = new osg::MatrixTransform; //stationary clipping plane
	osg::ref_ptr<osg::PositionAttitudeTransform> pointerTransform2 = new osg::PositionAttitudeTransform;
	osg::ref_ptr<osg::Geode> newGeodeL = new osg::Geode;
	osg::ref_ptr<osg::Geode> newGeodeR = new osg::Geode;
	osg::ref_ptr<osg::Geode> pointerGeode = new osg::Geode;
	osg::ref_ptr<osg::Geode> clipPlaneGeode = new osg::Geode;

	osg::ref_ptr<osg::MatrixTransform> headTrans = new osg::MatrixTransform(osg::Matrix::identity());
	headTrans->setMatrix(osg::Matrix::translate(osg::Vec3(0, 0.5, -1)));
	osg::ref_ptr<osg::Geode> headSphere = new osg::Geode;
	headSphere->addDrawable(new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0, 0, 0), 5.f)));
	headTrans->addChild(headSphere);
	

	osg::ref_ptr<osg::ShapeDrawable> newBoxL = new osg::ShapeDrawable(new osg::Box(osg::Vec3(0.0f, 0.0f, 0.0f), 0.05f, 0.05f, 0.05f));
	osg::ref_ptr<osg::ShapeDrawable> newBoxR = new osg::ShapeDrawable(new osg::Box(osg::Vec3(0.0f, 0.0f, 0.0f), 0.05f, 0.05f, 0.05f));
	osg::ref_ptr<osg::ShapeDrawable> pointer = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0, 0, 0), 0.5f));
	osg::ref_ptr<osg::ShapeDrawable> plane = new osg::ShapeDrawable(new osg::Box(osg::Vec3(0.0f, 0.0f, 0.0f), 0.01f, 0.6f, 0.6f));
	osg::ref_ptr<osg::ShapeDrawable> plane2 = new osg::ShapeDrawable(new osg::Box(osg::Vec3(0.0f, 0.0f, 0.0f), 0.01f, 0.6f, 0.6f));

	osg::Depth* dep = new osg::Depth(osg::Depth::Function::ALWAYS);
	
	pointerGeode->addDrawable(plane);
	clipPlaneGeode->addDrawable(plane2);
	//newGeodeL->addDrawable(newBoxL);
	//newGeodeR->addDrawable(newBoxR);
	//newGeodeR->addDrawable(text);
	boxPtL->addChild(newGeodeL);
	boxPtR->addChild(newGeodeR);

	pointerTransform->addChild(pointerGeode);
	pointerTransform2->addChild(clipPlaneGeode);
	
	//pointerTransform->setMatrix(osg::Matrix::identity());
	//pointerTransform->setPosition(osg::Vec3(0.0f, 0.0f, 0.0f));
	//boxPtL->setPosition(osg::Vec3(0.0f, 0.0f, -0.4f));
	//ptL->addChild(boxPtL);
	//add wand
	ptL->addChild(wandTrans);
	//ptR->addChild(wandTrans2);
	boxPtR->setPosition(osg::Vec3(0.0f, 0.0f, -0.0f));
	boxPtR->addChild(wandTrans2);
	ptR->addChild(boxPtR);
	ptR->addChild(fontGeode);
	//boxPtR->addChild(fontGeode);

	osg::ref_ptr<osg::Material> material = new osg::Material;
	osg::ref_ptr<osg::Material> material2 = new osg::Material;
	osg::ref_ptr<osg::Material> material3 = new osg::Material;
	osg::ref_ptr<osg::Material> redmaterial = new osg::Material;

	material->setColorMode(osg::Material::DIFFUSE);
	material->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(0.1f, 0.1f, 0.1f, 1.f));
	material->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0.1f, 0.1f, 0.1f, 1.0f));
	material->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(0.9f, 0.9f, 0.9f, 0.9f));
	material->setShininess(osg::Material::FRONT_AND_BACK, 1.0f);

	//material2 = dynamic_cast<osg::Material*>(material->clone(osg::CopyOp::DEEP_COPY_ALL));
	
	material2->setColorMode(osg::Material::EMISSION);
	material2->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(0.9f, 0.9f, 0.9f, 1.f));
	material2->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0.3f, 0.9f, 0.9f, 1.0f));
	material2->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(0.9f, 0.9f, 0.9f, 0.9f));
	material2->setEmission(osg::Material::FRONT_AND_BACK, osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	material2->setShininess(osg::Material::FRONT_AND_BACK, 0.3f);
	

	material3->setColorMode(osg::Material::AMBIENT_AND_DIFFUSE);
	material3->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0f, 0.0f, 1.0f, 1.f));
	material3->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0.3f, 0.9f, 0.9f, 0.99f));
	//material2->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f));
	//material3->setEmission(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0f, 0.0f, 1.0f, 0.2f));
	
	material3->setShininess(osg::Material::FRONT_AND_BACK, 0.3f);

	redmaterial->setColorMode(osg::Material::AMBIENT_AND_DIFFUSE);
	redmaterial->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(0.5f, 0.0f, 0.0f, 1.f));
	redmaterial->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(1.0f, 0.1f, 0.1f, 1.f));
	redmaterial->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(1.0f, 1.0f, 1.0f, 0.3f));

	
	text3d->getOrCreateStateSet()->setAttributeAndModes(material2.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

	newBoxL->getOrCreateStateSet()->setAttributeAndModes(redmaterial.get(), osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
	gViewer.rootTransform->addChild(ptL);
	
	//gViewer.rootTransform->addChild(headTrans);
	
	//newBoxL->getOrCreateStateSet()->setAttribute(dep);
	

	newBoxR->getOrCreateStateSet()->setAttributeAndModes(redmaterial.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	gViewer.rootTransform->addChild(ptR);
	//newBoxR->getOrCreateStateSet()->setAttribute(dep);
	pointer->getOrCreateStateSet()->setAttributeAndModes(material3.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);


	//gViewer.rootTransform->addChild(pointerTransform);

	gViewer.pat->addChild(pointerTransform);

	gViewer.pat->addChild(pointerTransform2);

	//openvrViewer->getLeft()->setPreDrawCallback(new VRDrawCallback(openvrViewer->getLeft(), &openvrDevice->getbuf()[0]));

    // Create a key event handler to process key events
	//viewer->addEventHandler(new KeyEventHandler(cuboid));
	gViewer.addEventHandler(new KeyEventHandler(cuboid));
	//gViewer.addEventHandler(new KeyEventHandler(cuboid2));

    // Add the root node to the viewer
    //viewer->setSceneData(root.get());

	//openvrViewer->configure();

	openvrViewer->addChild(gViewer.rootTransform);
	gViewer.setSceneData(openvrViewer);

	osg::Matrix gProj = gViewer.getCamera()->getProjectionMatrix();
	osg::Matrix gTrans = gViewer.getCamera()->getViewMatrix();
	osg::Matrix openVRLeft, openVRRight;
	osg::Matrix openVRLeftProj;

	// Add stats
	gViewer.addEventHandler(new osgViewer::StatsHandler());


	gViewer.frame();
	
	osg::Camera* lc = openvrViewer->getLeft();
	osg::Camera* rc = openvrViewer->getRight();
	//lc->setPreDrawCallback(NULL);
	//rc->setPreDrawCallback(NULL);
	//gViewer.getCamera()->getView()->setLightingMode(osg::View::NO_LIGHT);
	//lc->getView()->setLightingMode(osg::View::NO_LIGHT);
	//rc->getView()->setLightingMode(osg::View::NO_LIGHT);


	//rc->setPreDrawCallback(new VRDrawCallback(rc, &openvrDevice->getbuf()[0], cuboid, lightSource->getLight()));
	lc->setPostDrawCallback(new VRDrawCallback(lc, &openvrDevice->getbuf()[0], cuboid, lightSource->getLight(), gViewer.pat, rc, gViewer.openvrDevice));
	rc->setPostDrawCallback(new VRDrawCallback(rc, &openvrDevice->getbuf()[1], cuboid, lightSource->getLight(), gViewer.pat, lc, gViewer.openvrDevice));

	
	//lc->setClearColor(osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
	//rc->setClearColor(osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
	
	//unsigned int lCull = lc->getCullMask();
	//unsigned int cuboidCull = cuboid->getNodeMask();

	//cuboid->setNodeMask(0x02);
	//lc->setCullMask(0xD);
	//rc->setCullMask(0xD);
	//ptR->setNodeMask(0x02);
	//ptL->setNodeMask(0x02);
	std::string lCullStr = std::bitset<sizeof(int)>(lc->getCullMask()).to_string();
	std::string cuboidCullStr = std::bitset<sizeof(int)>(cuboid->getNodeMask()).to_string();

	addHands(&gViewer);
	
	//pointer->getOrCreateStateSet()->setAttributeAndModes(material3.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	gViewer.handR->addDrawable(pointer);
	//rc->setPreDrawCallback(new VRDrawCallback(rc, &openvrDevice->getbuf()[1], cuboid, lightSource->getLight()));
	//pt->setPosition(lc->getInverseViewMatrix().getTrans());

	//create room-----
	osg::ref_ptr<osg::PositionAttitudeTransform> wallTransform = new osg::PositionAttitudeTransform;
	osg::ref_ptr<osg::PositionAttitudeTransform> wallTransform2 = new osg::PositionAttitudeTransform;
	osg::ref_ptr<osg::PositionAttitudeTransform> wallTransform3 = new osg::PositionAttitudeTransform;
	osg::ref_ptr<osg::PositionAttitudeTransform> wallTransform4 = new osg::PositionAttitudeTransform;
	osg::ref_ptr<osg::PositionAttitudeTransform> floorTransform = new osg::PositionAttitudeTransform;

	osg::ref_ptr<osg::Geode> wallGeode = new osg::Geode;
	osg::ref_ptr<osg::Geode> wallGeode2, wallGeode3, wallGeode4, floorGeode;
	osg::ref_ptr<osg::ShapeDrawable> wallBox = new osg::ShapeDrawable(new osg::Box(osg::Vec3(0.0f, 0.0f, 0.0f), 3.f, 3.f, 0.2f));
	wallBox->getOrCreateStateSet()->setAttributeAndModes(material.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

	wallGeode->addDrawable(wallBox);
	wallGeode2 = dynamic_cast<osg::Geode*>(wallGeode->clone(osg::CopyOp::DEEP_COPY_ALL));
	wallGeode3 = dynamic_cast<osg::Geode*>(wallGeode->clone(osg::CopyOp::DEEP_COPY_ALL));
	wallGeode4 = dynamic_cast<osg::Geode*>(wallGeode->clone(osg::CopyOp::DEEP_COPY_ALL));
	floorGeode = dynamic_cast<osg::Geode*>(wallGeode->clone(osg::CopyOp::DEEP_COPY_ALL));

	floorGeode->getOrCreateStateSet()->setAttributeAndModes(material.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	
	wallTransform->addChild(wallGeode);
	wallTransform2->addChild(wallGeode2);
	wallTransform3->addChild(wallGeode3);
	wallTransform4->addChild(wallGeode4);
	floorTransform->addChild(floorGeode);

	osg::Quat rotFloor = osg::Quat(1.570796, osg::Vec3(1, 0, 0));
	floorTransform->setAttitude(rotFloor);

	//floorTransform->addChild(wallTransform);
	//floorTransform->addChild(wallTransform2);
	//floorTransform->addChild(wallTransform3);
	//floorTransform->addChild(wallTransform4);
	
	
	//gViewer.rootTransform->addChild(wallTransform);
	//gViewer.rootTransform->addChild(wallTransform2);
	//gViewer.rootTransform->addChild(wallTransform3);
	//gViewer.rootTransform->addChild(wallTransform4);
	gViewer.rootTransform->addChild(floorTransform);

	wallTransform->setPosition(osg::Vec3f(0.0, 1.5, -1.5));
	wallTransform2->setPosition(osg::Vec3f(0.0, 1.5, 1.5));
	
	osg::Quat rot = osg::Quat(1.570796, osg::Vec3(0, 1, 0));
	

	wallTransform3->setAttitude(rot * rotFloor);
	wallTransform4->setAttitude(rot * rotFloor);
	

	wallTransform3->setPosition(osg::Vec3f(1.5, 1.5, 0));
	wallTransform4->setPosition(osg::Vec3f(-1.5, 1.5, 0.0));
	floorTransform->setPosition(osg::Vec3f(0, 0, 0.0));


	//light---------------------
	// create a local light.

	osg::StateSet* rootStateSet = new osg::StateSet;
	gViewer.rootTransform->setStateSet(rootStateSet);


	osg::Light* myLight = new osg::Light;
	myLight->setLightNum(1);
	myLight->setPosition(osg::Vec4(-1.0, 1.7, 1.50, 1.0f));
	myLight->setAmbient(osg::Vec4(0.02f, 0.02f, 0.02f, 0.1f));
	myLight->setDiffuse(osg::Vec4(0.5f, 0.5f, 0.5f, 1.0f));
	myLight->setConstantAttenuation(0.8f);
	//myLight->setLinearAttenuation(2.0f /2.5);
	//myLight->setQuadraticAttenuation(2.0f / osg::square(2.5));

	osg::LightSource* lightS = new osg::LightSource;
	lightS->setLight(myLight);
	lightS->setLocalStateSetModes(osg::StateAttribute::ON);

	lightS->setStateSetModes(*rootStateSet, osg::StateAttribute::ON);

	gViewer.rootTransform->addChild(lightS);

	//------------------

	//viewer.realize();
    // Start up the rendering
    //return viewer->run();

	
	//boxPtL->getWorldMatrices();
	float minClip = 0;
	float maxClip = 1;
	float clip = 0.0f;
	
	cuboid->setOctreeRenderingEnabledUniform(false);
	cuboid->setMinIntensityUniform(minIntensity);
	cuboid->setMaxIntensityUniform(maxIntensity);

	//wireframe rendering
	osg::PolygonMode *pm = new osg::PolygonMode;
	osg::LineWidth *lw = new osg::LineWidth;
	lw->setWidth(2.0f);


	material2->setTransparency(osg::Material::FRONT_AND_BACK, 0.3);
	pm->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
	pointerGeode->getOrCreateStateSet()->setAttributeAndModes(pm, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
	pointerGeode->getOrCreateStateSet()->setAttribute(dep);
	//pointerGeode->getOrCreateStateSet()->setAttribute(lw);
	pointerGeode->getOrCreateStateSet()->setAttributeAndModes(material2.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

	
	clipPlaneGeode->getOrCreateStateSet()->setAttributeAndModes(pm, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
	clipPlaneGeode->getOrCreateStateSet()->setAttribute(dep);
	clipPlaneGeode->getOrCreateStateSet()->setAttribute(lw);
	clipPlaneGeode->getOrCreateStateSet()->setAttributeAndModes(material2.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);


	

	cuboid->setPointerPosUniform(osg::Vec3(-1, 0, 0));
	cuboid->setPointerQuatUniform(osg::Vec3(1, 0, 0));
	// turn to false for
	cuboid->setBackfaceTextureEnabledUniform(false);
	//cuboid->setBackfaceTextureEnabledUniform(true);
	//cuboid->setWindowDimensionsUniform(osg::Vec2(1440, 1600));
	//find offset information

	/*
	float fLeft, fRight, fTop, fBottom;
	vr::EVREye *e = new vr::EVREye();
	*e = vr::EVREye::Eye_Left;
	gViewer.openvrDevice->getVrSystem()->GetProjectionRaw(*e, &fLeft, &fRight, &fTop, &fBottom);


	float xOffsetL, yOffsetL, xOffsetR, yOffsetR;
	float total = fRight - fLeft;

	xOffsetL = 1440 * ((fLeft + fRight) / total);

	*e = vr::EVREye::Eye_Right;
	gViewer.openvrDevice->getVrSystem()->GetProjectionRaw(*e, &fLeft, &fRight, &fTop, &fBottom);

	total = fRight - fLeft;
	xOffsetR = 1440 * ((fLeft + fRight) / total);

	cuboid->setROffset(xOffsetR);
	cuboid->setLOffset(xOffsetL);
	delete e;
	*/
	//set backface culling off
	static osg::ref_ptr<osg::CullFace> cull_face = new osg::CullFace();
	cull_face->setMode(osg::CullFace::FRONT);
	cuboid->getOrCreateStateSet()->setAttributeAndModes(cull_face, osg::StateAttribute::ON);

	

	vr::VRControllerAxis_t *rAxis = &gViewer.openvrDevice->rightState->rAxis[gViewer.joyaxis];
	vr::VRControllerAxis_t *lAxis = &gViewer.openvrDevice->leftState->rAxis[gViewer.joyaxis];
	vr::VRControllerAxis_t *rPadAxis = &gViewer.openvrDevice->rightState->rAxis[gViewer.padaxis];
	vr::VRControllerAxis_t *lPadAxis = &gViewer.openvrDevice->leftState->rAxis[gViewer.padaxis];
	/*
	vr::VRControllerAxis_t *rAxis = &gViewer.openvrDevice->rightState->rAxis[2];
	vr::VRControllerAxis_t *lAxis = &gViewer.openvrDevice->leftState->rAxis[2];
	vr::VRControllerAxis_t *rPadAxis = &gViewer.openvrDevice->rightState->rAxis[0];
	vr::VRControllerAxis_t *lPadAxis = &gViewer.openvrDevice->leftState->rAxis[0];
	*/
	//cuboid->setColorTable(vipre::String("NIH"));
	//cuboid->setOpacityTable("");

	std::vector<vipre::String> strVec;
	strVec.push_back("NIH");
	strVec.push_back("Bone");
	strVec.push_back("Cardiac");
	strVec.push_back("GE");
	strVec.push_back("Grayscale");
	strVec.push_back("Muscle and Bone");
	strVec.push_back("Red Vessels");
	strVec.push_back("Stern");
	//createBoneTable();
	//createCardiacTable();
	//createGETable();
	//createGrayscaleTable();
	//createMuscleAndBoneTable();
	//createNIHTable();
	//createRedVesselsTable();
	//createSternTable();
	osg::Matrix vm;

	

	while (true) {
		vm = lc->getInverseViewMatrix();
		//forward head vector
		osg::Vec3 forwardVec = osg::Vec3(vm(2, 0), 0, vm(2, 2));
		osg::Vec3 rightVec = osg::Vec3(vm(0, 0), 0, vm(0, 2));
		// rootGroup -> handRRot -> handRPos -> handRCenter -> handR
		handleControllers(&gViewer);

		osg::Quat rot;

		if (gViewer.aToggle) {
			rAxis = &gViewer.openvrDevice->rightState->rAxis[gViewer.joyaxis];
			lAxis = &gViewer.openvrDevice->leftState->rAxis[gViewer.joyaxis];
			rPadAxis = &gViewer.openvrDevice->rightState->rAxis[gViewer.padaxis];
			lPadAxis = &gViewer.openvrDevice->leftState->rAxis[gViewer.padaxis];
		}

		if (rAxis->x * rAxis->x > 0.01f) {
			//continuous rotation
			gViewer.pat->setAttitude(osg::Quat((rAxis->x / 20.0f), osg::Vec3d(0.0, 1.0, 0.0)) * gViewer.pat->getAttitude());
			gViewer.axes->setAttitude(osg::Quat((rAxis->x / 20.0f), osg::Vec3d(0.0, 1.0, 0.0)) * gViewer.pat->getAttitude());

			//snapped rotation
			/*
			if (!gViewer.rDirection) {
				gViewer.rDirection = true;
				float amount = 3.1415926535f / 8.0f;
				if (rAxis.x < 0) {
					amount *= -1;
				}

				gViewer.pat->setAttitude(osg::Quat(amount, osg::Vec3d(0.0, 1.0, 0.0)) * gViewer.pat->getAttitude());

			}
			*/
		}
		else {
			if (gViewer.rDirection) {
				gViewer.rDirection = false;
			}
		}
		if (rAxis->y * rAxis->y > 0.1f) {
			gViewer.pat->setPosition(gViewer.pat->getPosition() + osg::Vec3d(0.0, rAxis->y / 100.0f, 0.0));

		}
		if (lAxis->x * lAxis->x > 0.01f) {
			//gViewer.pat->setPosition(gViewer.pat->getPosition() + osg::Vec3d(lAxis->x / 100.0, 0.0, 0.0));

			
			gViewer.pat->setPosition(gViewer.pat->getPosition() + rightVec * (lAxis->x / 100.0));
		}
		if (lAxis->y * lAxis->y > 0.1f) {
			//gViewer.pat->setPosition(gViewer.pat->getPosition() + osg::Vec3d(0.0, 0.0, lAxis->y / -100.0));

			gViewer.pat->setPosition(gViewer.pat->getPosition() + forwardVec * (-lAxis->y / 100.0));
		}
		

		osg::MatrixTransform *cuboidMT = new osg::MatrixTransform();
		osg::ref_ptr<osg::MatrixTransform> cuboidRot = new osg::MatrixTransform();
		cuboidMT->setMatrix(osg::Matrix::scale(gViewer.pat->getScale()) * osg::Matrix::rotate(gViewer.pat->getAttitude()) * osg::Matrix::translate(gViewer.pat->getPosition()));
		cuboidRot->setMatrix(osg::Matrix::rotate(gViewer.pat->getAttitude()));
		cuboid->setCuboidPosition(gViewer.pat->getPosition());
		//osg::Matrix cuboidm = (gViewer.pat->getAttitude()).get());
		//cuboid->setCuboidRotation(cuboidMT->getMatrix());
		cuboid->setCuboidRotation(cuboidRot->getMatrix());

		//gViewer.handR->getW
		osg::Matrixf worldTrans, worldTransMult;
		//osg::Matrixf* wt = getWorldMatrix(gViewer.handRCenter);
		worldTrans = getWorldTransform(gViewer.handRCenter);
		/*
		osg::MatrixList ml = gViewer.handRCenter->getWorldMatrices();
		for (int i = ml.size() - 1; i >= 0; i--) {
			worldTrans.preMult(ml[i]);
		}
		*/
		//world hand position
		worldTransMult = worldTrans;
		//worldTrans.preMult();
		

		//adjust pointer to cuboid transform
		//worldTransMult.postMult(osg::Matrix::inverse(*cuboid->getCuboidMatrix()));

		//world hand position to position relative to cuboid
		worldTransMult.postMult(cuboidMT->getInverseMatrix());
		
		
		//osg::Quat pointerrotation;
		//worldTransMult.decompose(nullptr, &pointerrotation, nullptr, nullptr);
		osg::Vec3f posit, scale;
		osg::Quat qu, quso;
		worldTransMult.decompose(posit, qu, scale, quso);

		osg::MatrixTransform *snapMat = new osg::MatrixTransform();
		snapMat->setMatrix( osg::Matrix::scale(scale) * osg::Matrix::rotate(qu) * osg::Matrix::translate(posit));

		//osg::Vec3 posVec(worldTransMult(3, 0) / cuboid->getCuboidDimensions()[0], worldTransMult(3, 1) / cuboid->getCuboidDimensions()[1], worldTransMult(3, 2) / cuboid->getCuboidDimensions()[0]);
		osg::Vec3 posVec(worldTransMult(3, 0) / cuboid->getCuboidDimensions()[0], worldTransMult(3, 1) / cuboid->getCuboidDimensions()[1], worldTransMult(3, 2) / paddedAmount);
		posVec += osg::Vec3(0.5f, 0.5f, (cuboid->getCuboidDimensions()[2] / paddedAmount) * 0.5f);

		/*
		printf("x = %f, y = %f, z = %f, w = %f\n", worldTrans(0, 0), worldTrans(1, 0), worldTrans(2, 0), worldTrans(3, 0));
		printf("x = %f, y = %f, z = %f, w = %f\n", worldTrans(0, 1), worldTrans(1, 1), worldTrans(2, 1), worldTrans(3, 1));
		printf("x = %f, y = %f, z = %f, w = %f\n", worldTrans(0, 2), worldTrans(1, 2), worldTrans(2, 2), worldTrans(3, 2));
		printf("x = %f, y = %f, z = %f  w = %f\n\n", worldTrans(0, 3), worldTrans(1, 3), worldTrans(2, 3), worldTrans(3, 3));
		*/
		
		
		//worldTrans.decompose(posit, qu, scale, quso);
		//printf("box location: x= %f, y= %f, z= %f\n", posit[0], posit[1], posit[2]);
		//std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
		//std::chrono::duration<float> simTime = std::chrono::duration_cast<std::chrono::duration<float>>(t2 - t1);
		//printf("time since start: %f\n", simTime.count());
		//cuboid->setClipPosition(sinf(simTime.count()));



		/*
		text3d->setText("X: " + std::to_string(posVec[0]) +
						"\nY: " + std::to_string(posVec[1]) +
			"\nZ: " + std::to_string(posVec[2]));
			*/

		double x, y, z;
		double ang;
		//qu.getRotate(ang, x, y, z);
		x = qu.asVec4()[0];
		y = qu.asVec4()[1];
		z = qu.asVec4()[2];
		ang = qu.asVec4()[3];

		//qu.

		double roll = atan2(2 * y*ang + 2 * x*z, 1 - 2 * y*y - 2 * z*z);
		double pitch = atan2(2 * x*ang + 2 * y*z, 1 - 2 * x*x - 2 * z*z);
		double yaw = asin(2 * x*y + 2 * z*ang);


		osg::Matrixf snaptest = cuboidMT->getMatrix();
		
		snaptest = osg::Matrix::rotate(roll, osg::Vec3f(0.0f, 1.f, 0.f)) * snaptest;	

		/*
		text3d->setText(std::string("     roll = " + std::to_string(roll) +
									"\n     pitch = " + std::to_string(pitch) + 
									"\n     yaw = " + std::to_string(yaw)));
		
		*/

		

		clip = worldTrans(3, 0) / cuboid->getCuboidDimensions()[0];

		//osg::Vec3d cubeDim = cuboid->getCuboidDimensions();
		//cubeDim.normalize();

		//clip *= (1.0 / cubeDim[0]);

		//printf("hand cube: %f\n", clip);
		//cuboid->setClipPosition(clip);

		//cuboid->
		if (gViewer.rTrigger) {
			gViewer.pt = GraphicsWindowViewer::planeType::snap;
		
			cuboid->setPointerPosUniform(posVec);

			osg::Vec3 newQuat = osg::Vec3(osg::Vec3(worldTransMult(0, 0), worldTransMult(0, 1), worldTransMult(0, 2)));
			//newQuat = newQuat * *(cuboid->getCuboidMatrix());
			//float r = sqrt(	(newQuat[0] * newQuat[0]) + (newQuat[2] * newQuat[1]) + (newQuat[2] * newQuat[2]));
			float r = sqrt(	(newQuat[0] * newQuat[0]) + (newQuat[2] * newQuat[1]) + (newQuat[2] * newQuat[2]));
			newQuat = osg::Vec3(newQuat[0] / r, newQuat[1] / r, newQuat[2] / r);
			float newx;
			float ax = acosf(newQuat[0]);
			newx = cosf(
				(float)((int)(ax / (3.1415926535f / 6))) *
				(3.1415926535f / 6)
			);
			newQuat[0] = newx;

			//SNAP roll and yaw, pitch ignored since it's in the x direction
			const float snapAmount = 3.1415926535f / 12.f;
			float snapRoll = snapAmount * floor(roll / snapAmount);
			float snapYaw = snapAmount * floor(yaw / snapAmount);
			pointerTransform2->setScale(scale);
			//pointerTransform2->setAttitude(qu);
			pointerTransform2->setAttitude(osg::Quat(snapYaw, osg::Vec3(0.0f, 0.0f, 1.0f)) * osg::Quat(snapRoll, osg::Vec3(0.0f, 1.0f, 0.0f)));
			pointerTransform2->setPosition(posit);
			

			osg::Matrix newMat = pointerTransform2->getWorldMatrices()[0] * osg::Matrix::inverse(gViewer.pat->getWorldMatrices()[0]);

			cuboid->setPointerQuatUniform(osg::Vec3(newMat(0, 0), newMat(0, 1), (paddedAmount / cuboid->getCuboidDimensions()[0]) * newMat(0, 2)));

			gViewer.snapClip->setMatrix(newMat);
			
			pointerTransform->setMatrix(osg::Matrix::identity());

			text3d->setText(std::string("          Clip Plane Position: \n          X = " + std::to_string(posVec[0]) +
				"\n          Y = " + std::to_string(posVec[1]) +
				"\n          Z = " + std::to_string(posVec[2])));


			gViewer.freePlaced = false;
			
			
		}
		else if (gViewer.rGrip) {
			if (gViewer.pt == GraphicsWindowViewer::planeType::snap) {
				pointerTransform2->setScale(osg::Vec3(0.01f, 0.01f, 0.01f));


				gViewer.pt = GraphicsWindowViewer::planeType::free;
			}
			


			if (gViewer.freePlaced) {
				if (gViewer.onRGrip) {
					gViewer.freeClip->setMatrix(pointerTransform->getMatrix());
					gViewer.grabTrans->setMatrix(worldTransMult);
				}
				osg::Matrix placedMatrix = (gViewer.freeClip->getMatrix() * osg::Matrix::inverse(gViewer.grabTrans->getMatrix()) * worldTransMult);
				pointerTransform->setMatrix(placedMatrix);
				//posVec = osg::Vec3(placedMatrix(3, 0) / cuboid->getCuboidDimensions()[0], placedMatrix(3, 1) / cuboid->getCuboidDimensions()[1], placedMatrix(3, 2) / cuboid->getCuboidDimensions()[2]);
				posVec = osg::Vec3(placedMatrix(3, 0) / cuboid->getCuboidDimensions()[0], placedMatrix(3, 1) / cuboid->getCuboidDimensions()[1], placedMatrix(3, 2) / paddedAmount);
				posVec += osg::Vec3(0.5f, 0.5f, (cuboid->getCuboidDimensions()[2] / paddedAmount) * 0.5f);
				cuboid->setPointerQuatUniform(osg::Vec3(placedMatrix(0, 0), placedMatrix(0, 1), placedMatrix(0, 2) * (paddedAmount / cuboid->getCuboidDimensions()[0])));
				cuboid->setPointerPosUniform(posVec);
			}
			else {
				cuboid->setPointerPosUniform(posVec);

				//osg::Vec3 newQuat = osg::Vec3(osg::Vec3(worldTransMult(0, 0), worldTransMult(0, 1), worldTransMult(0, 2)));
				gViewer.freeClip->setMatrix(worldTransMult);
				cuboid->setPointerQuatUniform(osg::Vec3(worldTransMult(0, 0), worldTransMult(0, 1), (paddedAmount / cuboid->getCuboidDimensions()[0]) * worldTransMult(0, 2)));

				//pointerTransform->setMatrix(worldTransMult);
				pointerTransform->setMatrix(worldTransMult);


				

				text3d->setText(std::string("          Clip Plane Position: \n          X = " + std::to_string(posVec[0]) +
					"\n          Y = " + std::to_string(posVec[1]) +
					"\n          Z = " + std::to_string(posVec[2])));

			}

		}

		//intensity
		//horizontal
		if (lPadAxis->x * lPadAxis->x > 0.3f && lPadAxis->y * lPadAxis->y < 0.3f) {
			if (lPadAxis->x < 0) {
				maxIntensity -= intensityDelta;
			}
			else {
				maxIntensity += intensityDelta;
			}
			
			if (maxIntensity <= minIntensity) {
				maxIntensity += intensityDelta;
			}
			else if (maxIntensity > 0.2f) {
				maxIntensity = 0.2f;
			}
			cuboid->setMaxIntensityUniform(maxIntensity);
			vipreLogDEBUG(examplePrefix) << "New intensities: (" << minIntensity << ", " << maxIntensity << ")" << std::endl;
		}
		//vertical
		if (lPadAxis->x * lPadAxis->x < 0.3f && lPadAxis->y * lPadAxis->y > 0.3f) {
			if (lPadAxis->y < 0) {
				minIntensity -= intensityDelta;
			}
			else {
				minIntensity += intensityDelta;
			}

			if (minIntensity >= maxIntensity) {
				minIntensity -= intensityDelta;
			}
			else if (minIntensity < 0) {
				minIntensity = 0;
			}
			cuboid->setMinIntensityUniform(minIntensity);
			vipreLogDEBUG(examplePrefix) << "New intensities: (" << minIntensity << ", " << maxIntensity << ")" << std::endl;
		}

		if (rPadAxis->x * rPadAxis->x > 0.01f || rPadAxis->y * rPadAxis->y > 0.01f) {
			//gViewer.pat->setPosition(gViewer.pat->getPosition() + osg::Vec3d(f.x / 100.0, 0.0, 0.0));
			//gViewer.pat->setPosition(gViewer.pat->getPosition() + osg::Vec3d(150.f, 0.0, 0.0));

			//continuous rotation
			//gViewer.pat->setAttitude(osg::Quat((rAxis.x / 20.0f), osg::Vec3d(0.0, 1.0, 0.0)) * gViewer.pat->getAttitude());

			//gViewer.pat->setPosition(gViewer.pat->getPosition() + osg::Vec3d(-150.f, 0.0f, 0.0));


			osg::Vec3 moveDirection(0.2f * rPadAxis->x, 0.0f, 0.2f * -rPadAxis->y);

			osg::Vec3f fPosition, fScale;
			osg::Quat fqu, fquso;

			if (gViewer.pt == GraphicsWindowViewer::planeType::snap) {
				gViewer.snapClip->getMatrix().decompose(fPosition, fqu, fScale, fquso);
				fPosition += moveDirection;

				gViewer.snapClip->setMatrix(osg::Matrix::scale(fScale) * osg::Matrix::rotate(fqu) * osg::Matrix::translate(fPosition));// *osg::Matrix::translate(posit));
				//pointerTransform2->setMatrix(gViewer.freeClip->getMatrix());
				pointerTransform2->setScale(fScale);
				pointerTransform2->setAttitude(fqu);
				pointerTransform2->setPosition(pointerTransform2->getPosition() + moveDirection);

				osg::Vec3 posVecFine(gViewer.snapClip->getMatrix()(3, 0) / cuboid->getCuboidDimensions()[0],
					gViewer.snapClip->getMatrix()(3, 1) / cuboid->getCuboidDimensions()[1],
					gViewer.snapClip->getMatrix()(3, 2) / paddedAmount);
					//gViewer.snapClip->getMatrix()(3, 2) / cuboid->getCuboidDimensions()[0]);

				posVec = posVecFine;
				//posVecFine += osg::Vec3(0.5f, 0.5f, 0.5f);
				cuboid->setPointerPosUniform(posVecFine);

				text3d->setText(std::string("          Clip Plane Position: \n          X = " + std::to_string(posVec[0]) +
					"\n          Y = " + std::to_string(posVec[1]) +
					"\n          Z = " + std::to_string(posVec[2])));
			}
			else {
				gViewer.freeClip->getMatrix().decompose(fPosition, fqu, fScale, fquso);
				fPosition += moveDirection;

				gViewer.freeClip->setMatrix(osg::Matrix::scale(fScale) * osg::Matrix::rotate(fqu) * osg::Matrix::translate(fPosition));// *osg::Matrix::translate(posit));
				pointerTransform->setMatrix(gViewer.freeClip->getMatrix());

				osg::Vec3 posVecFine(gViewer.freeClip->getMatrix()(3, 0) / cuboid->getCuboidDimensions()[0],
					gViewer.freeClip->getMatrix()(3, 1) / cuboid->getCuboidDimensions()[1],
					gViewer.freeClip->getMatrix()(3, 2) / paddedAmount);
					//gViewer.freeClip->getMatrix()(3, 2) / cuboid->getCuboidDimensions()[0]);
				posVec = posVecFine;
				//posVecFine += osg::Vec3(0.5f, 0.5f, 0.5f);
				cuboid->setPointerPosUniform(posVecFine);

				text3d->setText(std::string("          Clip Plane Position: \n          X = " + std::to_string(posVec[0]) +
					"\n          Y = " + std::to_string(posVec[1]) +
					"\n          Z = " + std::to_string(posVec[2])));
			}

		}

		

		if (clip < minClip) {
			clip = minClip;
		}
		else if (clip > maxClip) {
			clip = maxClip;
		}

		if (gViewer.onLGrip) {
			gViewer.colorTables++;
			if (gViewer.colorTables >= strVec.size()) {
				gViewer.colorTables = 0;
			}
			//printf((strVec->at(gViewer.colorTables)));
			cuboid->setColorTable(strVec[gViewer.colorTables]);
		}
		
		ptL->setAttitude(getWorldTransform(gViewer.handLPos).getRotate());
		ptL->setPosition(getWorldTransform(gViewer.handLPos).getTrans());


		ptR->setPosition(worldTrans.getTrans());
		gViewer.axes->setPosition(worldTrans.getTrans() + osg::Vec3(0.0f, 0.06f, 0.0f));

		ptR->setAttitude(rc->getInverseViewMatrix().getRotate());
		
		osg::Quat qua = worldTrans.getRotate() * rc->getViewMatrix().getRotate();
		
		osg::Matrix quam = worldTrans * rc->getViewMatrix();
		//boxPtR->setAttitude(worldTrans.getRotate());
		boxPtR->setAttitude(qua);
		//wandTrans2->setMatrix(quam * wandTrans2Matrix);
		
		//ptR->setPosition(rc->getInverseViewMatrix().getTrans());
		

		

		
		gViewer.frame();
	}

	return 0;
}
