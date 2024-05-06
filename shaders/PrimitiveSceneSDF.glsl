#ifndef PRIMITIVE_SCENE_SDF_GLSL_
#define PRIMITIVE_SCENE_SDF_GLSL_

#include "3DMaths.glsl"

#ifndef BINDING_SPHERES_BUFFER
    #define BINDING_SPHERES_BUFFER 0
#endif
#ifndef BINDING_TORUSES_BUFFER
    #define BINDING_TORUSES_BUFFER 1
#endif
#ifndef BINDING_CYLINDERS_BUFFER
    #define BINDING_CYLINDERS_BUFFER 2
#endif
#ifndef BINDING_BOXES_BUFFER
    #define BINDING_BOXES_BUFFER 3
#endif
#ifndef TYPE_SPHERE
    #define TYPE_SPHERE 1
#endif
#ifndef TYPE_TORUS
    #define TYPE_TORUS 2
#endif
#ifndef TYPE_CYLINDER
    #define TYPE_CYLINDER 3
#endif
#ifndef TYPE_BOX
    #define TYPE_BOX 4
#endif
#ifndef TYPE_INTERSECTION
    #define TYPE_INTERSECTION 5
#endif
#ifndef TYPE_UNION
    #define TYPE_UNION 6
#endif
#ifndef TYPE_DIFFERENCE
    #define TYPE_DIFFERENCE 7
#endif
#ifndef TYPE_COMPLEMENTARY
    #define TYPE_COMPLEMENTARY 8
#endif

/*************************************************
* Primitives definition
*************************************************/
struct Sphere
{
    mat4 inverseTransform;
    vec3 color;
    float radius;
};

struct Torus
{
    mat4 inverseTransform;
    vec3 color;
    float majorRadius; // Distance from the center of the torus to the center of the tube
    float minorRadius; // Radius of the tube
};

struct Cylinder
{
    mat4 inverseTransform;
    vec3 color;
    float height;
    float radius;
};

struct Box
{
    mat4 inverseTransform;
    vec3 color;
    vec3 size; // local length in X, Y and Z
};

struct Node
{
    int type;
    int leftChildIndex;
    int rightChildIndex;
    int primitiveIndex;
};

/*************************************************
* SSBOs
*************************************************/
layout(std430, binding = BINDING_SPHERES_BUFFER) buffer spheresSSBO
{
    Sphere spheresData[];
};

layout(std430, binding = BINDING_TORUSES_BUFFER) buffer torusesSSBO
{
    Torus torusesData[];
};

layout(std430, binding = BINDING_CYLINDERS_BUFFER) buffer cylinderSSBO
{
    Cylinder cylindersData[];
};

layout(std430, binding = BINDING_BOXES_BUFFER) buffer boxSSBO
{
    Box boxesData[];
};

layout(std430, binding = 4) buffer CSGSSBO
{
    Node nodesData[];
};

struct SmallNode
{
    vec3 color;
    float dist;
};

//@TODO Replace the local static array with a global SSBO and use the gl_WorkGroupID to 'allocate' individual stacks
layout(std430, binding = 9) buffer CSGSTACK
{
    SmallNode csgNodeStack[];
};

uniform int u_nbOfNode;

// Return the signed distance from a sphere
float sphereSDF(in Sphere sphere, in vec3 p)
{
    return length(p) - sphere.radius;
}

// Return the signed distance from a torus
float torusSDF(in Torus torus, in vec3 p)
{
    float x = length(p.xz) - torus.majorRadius;
    float y = p.y;
    return length(vec2(x, y)) - torus.minorRadius;
}

// Return the signed distance from a cylinder
float cylinderSDF(in Cylinder cylinder, in vec3 pos)
{
    float h = cylinder.height;
    float r = cylinder.radius;
    vec2 d = abs(vec2(length(pos.xz), pos.y)) - vec2(r, h);
    return min(max(d.x, d.y), 0.0) + length(max(d, 0.0));
}

