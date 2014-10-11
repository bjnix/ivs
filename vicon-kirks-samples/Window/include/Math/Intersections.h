#pragma once
#include "Helpers.h"

#define BIAS 0.00001f

typedef struct{
	float Position[3];
	float Direction[3];
} Ray;

float QuadIntersection(float* quad, float* normal, Ray ray){
	float center[3], P0[3], inter[3];
	float det, t;
	float minX = quad[0], minY = quad[1], minZ = quad[2];
	float maxX = quad[6], maxY = quad[7], maxZ = quad[8];

	if(quad[0] > quad[6]){
		minX = quad[6];
		maxX = quad[0];
	}

	if(quad[1] > quad[7]){
		minY = quad[7];
		maxY = quad[1];
	}

	if(quad[2] > quad[8]){
		minZ = quad[8];
		maxZ = quad[2];
	}

	VecDot(det, normal, ray.Direction);

	if(det > 0) return 0;

	// calculate the center of the quad
	VecAdd(center, quad, (quad + 6)); VecScl(center, 0.5f);

	// calulate the offset start position of the ray
	VecSub(P0, ray.Position, center);

	// solve for the intersection time of the ray with the plane
	VecDot(t, P0, normal);
	t = -t / det;

	if(t <= 0) return 0;

	VecScl(ray.Direction, t);
	VecAdd(P0, P0, ray.Direction);
	VecAdd(inter, P0, center);

	if(inter[0] >= maxX + BIAS || inter[0] <= minX - BIAS) return 0;
	if(inter[1] >= maxY + BIAS || inter[1] <= minY - BIAS) return 0;
	if(inter[2] >= maxZ + BIAS || inter[2] <= minZ - BIAS) return 0;

	return t;
}