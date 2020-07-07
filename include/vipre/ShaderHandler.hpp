//
//  ShaderHandler.hpp
//  vipre
//
//  Created by Christian Noon on 2/12/12.
//  Copyright (c) 2012 Christian Noon. All rights reserved.
//

#ifndef VIPRE_SHADER_HANDLER_HPP
#define VIPRE_SHADER_HANDLER_HPP

#include <map>
#include <set>

#include <osg/Node>

#include <vipre/Export.hpp>
#include <vipre/String.hpp>

namespace vipre {

class VIPRE_EXPORT ShaderHandler
{
public:

    /** Creates a singleton instance. */
    static ShaderHandler* instance() { static ShaderHandler sb; return &sb; }

    /** Adds the path to the list of paths to find shaders. */
    inline void addShaderPath(String path) { _shaderPaths.insert(path); }

    /** Loads the vert and frag shaders into a program using the defined shader paths. */
    void addShadersToNode(String shaderName, osg::Node* node);

    /** Re-loads all the shaders. */
    void reloadShaders();

protected:

    /** Constructor. */
    ShaderHandler();

    /** Destructor. */
    ~ShaderHandler();

    /** Locates the shader by iterating through all the shader paths that have been registered. */
    String getPathToShader(String shaderName);

    /** Instance member variables. */
    std::set<String> _shaderPaths;
	std::map<String, osg::ref_ptr<osg::Program> > _programsMap;
};

}   // End of vipre namespace

#endif  // End of VIPRE_SHADER_HANDLER_HPP
