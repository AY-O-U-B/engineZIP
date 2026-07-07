//
// Created by legen on 19/05/2025.
//

#ifndef FIGURES3D_H
#define FIGURES3D_H
// figures3d.h
#pragma once

#include "Structuren.h"
#include "vector3d.h"

/// @file figures3d.h
/// @brief Declaratie van functies om standaard 3D-lichamen te genereren als Figure3D.

/// Genereert een cubus rond (0,0,0) met zijde lengte 2
Figure3D createCube();

/// Genereert een tetrahedron rond (0,0,0) met zijde lengte 2
Figure3D createTetrahedron();

/// Genereert een octahedron rond (0,0,0) met randafstand 1
Figure3D createOctahedron();

/// Genereert een icosahedron rond (0,0,0) met randafstand 1
Figure3D createIcosahedron();

/// Genereert een dodecahedron rond (0,0,0) met randafstand 1
Figure3D createDodecahedron();

/// Genereert een cilinder rond Z-as, grondvlak op z=0, hoogte h, n segmenten
Figure3D createCylinder(int n, double h);

/// Genereert een kegel rond Z-as, grondvlak op z=0, hoogte h, n segmenten
Figure3D createCone(int n, double h);

/// Genereert een bol via subdivided icosahedron, n iteraties
Figure3D createSphere(int n);

/// Genereert een torus rond Z-as, afstand R, buisstraal r, n radial, m tubal punten
Figure3D createTorus(double R, double r, int n, int m);

#endif //FIGURES3D_H
