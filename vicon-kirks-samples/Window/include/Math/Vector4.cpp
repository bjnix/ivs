#include "Vector4.h"

void Vector4::normalize(){
		float length = sqrt((_x * _x) + (_y * _y) + (_z * _z) + (_w * _w));
		_x /= length;
		_y /= length;
		_z /= length;
		_w /= length;
	}

Vector4& Vector4::normalize(Vector4& source){
	float length = sqrt((source._x * source._x) + (source._y * source._y) + (source._z * source._z) + (source._w * source._w));
	source._x /= length;
	source._y /= length;
	source._z /= length;
	source._w /= length;
	return source;
}

float& Vector4::operator[](int idx){
	switch(idx){
	case 0:
		return _x;
		break;
	case 1:
		return _y;
		break;
	case 2:
		return _z;
		break;
	default:
		return _w;
		break;
	}
}


Vector4 & Vector4::operator*=(const Vector4 &rhs) {
	_x *= rhs._x;
	_y *= rhs._y;
	_z *= rhs._z;
	_w *= rhs._w;
	return *this;
}
