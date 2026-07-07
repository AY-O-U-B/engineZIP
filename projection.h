// src/3D/projection.h
#ifndef PROJECTION_H
#define PROJECTION_H

#include "Structuren.h"  // Point2D
#include "vector3d.h"                        // Vector3D

/**
 * Perspective projection of a 3D point in eye-coordinates onto z = -d plane.
 * @param pt  Eye-coordinate 3D point (Vector3D::point)
 * @param d   Distance to projection plane (default 20.0)
 * @return    2D point in projection plane
 */
// WIJZIGING: Standaardwaarde van d aangepast van 5.0 naar 20.0
Point2D projectPoint(const Vector3D &pt, double d = 20.0);

#endif // PROJECTION_H