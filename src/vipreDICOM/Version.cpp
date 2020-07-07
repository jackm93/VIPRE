//
//  Version.cpp
//  vipreDICOM
//
//  Created by Christian Noon on 12/2/11.
//  Copyright (c) 2011 Christian Noon. All rights reserved.
//

#include <vipreDICOM/Version.hpp>

vipre::String vipreDICOM::vipreDICOMGetVersion()
{
    static bool shouldInitialize = true;
    static vipre::String version;
    if (shouldInitialize)
    {
        if (VIPREDICOM_SOVERSION == 0)
        {
			version << VIPREDICOM_MAJOR_VERSION << "." << VIPREDICOM_MINOR_VERSION << "." << VIPREDICOM_PATCH_VERSION;
        }
        else
        {
			version << VIPREDICOM_MAJOR_VERSION << "." << VIPREDICOM_MINOR_VERSION << "."
				<< VIPREDICOM_PATCH_VERSION << "-" << VIPREDICOM_SOVERSION;
        }

        shouldInitialize = false;
    }

    return version;
}

vipre::String vipreDICOM::vipreDICOMGetSOVersion()
{
    static bool shouldInitialize = true;
    static vipre::String soVersion;
    if (shouldInitialize)
    {
		soVersion << VIPREDICOM_SOVERSION;
        shouldInitialize = false;
    }

    return soVersion;
}

vipre::String vipreDICOM::vipreDICOMGetLibraryName()
{
    return "VIPRE DICOM Library";
}
