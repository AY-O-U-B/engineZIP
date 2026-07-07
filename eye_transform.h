#ifndef EYE_TRANSFORM_H
#define EYE_TRANSFORM_H

#include "vector3d.h"
#include "Structuren.h"
#include <cmath>

Matrix eyePointTrans(const Vector3D &eyepoint);

void applyTransformation(Figure3D &fig, const Matrix &matrix);
void applyTransformation(std::vector<Figure3D> &figs, const Matrix &matrix);

#endif // EYE_TRANSFORM_H
