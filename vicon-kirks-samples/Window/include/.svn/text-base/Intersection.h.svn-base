#pragma once
#ifndef INTERSECTION_MATH
#define INTERSECTION_MATH

//#include <GL/gl.h>
#include "ViconVec.h"

typedef struct{
	ViVec Position;
	ViVec Direction;
} Ray;

// returns the worlspace coordinate of the given ray's intersection
// point on the specified mathematical plane. If the intersection
// cannot be calculated (ie ray dir and plane tan are parallel) then
// the start position of the ray is returned.
ViVec InterPlane(Ray ray, ViVec planePos, ViVec normal, float *t){
	float det = dot(ray.Direction, normal);
	*t = 0;

	if(det == 0) return ray.Position;

	// orient the ray's starting pos
	ray.Position -= planePos;

	// solve for intersection time
	*t = -dot(ray.Position, normal)/det;
	
	// calculate the intersection point
	ViVec inter = ray.Position + (ray.Direction * *t);

	// return to world space
	return inter + planePos;
}

#endif
