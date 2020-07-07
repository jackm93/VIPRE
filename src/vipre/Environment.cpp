//
//  Environment.cpp
//  vipre
//
//  Created by Christian Noon on 12/13/11.
//  Copyright (c) 2011 Christian Noon. All rights reserved.
//

#ifdef _MSC_VER
    #include <direct.h>
#endif

#include <vipre/Environment.hpp>

vipre::String vipre::getEnvironmentVariable(const String& variable)
{
    char* variableValue;
    vipre::String result;

#ifdef _MSC_VER
	size_t length;
	_dupenv_s(&variableValue, &length, variable.c_str());
#else
	variableValue = getenv(variable.c_str());
#endif

    if (variableValue)
    {
        result = vipre::String(variableValue);
    }

    return result;
}

vipre::String vipre::getCurrentWorkingDirectory()
{
	vipre::String directory;

#ifdef _MSC_VER
    directory = vipre::String(_getcwd(NULL, 0));
#else
    directory = vipre::String(getcwd(NULL, 0));
#endif

	return directory;
}
