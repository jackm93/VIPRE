/*
 *  composite_raycasting.frag
 *  vipre
 *
 *  Created by Christian Noon on 2/12/12.
 *  Copyright 2012 Christian Noon. All rights reserved.
 *
 */

// Vertex shader varying values
varying vec4 vertex;
varying vec4 color;

// Basic data type uniforms
uniform vec3 voxelDimensions;
uniform vec3 cuboidDimensions;
uniform vec3 inverseVoxelSpacing;
uniform float paddedDepth;
uniform mat4 viewToWorldMatrix;
uniform float minIntensity;
uniform float maxIntensity;
uniform float sampleSize;
uniform vec2 windowDimensions;

// Texture uniforms
uniform sampler3D voxels;
uniform sampler3D octree;
uniform sampler2D backfaces;
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

// Computing ray/bounding box intersections
vec3 calculateBackPoint(Ray ray);
vec3 preMult(vec3 v, mat4 mat);
void computeIntersections();
void computePreciseIntersections();

// Converting intensities to opacity and color
float computeIntensity(vec3 pos);
float computeOpacity(float intensity);
vec4 computeColor(float intensity);

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
	// Composite raycasting
    // computeIntersections();
    computePreciseIntersections();
    renderComposite();
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

void computePreciseIntersections()
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
		startPt = vertex.xyz;
		float width = windowDimensions.x;
		float height = windowDimensions.y;
		float x_coord = clamp(gl_FragCoord.x / width, 0.0, 1.0);
		float y_coord = clamp(gl_FragCoord.y / height, 0.0, 1.0);
		
		// This is a toggle between using the vertices directly in the backfaces
		// texture or whether they have been normalized. If we normalize them, we
		// can show a nice colorful overlay.
		// endPt = texture2D(backfaces, vec2(x_coord, y_coord)).xyz;
		vec3 normalizedEndPt = texture2D(backfaces, vec2(x_coord, y_coord)).xyz;
		endPt = normalizedEndPt * cuboidDimensions;

        // Make sure the end point was sampled okay
        if (endPt.rgb == vec3(0.0))
            discard;
	}

    // Not sure if I need this in here
    // endPt = clamp(endPt, vec3(0.0), cuboidDimensions);
    // startPt = clamp(startPt, vec3(0.0), cuboidDimensions);

    // Compute the total iterations and step vector
    totalIterations = ceil(length(endPt - startPt)) / sampleSize;
	totalIterations = clamp(totalIterations, 2.0, 2048.0);
    step = (endPt - startPt) / float(totalIterations - 1.0);
}

// Computes the intensity of the voxel at the given position
float computeIntensity(vec3 position)
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
    return texture3D(voxels, vec3(x, y, z)).r;
}

// Computes the opacity from the opacity table given the intensity
float computeOpacity(float intensity)
{
    float actual = (intensity - minIntensity) / intensityRange;
    return texture1D(opacityTable, actual).r;
}

// Computes the color from the color table given the intensity
vec4 computeColor(float intensity)
{
	intensity = (intensity - minIntensity) / intensityRange;
	return texture1D(colorTable, intensity);
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
            float intensity = computeIntensity(pos);

            // Get the opacity for the given intensity
            float opacity = computeOpacity(intensity);

            // Update the destColor and remainingOpacity if the voxel is at all opaque
            if (opacity > 0.0)
            {
                color = computeColor(intensity);
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
