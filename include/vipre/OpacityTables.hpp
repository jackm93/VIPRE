/*
 *  OpacityTables.hpp
 *  vipre
 *
 *  Created by Christian Noon on 2/14/12.
 *  Copyright 2012 Christian Noon. All rights reserved.
 *
 */

#ifndef VIPRE_OPACITY_TABLES_HPP
#define VIPRE_OPACITY_TABLES_HPP

#include <osg/Texture1D>

#include <vipre/Export.hpp>

namespace vipre {

/** Forward declarations. */
class String;

class VIPRE_EXPORT OpacityTables
{
public:

	/** Constructor. */
	OpacityTables();

	/** Destructor. */
	~OpacityTables();

	/** Generates all the default opacity table textures and pushes the textures to the shaders. */
	void generateAllTables();

	/** Loads the appropriate opacity table into the shader. */
	void loadOpacityTable(String opacityTable, osg::ref_ptr<osg::StateSet> stateset, unsigned int textureUnit);

protected:

	/** Creates the linear smooth opacity table. */
	void createLinearSmoothTable();

	/** Creates the linear sharp opacity table. */
	void createLinearSharpTable();

	/** Creates the normal smooth opacity table. */
	void createNormalSmoothTable();

	/** Creates the normal sharp opacity table. */
	void createNormalSharpTable();

	/** Creates a 1D texture for the given opacity array. */
	void buildTexture(osg::ref_ptr<osg::Texture1D> texture, float* opacity);

	/** Instance member variables. */
    std::map<String, osg::ref_ptr<osg::Texture1D> > _textures;
};

}   // End of vipre namespace

#endif	// End of VIPRE_OPACITY_TABLES_HPP
