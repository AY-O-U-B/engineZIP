#include "transformations.h"
#include <cmath> // For std::cos, std::sin

// Helper functie om graden naar radialen om te zetten
// Zorg ervoor dat M_PI beschikbaar is (meestal via <cmath> of definieer het zelf)
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif



static double toRad(double deg) {
    return deg * M_PI / 180.0;
}

Matrix4 scaleMatrix(double s) {
    Matrix4 m;  // Begint als identiteitsmatrix (aanname gebaseerd op eerdere code)
    m(1,1) = s;
    m(2,2) = s;
    m(3,3) = s;
    // m(4,4) is al 1
    return m;
}

Matrix4 rotateXMatrix(double deg) {
    double angleRad = toRad(deg);
    double c = std::cos(angleRad);
    double s = std::sin(angleRad);
    Matrix4 m;  // Identiteitsmatrix
    // Volgens PDF p26 (punttransformatie), voor P' = P * M_x:
    // M_x =
    // | 1  0    0   0 |
    // | 0 cosA sinA 0 |
    // | 0 -sinA cosA 0 |
    // | 0  0    0   1 |
    // Jouw matrix-indexering is (rij, kolom), 1-based.
    m(2,2) =  c;
    m(2,3) =  s; // PDF p26 voor PUNTtransformatie (P' = P * M) heeft sin(alpha) hier
    m(3,2) = -s; // PDF p26 voor PUNTtransformatie (P' = P * M) heeft -sin(alpha) hier
    m(3,3) =  c;
    return m;
}

Matrix4 rotateYMatrix(double deg) {
    double angleRad = toRad(deg);
    double c = std::cos(angleRad);
    double s = std::sin(angleRad);
    Matrix4 m;  // Identiteitsmatrix
    // Volgens PDF p27 (punttransformatie), voor P' = P * M_y:
    // M_y =
    // | cosA  0 -sinA 0 |
    // |  0    1   0   0 |
    // | sinA  0  cosA 0 |
    // |  0    0   0   1 |
    m(1,1) =  c;
    m(1,3) = -s; // PDF p27 voor PUNTtransformatie (P' = P * M) heeft -sin(alpha) hier
    m(3,1) =  s; // PDF p27 voor PUNTtransformatie (P' = P * M) heeft sin(alpha) hier
    m(3,3) =  c;
    return m;
}

Matrix4 rotateZMatrix(double deg) {
    double angleRad = toRad(deg);
    double c = std::cos(angleRad);
    double s = std::sin(angleRad);
    Matrix4 m;  // Identiteitsmatrix
    // Volgens PDF p28 (punttransformatie), voor P' = P * M_z:
    // M_z =
    // | cosA sinA 0 0 |
    // |-sinA cosA 0 0 |
    // |  0    0   1 0 |
    // |  0    0   0 1 |
    m(1,1) =  c;
    m(1,2) =  s; // PDF p28 voor PUNTtransformatie (P' = P * M) heeft sin(alpha) hier
    m(2,1) = -s; // PDF p28 voor PUNTtransformatie (P' = P * M) heeft -sin(alpha) hier
    m(2,2) =  c;
    return m;
}

Matrix4 translateMatrix(double tx, double ty, double tz) {
    Matrix4 m;  // Identiteitsmatrix
    // Voor P' = P * T, staat de translatie in de laatste rij.
    m(4,1) = tx;
    m(4,2) = ty;
    m(4,3) = tz;
    return m;
}
    // Implementatie van Rodrigues' Rotation Formula
    // v_rot = v * cos(theta) + (k x v) * sin(theta) + k * (k . v) * (1 - cos(theta))
    // Waarbij:
    // v = vector die geroteerd moet worden
    // k = genormaliseerde rotatie-as (axis)
    // theta = rotatiehoek in radialen
    Vector3D rotate(const Vector3D& v, const Vector3D& axis, double angle_rad) {
    Vector3D k = Vector3D::normalise(axis);

    double cos_theta = std::cos(angle_rad);
    double sin_theta = std::sin(angle_rad);

    Vector3D term1 = v * cos_theta;
    Vector3D cross_kv = Vector3D::cross(k, v);
    Vector3D term2 = cross_kv * sin_theta;
    double dot_kv = Vector3D::dot(k, v);
    Vector3D term3 = k * dot_kv * (1.0 - cos_theta);

    return term1 + term2 + term3;


}