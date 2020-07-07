//
//  Export.hpp
//  vipreDICOM
//
//  Created by Christian Noon on 12/2/11.
//  Copyright (c) 2011 Christian Noon. All rights reserved.
//

#ifndef VIPREDICOM_EXPORT_HPP
#define VIPREDICOM_EXPORT_HPP

// Disable VisualStudio warnings
#if defined(_MSC_VER) && defined(VIPRE_DISABLE_MSVC_WARNINGS)
    #pragma warning(disable:4244)
    #pragma warning(disable:4251)
    #pragma warning(disable:4275)
    #pragma warning(disable:4512)
    #pragma warning(disable:4267)
    #pragma warning(disable:4702)
    #pragma warning(disable:4511)
#endif

// Define the dllexport and dllimports for Windows
#if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined( __BCPLUSPLUS__) || defined( __MWERKS__)
    #if defined(VIPRE_LIBRARY_STATIC)
        #define VIPREDICOM_EXPORT
    #elif defined(VIPREDICOM_LIBRARY)
        #define VIPREDICOM_EXPORT __declspec(dllexport)
    #else
        #define VIPREDICOM_EXPORT __declspec(dllimport)
    #endif
#else
    #define VIPREDICOM_EXPORT
#endif

// Set up define for whether member templates are supported by VisualStudio compilers
#ifdef _MSC_VER
    #if (_MSC_VER >= 1300)
        #define __STL_MEMBER_TEMPLATES
    #endif
#endif

// Define NULL pointer value
#ifndef NULL
    #ifdef  __cplusplus
        #define NULL 0
    #else
        #define NULL ((void *)0)
    #endif
#endif

/**

 \namespace vipreDICOM

 The vipreDICOM library is used to provide an abstraction layer between DCMTK and OSG. The library can
 extract header and voxel information from all types of DICOM files as well as sort all the slices into
 unique series objects. The SeriesBuilder is also able to rank a list of series objects in order from
 highest to lowest quality. The vipreDICOM library supports DICOM files with 1, 3, and 4 channels of
 data. Once a unique series has been located, the voxel data can be passed off to the vipre library to
 be rendered.

 */

#endif  // End of VIPREDICOM_EXPORT_HPP
