//
//  Enums.hpp
//  vipre
//
//  Created by Christian Noon on 12/15/11.
//  Copyright (c) 2011 Christian Noon. All rights reserved.
//

#ifndef VIPRE_ENUMS_HPP
#define VIPRE_ENUMS_HPP

namespace vipre {

enum RenderMode
{
    INTERACTIVE_RENDER_MODE,
    STATIC_RENDER_MODE
};

enum RenderOrder
{
    PRE_RENDER_ORDER,
    POST_RENDER_ORDER
};

enum RenderResolution
{
    LOW_RESOLUTION,
    LOW_MEDIUM_RESOLUTION,
    MEDIUM_RESOLUTION,
    MEDIUM_HIGH_RESOLUTION,
    HIGH_RESOLUTION
};

}   // End of vipre namespace

#endif  // End of VIPRE_ENUMS_HPP
