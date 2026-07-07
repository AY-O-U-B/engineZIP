#include "projection.h"

Point2D projectPoint(const Vector3D &pt, double d) {
    Point2D p;
    p.x = d * pt.x / (-pt.z);
    p.y = d * pt.y / (-pt.z);
    return p;
}
