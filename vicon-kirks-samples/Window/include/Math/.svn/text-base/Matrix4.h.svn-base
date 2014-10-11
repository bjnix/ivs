#pragma once

#include "Vector3.h"
#include "Vector4.h"
#include "Vector3.h"

class Matrix4 {
public:
	Matrix4(){ SetIdentity(); }
	Matrix4(Vector4& col0, Vector4& col1, Vector4& col2, Vector4& col3);
	Matrix4(Vector3& col0, Vector3& col1, Vector3& col2);
	Matrix4(float c00,float c01,float c02,float c03,
			float c10,float c11,float c12,float c13,
			float c20,float c21,float c22,float c23,
			float c30,float c31,float c32,float c33);
	
	float* ptr(){	return cols[0].ptr(); }
	Matrix4 Multiply(Matrix4 m);
	Vector4 Multiply(Vector4 v);
	// Helper methods
	void SetIdentity();
	static Matrix4 RotateX(float x);
	static Matrix4 RotateY(float x);
	static Matrix4 RotateZ(float x);
	static Matrix4 Translate(float x, float y, float z);
	static Matrix4 Scale(float x);
	static Matrix4 Scale(float x, float y, float z);
	static Matrix4 CreatePerspective(float fovy, float aspect, float near, float far);
	static Matrix4 CreateOrthographic( const float left, const float right,
		const float bottom, const float top,
	    const float zNear, const float zFar );
	static Matrix4 CreateLook(Vector3 campos, Vector3 look, Vector3 up);
	void transpose();
	
	// Operator overloads
	Matrix4& operator*=(const Matrix4 &rhs);
	Vector4& operator[](int idx);
	Matrix4 operator*(Matrix4& right);
	Matrix4 operator*=(Matrix4& right);
private:
	// Data Structure 4 * 4 = 16 floats
	//		32 bit = 16 * 32 = 512 bits or 64 bytes
	//		64 bit = 16 * 64 = 1024 bits or 128 bytes
	Vector4 cols[4];
};