// Return the signed distance from a box
float boxSDF(in Box box, in vec3 pos)
{
    vec3 b = box.size;
    vec3 q = abs(pos) - b;

    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

//  Place a primitive in the scene given its transformation matrix, by actually adapting the ray that is actually casted and not the primitive in itself
void transformRay(in vec3 worldPos, in mat4 inverseTransform, out vec3 localPos, out float scale)
{
    // Instead of moving the object, we are moving the ray so we keep simple SDF functions
    localPos = (inverseTransform * vec4(worldPos, 1.)).xyz;
    // Compute the scale of the transform
    vec3 scaleVec = vec3(length(inverseTransform[0]), length(inverseTransform[1]), length(inverseTransform[2]));
    scale = min(scaleVec.x, min(scaleVec.y, scaleVec.z));
}

// Intersection of 2 primitives
float intersectionSDF(in float a, in float b)
{
    return max(a,b);
}

// Union of 2 primitives
float unionSDF(in float a, in float b)
{
    return min(a,b);
}

// Difference of 2 primitives
float differenceSDF(in float a, in float b)
{
    return max(a,-b);
}

// Complementary of 1 primitive
float complementarySDF(in float a)
{
    return -a;
}

// Scan a node of the CSG tree and return its distance, begin at the root of the tree
void scanCSG(in int nodeIndex, in vec3 pos, int stackStartIndex)
{
    vec3 localPos;
    float scale;

    switch (nodesData[nodeIndex].type)
    {
    case TYPE_SPHERE:
    {
        // Procede to comptue sphere SDF and register distance and color for futur operations
        int index = nodesData[nodeIndex].primitiveIndex;

        Sphere sphere = spheresData[index];

        transformRay(pos, sphere.inverseTransform, localPos, scale);

        csgNodeStack[stackStartIndex + nodeIndex].color = sphere.color;

        float dist = sphereSDF(sphere, localPos) * scale;
        csgNodeStack[stackStartIndex + nodeIndex].dist = dist;
        break;
    }
    case TYPE_TORUS:
    {
        // Same principle for a torus, and the others primitives
        int index = nodesData[nodeIndex].primitiveIndex;

        Torus torus = torusesData[index];

        transformRay(pos, torus.inverseTransform, localPos, scale);

        csgNodeStack[stackStartIndex + nodeIndex].color = torus.color;

        float dist = torusSDF(torus, localPos) * scale;
        csgNodeStack[stackStartIndex + nodeIndex].dist = dist;
        break;
    }
    case TYPE_CYLINDER:
    {
        int index = nodesData[nodeIndex].primitiveIndex;

        Cylinder cylinder = cylindersData[index];

        transformRay(pos, cylinder.inverseTransform, localPos, scale);

        csgNodeStack[stackStartIndex + nodeIndex].color = cylinder.color;

        float dist = cylinderSDF(cylinder, localPos) * scale;
        csgNodeStack[stackStartIndex + nodeIndex].dist = dist;
        break;
    }
    case TYPE_BOX:
    {
        int index = nodesData[nodeIndex].primitiveIndex;

        Box box = boxesData[index];

        transformRay(pos, box.inverseTransform, localPos, scale);

        csgNodeStack[stackStartIndex + nodeIndex].color = box.color;

        float dist = boxSDF(box, localPos) * scale;
        csgNodeStack[stackStartIndex + nodeIndex].dist = dist;
        break;
    }
    case TYPE_INTERSECTION:
    {
        // consider left child and right child as sub CSG tree and analyse them, then calculate the intersection and store the result
        int index1 = nodesData[nodeIndex].leftChildIndex;
        int index2 = nodesData[nodeIndex].rightChildIndex;

        float a = csgNodeStack[stackStartIndex + index1].dist;
        float b = csgNodeStack[stackStartIndex + index2].dist;

        float result = intersectionSDF(a, b);

        if (result == a)
            csgNodeStack[stackStartIndex + nodeIndex].color = csgNodeStack[stackStartIndex + index1].color;
        else
            csgNodeStack[stackStartIndex + nodeIndex].color = csgNodeStack[stackStartIndex + index2].color;

        csgNodeStack[stackStartIndex + nodeIndex].dist = result;
        break;
    }
    case TYPE_UNION:
    {
        // consider left child and right child as sub CSG tree and analyse them, then calculate the union and store the result
        int index1 = nodesData[nodeIndex].leftChildIndex;
        int index2 = nodesData[nodeIndex].rightChildIndex;

        float a = csgNodeStack[stackStartIndex + index1].dist;
        float b = csgNodeStack[stackStartIndex + index2].dist;

        float result = unionSDF(a, b);

        if (result == a)
            csgNodeStack[stackStartIndex + nodeIndex].color = csgNodeStack[stackStartIndex + index1].color;
        else
            csgNodeStack[stackStartIndex + nodeIndex].color = csgNodeStack[stackStartIndex + index2].color;

        csgNodeStack[stackStartIndex + nodeIndex].dist = result;
        break;
    }
    case TYPE_DIFFERENCE:
    {
        // consider left child and right child as sub CSG tree and analyse them, then calculate the difference and store the result
        int index1 = nodesData[nodeIndex].leftChildIndex;
        int index2 = nodesData[nodeIndex].rightChildIndex;

        float a = csgNodeStack[stackStartIndex + index1].dist;
        float b = csgNodeStack[stackStartIndex + index2].dist;

        float result = differenceSDF(a, b);

        if (result == a)
            csgNodeStack[stackStartIndex + nodeIndex].color = csgNodeStack[stackStartIndex + index1].color;
        else
            csgNodeStack[stackStartIndex + nodeIndex].color = csgNodeStack[stackStartIndex + index2].color;

        csgNodeStack[stackStartIndex + nodeIndex].dist = result;
        break;
    }
    case TYPE_COMPLEMENTARY:
    {
        // unique child, we will consider it is the left child, then calculate the complementary and store the result
        int index = nodesData[nodeIndex].leftChildIndex;

        float a = csgNodeStack[stackStartIndex + index].dist;

        float result = complementarySDF(a);

        csgNodeStack[stackStartIndex + nodeIndex].color = vec3(0.f, 0.f, 0.f);

        csgNodeStack[stackStartIndex + nodeIndex].dist = result;
        break;
    }
    }
    return;
}

// Return smallest distance from a primitive
float scanSDF(vec3 pos, out vec3 hitColor)
{
    float minDistance = FLOAT_INFINITY;
    hitColor = vec3(0.);

    vec3 testcol = vec3(0,0,0);
    float testdist = FLOAT_INFINITY;

    //@TODO Replace the local static array with a global SSBO and use the gl_WorkGroupID to 'allocate' individual stacks
    ivec2 coords2D = ivec2(gl_GlobalInvocationID.xy);
    ivec2 dims = imageSize(u_outTexture);
    int stackStartIndex = (coords2D.x + coords2D.y * dims.x) * u_nbOfNode;

    // the entire primitive scene is defined by a csg tree, so we analyze all the node of this tree to render our primitives with their relations
    // because the first element of the ssbo is a leaf of the tree, we start from the start of ssbo and compute it until we reach the root
    for(int i = 0; i < u_nbOfNode; i++)
    {
        scanCSG(i, pos, stackStartIndex);
    }

    if(csgNodeStack[stackStartIndex + u_nbOfNode-1].dist < minDistance) // If the result of the CSG tree is closer than what is previously found
    {
        minDistance = csgNodeStack[stackStartIndex + u_nbOfNode-1].dist;
        hitColor = csgNodeStack[stackStartIndex + u_nbOfNode-1].color; // Retrive the color of the CSG result, for debug purpose
    }

    return minDistance;
}

#endif // PRIMITIVE_SCENE_SDF_GLSL_