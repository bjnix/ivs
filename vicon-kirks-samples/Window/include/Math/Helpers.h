#pragma once
#include <stdlib.h>

#define VecSub(vecOut, vecPtr1, vecPtr2){\
	vecOut[0] = vecPtr1[0] - vecPtr2[0]; \
	vecOut[1] = vecPtr1[1] - vecPtr2[1]; \
	vecOut[2] = vecPtr1[2] - vecPtr2[2]; \
}                                        \

#define VecMul(vecOut, vecPtr1, vecPtr2){\
	vecOut[0] = vecPtr1[0] * vecPtr2[0]; \
	vecOut[1] = vecPtr1[1] * vecPtr2[1]; \
	vecOut[2] = vecPtr1[2] * vecPtr2[2]; \
}                                        \

#define Lerp(o, f1, f2, p){\
	o = ((1.0f - p) * f1) + (f2 * p);\
}\

#define VecLerp(vecOut, vecPtr1, vecPtr2, p){\
	vecOut[0] = ((1.0f - p) * vecPtr1[0]) + (vecPtr2[0] * p);\
	vecOut[1] = ((1.0f - p) * vecPtr1[1]) + (vecPtr2[1] * p);\
	vecOut[2] = ((1.0f - p) * vecPtr1[2]) + (vecPtr2[2] * p);\
}\	

#define VecCpy(vecDest, vecSrc){\
	vecDest[0] = vecSrc[0]; \
	vecDest[1] = vecSrc[1]; \
	vecDest[2] = vecSrc[2]; \
}   

#define VecAdd(vecOut, vecPtr1, vecPtr2){\
	vecOut[0] = vecPtr1[0] + vecPtr2[0]; \
	vecOut[1] = vecPtr1[1] + vecPtr2[1]; \
	vecOut[2] = vecPtr1[2] + vecPtr2[2]; \
}                                        \

#define VecScl(vec, s){\
	vec[0] *= s; vec[1] *= s; vec[2] *= s;\
}\

#define VecPrint(vec){\
	printf("<%.2f, %.2f, %.2f>", vec[0], vec[1], vec[2]);\
}                     \

#define VecLen(vec, len){\
	len = (float)sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);\
}\

#define VecNorm(vec){\
	float len = 0;\
	VecLen(vec, len);\
	VecScl(vec, 1.0f/len);\
}\

#define VecDirTo(vecOut, from, to){\
	VecSub(vecOut, to, from);\
	VecNorm(vecOut);\
}\

#define VecDot(dot, vecPtr1, vecPtr2) {dot = vecPtr1[0]*vecPtr2[0] + vecPtr1[1]*vecPtr2[1] + vecPtr1[2]*vecPtr2[2];}