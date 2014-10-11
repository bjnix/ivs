#pragma once
#include <math.h>

class Vector4{
public:
	Vector4(){
		_x = 0;		_y = 0;		_z = 0;		_w = 0;
	}
	Vector4(float s){
		_x = s;		_y = s;		_z = s;		_w = s;
	}
	Vector4(float x, float y, float z, float w){
		_x = x;		_y = y;		_z = z;		_w = w;
	}

	float* ptr(){	return &_x;	}
	
	static Vector4 add(Vector4 left, Vector4 right){
		return Vector4(left[0] + right[0], left[1] + right[1], left[2] + right[2], left[3] + right[3]);
	}

	static Vector4 sub(Vector4 left, Vector4 right){
		return Vector4(left[0] - right[0], left[1] - right[1], left[2] - right[2], left[3] - right[3]);
	}

	// Helpers
	void normalize();
	static Vector4& normalize(Vector4& source);	

	// Operator overloads
	float& operator[](int idx);	
	Vector4 & operator*=(const Vector4 &rhs);

	union 
	{
		struct{float _x, _y, _z, _w;};
	};
};
