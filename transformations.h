#ifndef TRANSFORMATIONS_H
#define TRANSFORMATIONS_H
#include "vector3d.h"    // voor Matrix type
using Matrix4 = Matrix;   // op voorwaarde dat vector3d.hh een 4×4 Matrix definieert

/// Bouwt een 4×4 homogene scaling-matrix
Matrix4 scaleMatrix(double s);

/// Bouwt een 4×4 rotatie om de X-as (hoek in graden)
Matrix4 rotateXMatrix(double degrees);

/// Bouwt een 4×4 rotatie om de Y-as (hoek in graden)
Matrix4 rotateYMatrix(double degrees);

/// Bouwt een 4×4 rotatie om de Z-as (hoek in graden)
Matrix4 rotateZMatrix(double degrees);

/// Bouwt een 4×4 translatie-matrix voor vector (tx,ty,tz)
Matrix4 translateMatrix(double tx, double ty, double tz);

Vector3D rotate(const Vector3D& v, const Vector3D& axis, double angle_rad);


#endif // TRANSFORMATIONS_H

