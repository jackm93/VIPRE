/*
 *  light_raycasting.frag
 *  vipre
 *
 *  Created by Christian Noon on 2/12/12.
 *  Copyright 2012 Christian Noon. All rights reserved.
 *
 */

// Vertex shader varying values
varying vec4 vertex;
varying vec4 color;
varying vec4 diffuseLight;
varying vec4 specularLight;
varying vec4 ambientLight;
varying float shininess;

// Basic data type uniforms
uniform vec3 voxelDimensions;
uniform vec3 cuboidDimensions;
uniform vec3 inverseVoxelSpacing;
uniform float paddedDepth;
uniform mat4 viewToWorldMatrix;
uniform float minIntensity;
uniform float maxIntensity;
uniform float sampleSize;
uniform vec3 lightPosition;

// Texture uniforms
uniform sampler3D voxels;
uniform sampler3D octree;
uniform sampler3D normals;
uniform sampler1D colorTable;
uniform sampler1D opacityTable;

// Helper struct to store ray info
struct Ray
{
	vec3 origin;
	vec3 dir;
	vec3 inverseDir;
	vec3 sign;
};

// Tests the uniforms for correctness
void checkUpOnInput();

// Computing ray/bounding box intersections
vec3 calculateBackPoint(Ray ray);
vec3 preMult(vec3 v, mat4 mat);
void computeIntersections();

// Converting intensities to opacity and color
float computeIntensity(vec3 pos);
float rand(vec3 co);
float computeOpacity(float intensity);
vec3 computeColor(float intensity);
vec4 phongShading(vec3 position, vec3 texturePosition, float intensity, float opacity);

// Render algorithms
void renderShell();
void renderComposite();

// Global variables
vec3 startPt;
vec3 endPt;
float totalIterations;
vec3 step;
vec4 destColor;
float remainingOpacity;

// Helper variables that should be uniforms
vec3 voxelDimensionsMinusOne = voxelDimensions - 1.0; // Helper for quicker access
float intensityRange = maxIntensity - minIntensity;
float paddedDepthOffset = voxelDimensionsMinusOne.z / (paddedDepth + voxelDimensions.z);

void main()
{
	// Tests the inputs to the shader for correctness
    //checkUpOnInput();

	// Renders the original cuboid colors
    //gl_FragColor = color;
	//gl_FragColor = vec4(rand(vec3(0.3, 0.6, 0.9)), 1.0);
	//gl_FragColor = gl_FrontColor;

	// Renders the volume shell
	//renderShell();
	
	// Composite raycasting
    
	
	computeIntersections();
    //gl_FragColor = vec4(gl_FragCoord.y/1000.0, gl_FragCoord.y/1000.0, gl_FragCoord.y/1000.0, 1);
	
	//renderComposite();
	
	
	gl_FragColor = texture3D(normals, vec3(100,100,100));
	
	
}

// Performs a bunch of checks to make sure the data is coming in correctly
void checkUpOnInput()
{
	vec4 good = vec4(0.0, 1.0, 0.0, 1.0);
	vec4 bad = vec4(1.0, 0.0, 0.0, 0.2);
	
	// // paddedDepth check
	// if (paddedDepth == 64)
	// 	gl_FragColor = good;
	// else
	// 	gl_FragColor = bad;

	// cuboidDimensions
	if (cuboidDimensions.x == 345.0 && cuboidDimensions.y == 345.0 && cuboidDimensions.z == 257.0)
		gl_FragColor = good;
		//gl_FragColor = 
	else
		gl_FragColor = bad;

	// voxelDimensions
	if (voxelDimensions.x == 512.0 && voxelDimensions.y == 512.0 && voxelDimensions.z == 257.0)
		gl_FragColor = good;
	else
		gl_FragColor = bad;
}

// Renders the outer shell of the volume
void renderShell()
{
	float intensity = computeIntensity(vertex.xyz);
	gl_FragColor =  vec4(intensity, intensity, intensity, 1.0);
}

