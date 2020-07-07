//
//  ShaderHandler.cpp
//  vipre
//
//  Created by Christian Noon on 2/12/12.
//  Copyright (c) 2012 Christian Noon. All rights reserved.
//

#include <osg/Program>
#include <osgDB/FileUtils>

#include <vipre/Environment.hpp>
#include <vipre/Log.hpp>
#include <vipre/ShaderHandler.hpp>

using namespace vipre;

ShaderHandler::ShaderHandler() :
    _shaderPaths()
{
    // By default, add the VIPRE_SHADER_PATH environment variable if it exists
    String vipre_shader_path = getEnvironmentVariable("VIPRE_SHADER_PATH");
    StringList paths = vipre_shader_path.split(":");
    StringList::iterator iter = paths.begin();
    while (iter != paths.end())
    {
        if (osgDB::fileType(*iter) == osgDB::DIRECTORY)
        {
            vipreLogDEBUG(viprePrefix) << "Adding shader search path: " << *iter << std::endl;
            _shaderPaths.insert(*iter);
        }
        ++iter;
    }
}

ShaderHandler::~ShaderHandler()
{
    ;
}

void ShaderHandler::addShadersToNode(String shaderName, osg::Node* node)
{
    // Attempt to load both the vertex and fragment shaders
	String vert_shader_path = getPathToShader(shaderName + ".vert");
	String frag_shader_path = getPathToShader(shaderName + ".frag");

    // Make sure the shaders were located
    if (vert_shader_path.empty() || frag_shader_path.empty())
    {
        vipreLogWARNING(viprePrefix) << "Could not load the vert or frag shaders: " << shaderName << std::endl;
        return;
    }
    
	// Create a new program to house the two shaders
	osg::ref_ptr<osg::Program> program = new osg::Program();

	// Create the vertex shader
	osg::ref_ptr<osg::Shader> vert_shader = new osg::Shader(osg::Shader::VERTEX);
	vert_shader->loadShaderSourceFromFile(vert_shader_path);
	program->addShader(vert_shader.get());
    
	// Create the fragment shader
	osg::ref_ptr<osg::Shader> frag_shader = new osg::Shader(osg::Shader::FRAGMENT);
	frag_shader->loadShaderSourceFromFile(frag_shader_path);
	program->addShader(frag_shader.get());

	// Apply the program shaders to the node
	node->getOrCreateStateSet()->setAttributeAndModes(program.get(), osg::StateAttribute::ON);

    // Keep the program around for reloading
    vert_shader_path.remove(".vert");
    _programsMap[vert_shader_path] = program;
}

void ShaderHandler::reloadShaders()
{
	std::map<String, osg::ref_ptr<osg::Program> >::iterator iter;
	for (iter = _programsMap.begin(); iter != _programsMap.end(); ++iter)
	{
		String shader_path = iter->first;
		String vert_path = shader_path + ".vert";
        String frag_path = shader_path + ".frag";
		iter->second->getShader(0)->loadShaderSourceFromFile(vert_path);
		iter->second->getShader(1)->loadShaderSourceFromFile(frag_path);
	}
}

String ShaderHandler::getPathToShader(String filename)
{
    std::set<String>::iterator iter = _shaderPaths.begin();
	for (iter = _shaderPaths.begin(); iter != _shaderPaths.end(); ++iter)
    {
        String shader_path = String::join(*iter, filename);
        if (osgDB::fileExists(shader_path))
        {
            return shader_path;
        }
    }

    return String();
}
