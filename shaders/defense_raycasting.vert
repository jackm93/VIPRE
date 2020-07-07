/*
 *  defense_raycasting.vert
 *  vipre
 *
 *  Created by Christian Noon on 2/25/12.
 *  Copyright 2012 Christian Noon. All rights reserved.
 *
 */

 varying vec4 vertex;
 varying vec4 color;
 varying vec4 diffuseLight;
 varying vec4 specularLight;
 varying vec4 ambientLight;
 varying float shininess;
 varying bool insideVolume;
 void main()
 {	
 	// Set the lighting parameters for use in the frag shader
 	diffuseLight = gl_LightSource[0].diffuse;
 	specularLight = gl_LightSource[0].specular;
 	ambientLight = gl_LightSource[0].ambient;
 	diffuseLight = vec4(vec3(1.0), 1.0);
 	specularLight = vec4(vec3(0.0), 1.0);
 	ambientLight = vec4(vec3(0.2), 1.0);
 	shininess = 0.0; // 0.0 - 128.0 are valid nums

 	// Original shader
 	vertex = gl_Vertex;
 	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
 	color = gl_Color;
 	gl_FrontColor = gl_Color;
 }