// Computes the back intersection point of the ray and the bounding box
vec3 calculateBackPoint(Ray ray)
{
    vec3 parameters[2];
    parameters[0] = vec3(0.0, 0.0, 0.0);
    parameters[1] = cuboidDimensions;

    vec3 minPt, maxPt;
    minPt.x = (parameters[int(ray.sign.x)].x - ray.origin.x) * ray.inverseDir.x;
    maxPt.x = (parameters[1-int(ray.sign.x)].x - ray.origin.x) * ray.inverseDir.x;
    minPt.y = (parameters[int(ray.sign.y)].y - ray.origin.y) * ray.inverseDir.y;
    maxPt.y = (parameters[1-int(ray.sign.y)].y - ray.origin.y) * ray.inverseDir.y;
    if (minPt.x > maxPt.y || minPt.y > maxPt.x)
        discard;
    if (minPt.y > minPt.x)
        minPt.x = minPt.y;
    if (maxPt.y < maxPt.x)
        maxPt.x = maxPt.y;
    minPt.z = (parameters[int(ray.sign.z)].z - ray.origin.z) * ray.inverseDir.z;
    maxPt.z = (parameters[1-int(ray.sign.z)].z - ray.origin.z) * ray.inverseDir.z;
    if (minPt.x > maxPt.z || minPt.z > maxPt.x)
        discard;
    if (minPt.z > minPt.x)
        minPt.x = minPt.z;
    if (maxPt.z < maxPt.x)
        maxPt.x = maxPt.z;

    // Returns the end point
    return ray.dir * maxPt.x + ray.origin;
}

// Pre-multiplies the vec3 with the mat4
vec3 preMult(vec3 v, mat4 mat)
{
    float d = 1.0 / (mat[0][3]*v.x+mat[1][3]*v.y+mat[2][3]*v.z+mat[3][3]);
    return vec3((mat[0][0]*v.x + mat[1][0]*v.y + mat[2][0]*v.z + mat[3][0])*d,
                (mat[0][1]*v.x + mat[1][1]*v.y + mat[2][1]*v.z + mat[3][1])*d,
                (mat[0][2]*v.x + mat[1][2]*v.y + mat[2][2]*v.z + mat[3][2])*d);
}

// Computes the start and end point of the ray
void computeIntersections()
{
	// Find the camera position
	vec3 cameraPosition = preMult(vec3(gl_FragCoord.x, gl_FragCoord.y, 0.0), viewToWorldMatrix);
	if (all(greaterThanEqual(cameraPosition, vec3(0.0, 0.0, 0.0))) && all(lessThanEqual(cameraPosition, cuboidDimensions)))
	{
		// Camera is inside the volume so the start point is the camera position and the end point is the back-face vertex
        startPt = preMult(vec3(gl_FragCoord.x, gl_FragCoord.y, 0.0), viewToWorldMatrix);
        endPt = vertex.xyz;
	}
	else
	{
        // Compute the back intersection point
        Ray ray;
        ray.origin = vertex.xyz; // Faster than using preMult at the zNear clipping plane
        vec3 zFarPt = preMult(vec3(gl_FragCoord.x, gl_FragCoord.y, 1.0), viewToWorldMatrix);
        ray.dir = zFarPt - ray.origin;
        ray.inverseDir = vec3(1.0 / ray.dir.x, 1.0 / ray.dir.y, 1.0 / ray.dir.z);
        ray.sign = vec3(int(ray.inverseDir.x < 0.0), int(ray.inverseDir.y < 0.0), int(ray.inverseDir.z < 0.0));
        startPt = vertex.xyz;
        endPt = calculateBackPoint(ray);
	}

    // Compute the total iterations and step vector
    totalIterations = ceil(length(endPt - startPt)) / sampleSize;
	totalIterations = clamp(totalIterations, 2.0, 2048.0);
    step = (endPt - startPt) / float(totalIterations - 1.0);
}

// Converts the position from object coords to texture sampler coordinates
vec3 convertPositionToTextureCoords(vec3 position)
{
    // Adjust the position with the inverse voxel spacing
    position.x = position.x * inverseVoxelSpacing.x;
    position.y = position.y * inverseVoxelSpacing.y;
    position.z = position.z * inverseVoxelSpacing.z;

	// Adjust the z position by the padded depth
    position.z *= paddedDepthOffset;

    // Compute the intensity value for the voxel location
    float x = position.x / voxelDimensionsMinusOne.x;
    float y = position.y / voxelDimensionsMinusOne.y;
    float z = position.z / voxelDimensionsMinusOne.z;

	return vec3(x, y, z);
}

