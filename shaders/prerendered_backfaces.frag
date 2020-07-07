/*
 *  prerendered_backfaces.frag
 *  vipre
 *
 *  Created by Christian Noon on 2/20/12.
 *  Copyright 2012 Christian Noon. All rights reserved.
 *
 */

// Vertex shader varying values
varying vec4 vertex;
varying vec4 color;

// Uniforms from the main program
uniform vec3 cuboidDimensions;

void main()
{
	float red = vertex.x / cuboidDimensions.x;
	float green = vertex.y / cuboidDimensions.y;
	float blue = vertex.z / cuboidDimensions.z;
	gl_FragData[0] = vec4(red, green, blue, 1.0);
	// gl_FragData[0] = vertex;
}
