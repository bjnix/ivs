#pragma once
#ifndef VICON_VECTOR
#define VICON_VECTOR

#include <cmath>
#include <iostream>
#include <vector>
#include <string.h>

struct ViVec {

    double  x;
    double  y;
    double  z;

    //
    //  --- Constructors and Destructors ---
    //

    ViVec( double s = double(0.0) ) :
	x(s), y(s), z(s) {}

    ViVec( double x, double y, double z ) :
	x(x), y(y), z(z) {}

    ViVec( const ViVec& v ) { x = v.x;  y = v.y;  z = v.z; }

    ViVec( const double* array){ x = array[0]; y = array[1]; z = array[2];}

    //
    //  --- Indexing Operator ---
    //

    double& operator [] ( int i ) { return *(&x + i); }
    const double operator [] ( int i ) const { return *(&x + i); }

    //
    //  --- (non-modifying) Arithematic Operators ---
    //

    ViVec operator - () const  // unary minus operator
	{ return ViVec( -x, -y, -z ); }

    ViVec operator + ( const ViVec& v ) const
	{ return ViVec( x + v.x, y + v.y, z + v.z ); }

    ViVec operator - ( const ViVec& v ) const
	{ return ViVec( x - v.x, y - v.y, z - v.z ); }

    ViVec operator * ( const double s ) const
	{ return ViVec( s*x, s*y, s*z ); }

    ViVec operator * ( const ViVec& v ) const
	{ return ViVec( x*v.x, y*v.y, z*v.z ); }

    friend ViVec operator * ( const double s, const ViVec& v )
	{ return v * s; }

    ViVec operator / ( const double s ) const {
#ifdef DEBUG
	if ( std::fabs(s) < DivideByZeroTolerance ) {
	    std::cerr << "[" << __FILE__ << ":" << __LINE__ << "] "
		      << "Division by zero" << std::endl;
	    return ViVec();
	}
#endif // DEBUG
	double r = double(1.0) / s;
	return *this * r;
    }

    ViVec operator / ( const ViVec& v ) const
    { return ViVec( x/v.x, y/v.y, z/v.z ); }

    //
    //  --- (modifying) Arithematic Operators ---
    //

    ViVec& operator += ( const ViVec& v )
	{ x += v.x;  y += v.y;  z += v.z;  return *this; }

    ViVec& operator -= ( const ViVec& v )
	{ x -= v.x;  y -= v.y;  z -= v.z;  return *this; }

    ViVec& operator *= ( const double s )
	{ x *= s;  y *= s;  z *= s;  return *this; }

    ViVec& operator *= ( const ViVec& v )
	{ x *= v.x;  y *= v.y;  z *= v.z;  return *this; }

    ViVec& operator /= ( const double s ){
#ifdef DEBUG
		if ( std::fabs(s) < DivideByZeroTolerance ) {
		    std::cerr << "[" << __FILE__ << ":" << __LINE__ << "] "
			      << "Division by zero" << std::endl;
		    return ViVec();
		}
#endif // DEBUG
		double r = double(1.0) / s;
		x *= r;  y *= r;  z *= r;  return *this;
    }

    ViVec operator /= ( const ViVec& v )
    { x /= v.x;  y /= v.y;  z /= v.z;  return *this; }
	
    //
    //  --- Insertion and Extraction Operators ---
    //

    friend std::ostream& operator << ( std::ostream& os, const ViVec& v ) {
	return os << "( " << v.x << ", " << v.y << ", " << v.z <<  " )";
    }

    friend std::istream& operator >> ( std::istream& is, ViVec& v )
	{ return is >> v.x >> v.y >> v.z ; }

    //
    //  --- Conversion Operators ---
    //

    operator const double* () const
	{ return static_cast<const double*>( &x ); }

    operator double* ()
	{ return static_cast<double*>( &x ); }
};

inline
double dot( const ViVec& u, const ViVec& v ) {
    return u.x*v.x + u.y*v.y + u.z*v.z ;
}

inline
double length( const ViVec& v ) {
    return std::sqrt( dot(v,v) );
}

inline
ViVec normalize( const ViVec& v ) {
    return v / length(v);
}

inline
ViVec cross(const ViVec& a, const ViVec& b )
{
    return ViVec( a.y * b.z - a.z * b.y,
		 a.z * b.x - a.x * b.z,
		 a.x * b.y - a.y * b.x );
}

#endif