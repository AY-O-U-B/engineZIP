#ifndef TRIANGLE_DRAW_H
#define TRIANGLE_DRAW_H

#include "Structuren.h"      // Voor Zbuffer, FloatColor, Light
#include "vector3d.h"        // Voor Vector3D
#include "easy_image.h"      // Voor img::EasyImage
#include <vector>

/**
 * @brief Tekent een Z-buffered driehoek op de afbeelding met één constante kleur.
 */
void draw_zbuf_triag(
    Zbuffer& zbuffer,
    img::EasyImage& image,
    const Vector3D& A,
    const Vector3D& B,
    const Vector3D& C,
    double d,
    double dx,
    double dy,
    const FloatColor& color
);

/**
 * @brief Tekent een Z-buffered driehoek met ambient/diffuse/specular belichting.
 */
void draw_zbuf_triag_lighted(
    Zbuffer& zbuffer,
    img::EasyImage& image,
    const Vector3D& A,
    const Vector3D& B,
    const Vector3D& C,
    double d,
    double dx,
    double dy,
    const FloatColor& ambientReflection,
    const FloatColor& diffuseReflection,
    const FloatColor& specularReflection,
    double reflectionCoefficient,
    const std::vector<Light>& lights
);

#endif // TRIANGLE_DRAW_H
