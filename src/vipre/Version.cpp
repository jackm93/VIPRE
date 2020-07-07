//
//  Version.cpp
//  vipre
//
//  Created by Christian Noon on 12/2/11.
//  Copyright (c) 2011 Christian Noon. All rights reserved.
//

#include <vipre/Version.hpp>

vipre::String vipre::vipreGetVersion()
{
    static bool shouldInitialize = true;
    static vipre::String vipreVersion;
    if (shouldInitialize)
    {
        if (VIPRE_SOVERSION == 0)
        {
			vipreVersion << VIPRE_MAJOR_VERSION << "." << VIPRE_MINOR_VERSION << "." << VIPRE_PATCH_VERSION;
        }
        else
        {
			vipreVersion << VIPRE_MAJOR_VERSION << "." << VIPRE_MINOR_VERSION << "." << VIPRE_PATCH_VERSION << "-" << VIPRE_SOVERSION;
        }

        shouldInitialize = false;
    }

    return vipreVersion;
}

vipre::String vipre::vipreGetSOVersion()
{
    static bool shouldInitialize = true;
    static vipre::String vipreSOVersion;
    if (shouldInitialize)
    {
		vipreSOVersion << VIPRE_SOVERSION;
        shouldInitialize = false;
    }

    return vipreSOVersion;
}

vipre::String vipre::vipreGetLibraryName()
{
    return "VIPRE Library";
}
