//
// Created by legen on 19/02/2025.
//

#ifndef STRUCTUREN_H
#define STRUCTUREN_H

#include <list>
#include <vector>
#include <limits> // Voor std::numeric_limits
#include "vector3d.h"
#include "easy_image.h" // Voor img::Color

class FloatColor {
public:
    double red, green, blue;
};

class Point2D {
public:
    double x, y;
};

class Line2D {
public:
    Point2D p1, p2;
    FloatColor color;
    double z1; // NIEUW: Z-waarde van het eerste punt
    double z2; // NIEUW: Z-waarde van het tweede punt
};

using Lines2D = std::list<Line2D>;


/// Een “vlak” in deze opdracht is altijd een lijn (2 punten). Later wordt dit for 3+ gebruikt.
class Face {
public:
    /// Indexen in de Figure3D::points‐vector
    std::vector<int> point_indexes;
};

/// Een 3D‐figure met punten in homogene coördinaten en vlakken (hier: lijnen)
class Figure3D {
public:
    /// Punten in 3D (Vector3D::point(x,y,z) geeft homo‐coördinaat w=1)
    std::vector<Vector3D> points;
    /// Vlakken (voor nu altijd 2 indices)
    std::vector<Face>     faces;
    /// Kleur van de lijnen
    FloatColor            color;
};

/// Al je 3D‐figuren in één scène
using Figures3D = std::list<Figure3D>;


// NIEUW: Zbuffer klasse voor Z-buffering
class Zbuffer {
private:
    std::vector<std::vector<double>> buffer;
    unsigned int width;
    unsigned int height;

public:
    // Constructor: initialiseert de Z-buffer met oneindige waarden
    Zbuffer(unsigned int w, unsigned int h) : width(w), height(h) {
        buffer.resize(height);
        for (unsigned int i = 0; i < height; ++i) {
            buffer[i].resize(width, std::numeric_limits<double>::infinity());
        }
    }

    // Retourneert de Z-waarde op een specifieke coördinaat
    double get_value(unsigned int x, unsigned int y) const {
        if (x < width && y < height) {
            return buffer[y][x];
        }
        // Retourneer oneindig als buiten de grenzen
        return std::numeric_limits<double>::infinity();
    }

    // Stelt de Z-waarde in op een specifieke coördinaat
    void set_value(unsigned int x, unsigned int y, double z) {
        if (x < width && y < height) {
            buffer[y][x] = z;
        }
    }

    // Reset de Z-buffer naar oneindige waarden
    void clear() {
        for (unsigned int i = 0; i < height; ++i) {
            for (unsigned int j = 0; j < width; ++j) {
                buffer[i][j] = std::numeric_limits<double>::infinity();
            }
        }
    }
};


#endif //STRUCTUREN_H