// Computes the intensity of the voxel at the given position
float computeIntensity(vec3 position)
{
    return texture3D(voxels, position).r;
}
float rand (vec3 co){
    return fract(sin(dot(co.xyz ,vec3(12.9898,78.233, 4.5325936))) * 43758.5453);
}
// Computes the opacity from the opacity table given the intensity
float computeOpacity(float intensity)
{
    float actual = (intensity - minIntensity) / intensityRange;
    return texture1D(opacityTable, actual).r;
}

/**
 * Computes the opacity gradient using forward differences
 * @param position - position of the voxel in texture coordinates
 * @param opacity - opacity of the voxel
 */
vec3 computeOpacityGradient(vec3 position, float opacity)
{
    // Compute the offset in each direction for neighboring voxels
    vec3 offset = vec3(1.0 / voxelDimensions.x, 1.0 / voxelDimensions.y, 1.0 / voxelDimensions.z);

    // Compute the exact position of each neighboring voxel
    vec3 px = position + vec3(offset.x, 0.0, 0.0);
    vec3 py = position + vec3(0.0, offset.y, 0.0);
    vec3 pz = position + vec3(0.0, 0.0, offset.z);

    // Compute the intensity at each of the neighboring voxels
    float ix = computeIntensity(px);
    float iy = computeIntensity(py);
    float iz = computeIntensity(pz);

    // Compute the opacity at each of the neighboring voxels
    float ox = computeOpacity(ix);
    float oy = computeOpacity(iy);
    float oz = computeOpacity(iz);

    // Compute the opacity gradient
    vec3 gradient = vec3(opacity - ox, opacity - oy, opacity - oz);

    return gradient;
}

/**
 * Computes the intensity gradient using forward differences
 * @param position - position of the voxel in texture coordinates
 * @param intensity - intensity of the voxel
 */
vec3 computeIntensityGradient(vec3 position, float intensity)
{
    // Compute the offset in each direction for neighboring voxels
    vec3 offset = vec3(1.0 / voxelDimensions.x, 1.0 / voxelDimensions.y, 1.0 / voxelDimensions.z);

    // Compute the exact position of each neighboring voxel
    vec3 px = position + vec3(offset.x, 0.0, 0.0);
    vec3 py = position + vec3(0.0, offset.y, 0.0);
    vec3 pz = position + vec3(0.0, 0.0, offset.z);

    // Compute the intensity at each of the neighboring voxels
    float ix = computeIntensity(px);
    float iy = computeIntensity(py);
    float iz = computeIntensity(pz);

    // Compute the opacity gradient
    vec3 gradient = vec3(intensity - ix, intensity - iy, intensity - iz);

    return gradient;
}

// Computes the intensity gradient using central difference
vec3 computeAccurateIntensityGradient(vec3 position)
{
    // Compute the offset in each direction for neighboring voxels
    vec3 offset = vec3(1.0 / voxelDimensions.x, 1.0 / voxelDimensions.y, 1.0 / voxelDimensions.z);

    // Compute the front/back neighboring voxels
    vec3 fx = position + vec3(offset.x, 0.0, 0.0);
    vec3 fy = position + vec3(0.0, offset.y, 0.0);
    vec3 fz = position + vec3(0.0, 0.0, offset.z);
    vec3 bx = position - vec3(offset.x, 0.0, 0.0);
    vec3 by = position - vec3(0.0, offset.y, 0.0);
    vec3 bz = position - vec3(0.0, 0.0, offset.z);

    // Compute the front/back intensities at each of the neighboring voxels
    float ifx = computeIntensity(fx);
    float ify = computeIntensity(fy);
    float ifz = computeIntensity(fz);
    float ibx = computeIntensity(bx);
    float iby = computeIntensity(by);
    float ibz = computeIntensity(bz);

    // Compute the intensity gradient
    vec3 gradient = vec3(ibx - ifx, iby - ify, ibz - ifz);

    return gradient;
}

// Computes the color from the color table given the intensity
vec3 computeColor(float intensity)
{
	intensity = (intensity - minIntensity) / intensityRange;
	return texture1D(colorTable, intensity).rgb;
}

/**
 * Computes the diffuse color.
 * @param color - sampled color fetched from the color table
 * @param gradient - normalized gradient
 * @param light - normalized light vector
 */
vec3 computeDiffuseColor(vec3 color, vec3 gradient, vec3 light)
{
    float GdotL = max(dot(gradient, light), 0.0);
    return color * diffuseLight.rgb * GdotL;
}

