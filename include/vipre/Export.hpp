//
//  Export.hpp
//  vipre
//
//  Created by Christian Noon on 12/2/11.
//  Copyright (c) 2011 Christian Noon. All rights reserved.
//

#ifndef VIPRE_EXPORT_HPP
#define VIPRE_EXPORT_HPP

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
        #define VIPRE_EXPORT
    #elif defined(VIPRE_LIBRARY)
        #define VIPRE_EXPORT __declspec(dllexport)
    #else
        #define VIPRE_EXPORT __declspec(dllimport)
    #endif
#else
    #define VIPRE_EXPORT
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

 \namespace vipre

 The core vipre library provides various volume rendering techinques (Raycasting, Orthogonal Texture Slicing, etc.)
 as well as all the support level classes such ClippingPlanes, BoundingBoxes, VolumeSlices, Shaders, etc.

 */

#endif  // End of VIPRE_EXPORT_HPP
