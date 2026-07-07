#include "lijntekening.h"
#include <iostream>
#include <limits>
#include <algorithm>
#include <cmath>

img::EasyImage Draw2DLines(Lines2D &lines, int size) {
    if (lines.empty()) {
        std::cerr << "Fout: De lijst met lijnen is leeg!" << std::endl;
        return img::EasyImage();
    }
    // Initialiseert xmin, xmax, ymin, ymax
    double xmin = std::numeric_limits<double>::max();
    double xmax = std::numeric_limits<double>::lowest();
    double ymin = std::numeric_limits<double>::max();
    double ymax = std::numeric_limits<double>::lowest();

    // Doorloop alle lijnen en bepaal de grenzen
    for (auto &line : lines) {
        // Punt 1
        xmin = std::min(xmin, line.p1.x);
        xmax = std::max(xmax, line.p1.x);
        ymin = std::min(ymin, line.p1.y);
        ymax = std::max(ymax, line.p1.y);
        // Punt 2
        xmin = std::min(xmin, line.p2.x);
        xmax = std::max(xmax, line.p2.x);
        ymin = std::min(ymin, line.p2.y);
        ymax = std::max(ymax, line.p2.y);
    }

    double xrange = xmax - xmin;
    double yrange = ymax - ymin;
    double maxrange = std::max(xrange, yrange);

    double Imagex = size * (xrange / maxrange);
    double Imagey = size * (yrange / maxrange);

    // Schaalfactor
    double d = 0.95 * (Imagex / xrange);

    // Schaal de coördinaten
    for (auto &line : lines) {
        line.p1.x *= d;
        line.p1.y *= d;
        line.p2.x *= d;
        line.p2.y *= d;
    }

    // Bepaal het midden en bereken de verschuiving
    double DCx = d * (xmin + xmax) / 2.0;
    double DCy = d * (ymin + ymax) / 2.0;
    double dx = (Imagex / 2.0) - DCx;
    double dy = (Imagey / 2.0) - DCy;

    // Pas de verschuiving toe
    for (auto &line : lines) {
        line.p1.x += dx;
        line.p1.y += dy;
        line.p2.x += dx;
        line.p2.y += dy;
    }

    // Afronden van coördinaten
    for (auto &line : lines) {
        line.p1.x = std::round(line.p1.x);
        line.p1.y = std::round(line.p1.y);
        line.p2.x = std::round(line.p2.x);
        line.p2.y = std::round(line.p2.y);
    }

    // Hier maak je een EasyImage aan (afhankelijk van je implementatie)
    // Voorbeeld: maak een afbeelding met de berekende breedte en hoogte
    int width = static_cast<int>(Imagex);
    int height = static_cast<int>(Imagey);
    img::EasyImage image(width, height);
    // (Optioneel: teken de lijnen op 'image')
    return image;
}

