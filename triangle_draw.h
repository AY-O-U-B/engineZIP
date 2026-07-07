#ifndef TRIANGLE_DRAW_H
#define TRIANGLE_DRAW_H

#include "Structuren.h"      // Voor Zbuffer, FloatColor
#include "vector3d.h"        // Voor Vector3D
#include "easy_image.h"      // Voor img::EasyImage

/**
 * @brief Tekent een Z-buffered driehoek op de afbeelding.
 * 
 * Deze functie gebruikt de Z-buffer om depth testing uit te voeren.
 * Alleen de dichtstbijzijnde pixels worden getekend.
 * 
 * @param zbuffer    De Z-buffer voor depth testing
 * @param image      De afbeelding waarop getekend wordt
 * @param A          Eerste hoekpunt in eye-coördinaten
 * @param B          Tweede hoekpunt in eye-coördinaten
 * @param C          Derde hoekpunt in eye-coördinaten
 * @param d          Schaalfactor voor projectie
 * @param dx         X-verschuiving voor centering
 * @param dy         Y-verschuiving voor centering
 * @param color      Kleur van de driehoek (FloatColor met waardes 0-1)
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

#endif // TRIANGLE_DRAW_H