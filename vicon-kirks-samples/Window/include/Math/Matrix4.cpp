#include "Matrix4.h"

Matrix4::Matrix4(
	Vector4& col0,
	Vector4& col1,
	Vector4& col2,
	Vector4& col3){
		cols[0] = col0;
		cols[1] = col1;
		cols[2] = col2;
		cols[3] = col3;
}

Matrix4::Matrix4(Vector3& col0, Vector3& col1, Vector3& col2){
	cols[0] = Vector4(col0[0], col0[1], col0[2], 0);
	cols[1] = Vector4(col1[0], col1[1], col1[2], 0);
	cols[2] = Vector4(col2[0], col2[1], col2[2], 0);
	cols[3] = Vector4(0, 0, 0, 1);
}

Matrix4::Matrix4(
	float c00,float c01,float c02,float c03,
	float c10,float c11,float c12,float c13,
	float c20,float c21,float c22,float c23,
	float c30,float c31,float c32,float c33){
		cols[0][0] = c00; cols[0][1] = c01; cols[0][2] = c02; cols[0][3] = c03;
		cols[1][0] = c10; cols[1][1] = c11; cols[1][2] = c12; cols[1][3] = c13;
		cols[2][0] = c20; cols[2][1] = c21; cols[2][2] = c22; cols[2][3] = c23;
		cols[3][0] = c30; cols[3][1] = c31; cols[3][2] = c32; cols[3][3] = c33;
}

Matrix4 Matrix4::Multiply(Matrix4 m){
	Matrix4 c = m;
	float sum;
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 4; j++){
			sum = 0;
			for(int k = 0; k < 4; k++){
				sum += m.cols[i][k] * cols[k][j];
			}
			c.cols[i][j] = sum;
		}
	}
	
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 4; j++){
			cols[i][j] = c[i][j];
		}
	}
	return c;
}

Vector4 Matrix4::Multiply(Vector4 v){
	Vector4 out(0);

	for(int i = 4; i--;)
		for(int j = 4; j--;){
			out[i] += cols[j][i] * v[j];
		}
	return out;
}

Matrix4 Matrix4::RotateX(float x){
	float t1 = cos(x);
	float t2 = sin(x);
	return Matrix4(
		1,0,0,0,
		0, t1, t2, 0,
		0, -t2, t1, 0,
		0,0,0,1
		);
}

Matrix4 Matrix4::RotateY(float x){
	float t1 = cos(x);
	float t2 = sin(x);
	return Matrix4(
		t1,0,-t2,0,
		0, 1, 0, 0,
		t2, 0, t1, 0,
		0,0,0,1
		);
}

Matrix4 Matrix4::RotateZ(float x){
	float t1 = cos(x);
	float t2 = sin(x);
	return Matrix4(
		t1, t2, 0, 0,
		-t2, t1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
		);
}

Matrix4 Matrix4::Translate(float x, float y, float z){
	return Matrix4(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		x, y, z, 1
		);
}
Matrix4 Matrix4::Scale(float x){
	return Matrix4(
		x, 0, 0, 0,
		0, x, 0, 0,
		0, 0, x, 0,
		0, 0, 0, 1
		);
}
Matrix4 Matrix4::Scale(float x, float y, float z){
	return Matrix4(
		x, 0, 0, 0,
		0, y, 0, 0,
		0, 0, z, 0,
		0, 0, 0, 1
		);
}

void Matrix4::SetIdentity(){
	cols[0] = Vector4(1,0,0,0);
	cols[1] = Vector4(0,1,0,0);
	cols[2] = Vector4(0,0,1,0);
	cols[3] = Vector4(0,0,0,1);
}


Matrix4 Matrix4::CreatePerspective(float fovy, float aspect, float near, float far){

	float fovyD = (fovy/2.0f) * (M_PI / 180);
	float range = tan(fovyD) * near;
	float left = -range * aspect;
	float right = range * aspect;
	float bottom = -range;
	float top = range;

	Matrix4 m;
	m[0][0] = ( 2 * near ) / ( right - left );
	m[1][1] = ( 2 * near ) / ( top - bottom );
	m[2][2] = - ( far + near ) / ( far - near);
	m[2][3] = - 1;
	m[3][2] = - (2 * far * near) / ( far - near );
		
    return m;
}

Matrix4 Matrix4::CreateLook(Vector3 campos, Vector3 look, Vector3 up) {
	Vector3 f = Vector3::sub(look, campos); f.normalize();
	up.normalize();
	Vector3 s = Vector3::cross(f, up);
	s.normalize();
	Vector3 u = Vector3::cross(s, f);

	Matrix4 result;
	result.SetIdentity();
		
	for(int i = 0; i < 3; i++){
		result[i][0] = s[i];
	}
	for(int i = 0; i < 3; i++){
		result[i][1] = u[i];
	}
	for(int i = 0; i < 3; i++){
		result[i][2] = -f[i];
	}
		
	Matrix4 trans;
	trans.SetIdentity();
	trans[3][0] = -campos[0];
	trans[3][1] = -campos[1];
	trans[3][2] = -campos[2];

	result *= trans;
    return result;
}

Matrix4 Matrix4::CreateOrthographic( const float left, const float right,
	    const float bottom, const float top,
	    const float zNear, const float zFar )
{
    Matrix4 c;
    c[0][0] = 2.0/(right - left);
    c[1][1] = -2.0/(top - bottom);
    c[2][2] = 2.0/(zNear - zFar);
    c[3][3] = 1.0;
    c[3][0] = -(right + left)/(right - left);
    c[3][1] = -(top + bottom)/(top - bottom);
    c[3][2] = -(zFar + zNear)/(zFar - zNear);
	//c.transpose();
    return c;
}

void Matrix4::transpose(){
	Matrix4 tmp;

	// Get the transpose into a temporary Matrix
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 4; j++){
			tmp[i][j] = cols[j][i];
		}
	}

	// Put the temporary variable into the current
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 4; j++){
			cols[i][j] = tmp[i][j];
		}
	}
}
	
Matrix4 & Matrix4::operator*=(const Matrix4 &rhs) {
	cols[0] *= rhs.cols[0];
	cols[1] *= rhs.cols[1];
	cols[2] *= rhs.cols[2];
	cols[3] *= rhs.cols[3];
	return *this;
}

Vector4& Matrix4::operator[](int idx){		
	switch(idx){
	case 0:
		return cols[0];
		break;
	case 1:
		return cols[1];
		break;
	case 2:
		return cols[2];
		break;
	default:
		return cols[3];
		break;
	}
}

Matrix4 Matrix4::operator*(Matrix4& right)
{
	Matrix4 c = right;
	float sum;
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 4; j++){
			sum = 0;
			for(int k = 0; k < 4; k++){
				sum += cols[i][k] * right.cols[k][j];
			}
			c.cols[i][j] = sum;
		}
	}
	return c;
}

Matrix4 Matrix4::operator*=(Matrix4& right)
{
	Matrix4 c = right;
	float sum;
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 4; j++){
			sum = 0;
			for(int k = 0; k < 4; k++){
				sum += right.cols[i][k] * cols[k][j];
			}
			c.cols[i][j] = sum;
		}
	}
	
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 4; j++){
			cols[i][j] = c[i][j];
		}
	}
	return c;
}
