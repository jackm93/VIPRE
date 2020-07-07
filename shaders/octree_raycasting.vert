/*
 *  octree_raycasting.vert
 *  vipre
 *
 *  Created by Christian Noon on 2/12/12.
 *  Copyright 2012 Christian Noon. All rights reserved.
 *
 */

varying vec4 vertex;
varying vec4 color;

void main()
{
	vertex = gl_Vertex;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	color = gl_Color;
	gl_FrontColor = gl_Color;
}
