//
//  Environment.hpp
//  vipre
//
//  Created by Christian Noon on 12/13/11.
//  Copyright (c) 2011 Christian Noon. All rights reserved.
//

#ifndef VIPRE_ENVIRONMENT_HPP
#define VIPRE_ENVIRONMENT_HPP

#include <vipre/Export.hpp>
#include <vipre/String.hpp>

namespace vipre {

/** Returns the value of the given environment variable as a vipre::String. */
extern VIPRE_EXPORT String getEnvironmentVariable(const String& variable);

/** Returns the current working directory as a vipre::String. */
extern VIPRE_EXPORT String getCurrentWorkingDirectory();

}   // End of vipre namespace

#endif  // End of VIPRE_ENVIRONMENT_HPP
