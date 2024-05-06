#version 460

#define MAX_MARCHING_STEPS 100
#define MIN_EPSILON 0.01 // Threshold under which we consider that the ray has hit the object
#define MAX_RAY_LENGTH 1000000
#define FLT_MAX 3.402823466e+38

/* Uniform */
// uniform ivec2 u_viewportSize;
uniform mat4 u_viewMat;
// uniform mat4 u_projectionMat;
uniform float u_fieldOfView;

/* In */
layout(local_size_x = 16, local_size_y = 16) in;

/* Out */
layout(binding = 0, rgba32f) writeonly uniform image2D u_outTexture; // Output image

#include "../Common/PrimitiveSceneSDF.glsl"

mat4 buildTranslation(in float x, in float y, in float z)
{
    return mat4(
        vec4(1.0, 0.0, 0.0, 0.0),
        vec4(0.0, 1.0, 0.0, 0.0),
        vec4(0.0, 0.0, 1.0, 0.0),
        vec4(x, y, z, 1.0));
}

mat4 buildScale(float x, float y, float z){
    return mat4(
        vec4(x, 0., 0., 0.),
        vec4(0., y, 0., 0.),
        vec4(0., 0., z, 0.),
        vec4(0., 0., 0., 1.));
}

// Angle is in radians
mat4 rotationX( in float angle ) {
	return mat4(	1.0,		0,			0,			0,
			 		0, 	cos(angle),	-sin(angle),		0,
					0, 	sin(angle),	 cos(angle),		0,
					0, 			0,			  0, 		1);
}

// Angle is in radians
mat4 rotationY( in float angle ) {
	return mat4(	cos(angle),		0,		sin(angle),	0,
			 				0,		1.0,			 0,	0,
					-sin(angle),	0,		cos(angle),	0,
							0, 		0,				0,	1);
}

// Angle is in radians
mat4 rotationZ( in float angle ) {
	return mat4(	cos(angle),		-sin(angle),	0,	0,
			 		sin(angle),		cos(angle),		0,	0,
							0,				0,		1,	0,
							0,				0,		0,	1);
}

void main()
{
	/* Current pixel coordinates */
	const ivec2 currentPixel = ivec2(gl_GlobalInvocationID.xy);

    /* Output image properties */
    ivec2 dims = imageSize(u_outTexture);
	const float aspectRatio = float(dims.x) / float(dims.y);

    /* Coordinates of current pixel */
	const vec2 NDCmiddleOfCurrentPixel = (2. * ((vec2(currentPixel) + .5) / vec2(dims))) - 1.; // Pixel coordinate in screen space [-1, 1], and centered inside the current pixel

    /* Camera */
    const vec3 cameraOrigin = (inverse(u_viewMat) * vec4(0., 0., 0., 1)).xyz;
    const vec3 cameraToCurrentPixelDirection = vec3(NDCmiddleOfCurrentPixel * tan(u_fieldOfView / 2.) * vec2(aspectRatio, 1.), -1.); // Direction from the camera origin to the center of the current pixel
	const Ray ray = Ray(cameraOrigin, (inverse(u_viewMat) * normalize(vec4(cameraToCurrentPixelDirection, 0.))).xyz);

    /* Sphere Marching */
    float last_delta = 0.; // Last delta is added to the next step to implement Sphere oversteping
    float depth = 0.;
    for (int i = 0; i < MAX_MARCHING_STEPS; i++)
    {
        vec3 currentPos = ray.origin + (depth + last_delta) * ray.direction;
        vec3 hitColor;
        float minDistance = scanSDF(currentPos, hitColor);

        // oversteping failed : go back
        if (minDistance < last_delta) {

            currentPos = ray.origin + depth * ray.direction;
            minDistance = scanSDF(currentPos, hitColor);
        }

        // adaptative epsilon (alway keep an epsilon close to pixel size)
        float epsilon = max(MIN_EPSILON, length(currentPos) / float(max(dims.x, dims.y))); // 2 * max(res.x, res.y)

        // Detect a hit
        if (abs(minDistance) < epsilon)
        {
            // Compute normals
            float scan_epsilon = epsilon;
            float dx = scanSDF(currentPos + vec3(scan_epsilon, 0, 0), hitColor);
            float dy = scanSDF(currentPos + vec3(0, scan_epsilon, 0), hitColor);
            float dz = scanSDF(currentPos + vec3(0, 0, scan_epsilon), hitColor);
            vec3 hitNormal = normalize(vec3(minDistance - dx, minDistance - dy, minDistance - dz));

            float light = clamp(dot(hitNormal, normalize(vec3(1))), 0.2, 1.); // Cheap light calculation

            imageStore(u_outTexture, currentPixel, vec4(hitColor * light, 1));
            return;
        }

        float delta = abs(minDistance) - epsilon * 0.5; // float precision fix (to ensure the ray will stop before the surface)
        depth += delta;
        last_delta = delta;

        if (depth >= MAX_RAY_LENGTH) {
            imageStore(u_outTexture, currentPixel, vec4(0., 0., 0., 0.)); // background
            return;
        }
    }
    imageStore(u_outTexture, currentPixel, vec4(1., 0., 0., 1.)); // Draw red when we hit background
}