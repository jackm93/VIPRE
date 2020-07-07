/*
 *  ColorTables.hpp
 *  vipre
 *
 *  Created by Christian Noon on 2/14/12.
 *  Copyright 2011 Christian Noon. All rights reserved.
 *
 */

#ifndef VIPRE_COLOR_TABLES_HPP
#define VIPRE_COLOR_TABLES_HPP

#include <osg/Texture1D>

#include <vipre/Export.hpp>

namespace vipre {

/** Forward declarations. */
class String;

class VIPRE_EXPORT ColorTables
{
public:

	/** Constructor. */
	ColorTables();

	/** Destructor. */
	~ColorTables();

	/** Generates all the color tables and pushes the textures to the shaders. */
	void generateAllTables();

	/** Loads the appropriate color table into the shader. */
	void loadColorTable(String colorTable, osg::ref_ptr<osg::StateSet> stateset, unsigned int textureUnit);

protected:

	/** Creates the bone color table. */
	void createBoneTable();

	/** Creates the cardiac color table. */
	void createCardiacTable();

	/** Creates the ge color table. */
	void createGETable();

	/** Creates the grayscale color table. */
	void createGrayscaleTable();

	/** Creates the muscle and bone color table. */
	void createMuscleAndBoneTable();

	/** Creates the nih color table. */
	void createNIHTable();

	/** Creates the red vessels color table. */
	void createRedVesselsTable();

	/** Creates the stern color table. */
	void createSternTable();

	/** Adds the given rgb values into the color array at the given index. */
	void addRGBPoint(unsigned int index, float* colors, float r, float g, float b);

	/** Creates a 1D texture for the given color array. */
	void buildTexture(osg::ref_ptr<osg::Texture1D> texture, float* colors);

	/** Instance member variables. */
    std::map<String, osg::ref_ptr<osg::Texture1D> > _textures;
};

}   // End of vipre namespace

#endif	// End of VIPRE_COLOR_TABLES_HPP