/**
 * Computes the specular color.
 * @param color - the specular color to be used
 * @param gradient - the computed gradient
 * @param light - the normalized light vector
 * @param view - the normalized view vector
 */
vec3 computeSpecularColor(vec3 color, vec3 gradient, vec3 light, vec3 view)
{
    vec3 H = normalize(view + light);
    float GdotH = pow(max(dot(gradient, H), 0.0), shininess);
    return color * specularLight.rgb * GdotH;
}

/**
 * Computes the ambient color.
 * @param color - sampled color fetched from the color table
 */
vec3 computeAmbientColor(vec3 color)
{
    return color * ambientLight.rgb;
}

/**
 * Calculates Phong shading.
 * @param position - position of the voxel in world coordinates
 * @param texturePosition - position of the voxel in texture coordinates
 * @param intensity - intensity of the voxel
 * @param opacity - opacity of the voxel
 */
vec4 phongShading(vec3 position, vec3 texturePosition, float intensity, float opacity)
{
    // Compute the color of the voxel without shading
    vec3 color = computeColor(intensity);

    // Compute the light and view directions
    // vec3 cameraPosition = preMult(vec3(gl_FragCoord.x, gl_FragCoord.y, 0.0), viewToWorldMatrix);
    // vec3 lightPosition = vec3(gl_LightSource[0].position);
    // vec3 lightDirection = normalize(lightPosition - position);
    vec3 lightDirection = normalize(lightPosition);
	// vec3 lightDirection = lightDir;
    // vec3 lightDirection = normalize(position - lightPosition);
    vec3 viewDirection = normalize(vec3(gl_FragCoord.x, gl_FragCoord.y, 0.0) - vec3(gl_FragCoord.x, gl_FragCoord.y, 1.0));
    // vec3 viewDirection = normalize(position - cameraPosition);ss

    // Compute the gradient
    // vec3 gradient = normalize(computeOpacityGradient(texturePosition, opacity));
	vec3 gradient = normalize(computeIntensityGradient(texturePosition, intensity));
	// vec3 gradient = normalize(computeAccurateIntensityGradient(texturePosition));

    // Compute the diffuse color using the sampled color
    vec3 shadedColor = vec3(0.0);
    shadedColor += computeDiffuseColor(color, gradient, lightDirection);
    shadedColor += computeSpecularColor(color, gradient, lightDirection, viewDirection);
    shadedColor += computeAmbientColor(color);

    return vec4(shadedColor, 1.0);
}

// Traverses the ray compositing non-opaque voxels
void renderComposite()
{
    // Initialize the destColor and remainingOpacity
    destColor = vec4(0.0, 0.0, 0.0, 0.0);
    remainingOpacity = 1.0 - destColor.a;
    vec3 pos = vec3(startPt.x, startPt.y, abs(startPt.z));
	vec3 minCornerBB = vec3(0.0, 0.0, 0.0);
	vec3 maxCornerBB = cuboidDimensions;

    // Initialize the local color and opacity variables
    vec4 color;

    while (totalIterations > 0.0)
    {
        while (totalIterations > 0.0)
        {
            // Get the intensity at the voxel position
            vec3 tex_pos = convertPositionToTextureCoords(pos);
            float intensity = computeIntensity(tex_pos);

            // Get the opacity for the given intensity
            float opacity = computeOpacity(intensity);

            // Update the destColor and remainingOpacity if the voxel is at all opaque
            if (opacity > 0.0)
            {
                // color = vec4(computeColor(intensity), 1.0);
                color = phongShading(pos, tex_pos, intensity, opacity);
				color = color * opacity;
                destColor = destColor + color * remainingOpacity;
                remainingOpacity = remainingOpacity * (1.0 - opacity);
            }

            // Update the position and total iterations
            pos = pos + step;
			totalIterations = totalIterations - 1.0;

			// Check to see if we are outside the bounding box or nearly opaque
            if (all(lessThanEqual(pos, minCornerBB)) ||
                all(greaterThanEqual(pos, maxCornerBB)) ||
                remainingOpacity < 0.0039) // 1/255 = 0.0039)
            {
                totalIterations = 0.0;
            }
        }
    }

    gl_FragColor = destColor;
    gl_FragColor.a = 1.0 - remainingOpacity;
}
