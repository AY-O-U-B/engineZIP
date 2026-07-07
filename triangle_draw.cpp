#include "triangle_draw.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <iostream>

/**
 * @brief Projecteert een 3D punt naar 2D schermcoördinaten met Z-waarde.
 * 
 * @param pt  Punt in eye-coördinaten (Vector3D)
 * @param d   Schaalfactor
 * @param dx  X-verschuiving
 * @param dy  Y-verschuiving
 * @param x_out  Output: X-schermcoördinaat
 * @param y_out  Output: Y-schermcoördinaat
 * @param z_out  Output: Z-waarde (voor Z-buffer, 1/z)
 * @return true als projectie geldig is, false bij delen door nul
 */
static bool projectPointToScreen(
    const Vector3D& pt,
    double d, double dx, double dy,
    double* x_out, double* y_out, double* z_out
) {
    // Check voor delen door nul
    if (std::abs(pt.z) < 1e-8) {
        return false;
    }
    
    // Projectie: x' = d * x / -z
    double x_proj = d * pt.x / (-pt.z);
    double y_proj = d * pt.y / (-pt.z);
    
    // Transformeer naar schermcoördinaten
    *x_out = x_proj + dx;
    *y_out = y_proj + dy;
    
    // Z-buffer waarde (1/z voor correcte interpolatie)
    *z_out = 1.0 / pt.z;
    
    return true;
}

/**
 * @brief Hulpstructuur voor een geprojecteerd punt.
 */
struct ScreenPoint {
    double x, y;      // Schermcoördinaten
    double inv_z;     // 1/z voor Z-buffer interpolatie
};

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
) {
    // 1. Projecteer de drie hoekpunten
    ScreenPoint pA, pB, pC;
    
    if (!projectPointToScreen(A, d, dx, dy, &pA.x, &pA.y, &pA.inv_z)) return;
    if (!projectPointToScreen(B, d, dx, dy, &pB.x, &pB.y, &pB.inv_z)) return;
    if (!projectPointToScreen(C, d, dx, dy, &pC.x, &pC.y, &pC.inv_z)) return;
    
    // 2. Converteer naar img::Color
    img::Color img_color(
        static_cast<uint8_t>(std::round(std::clamp(color.red, 0.0, 1.0) * 255)),
        static_cast<uint8_t>(std::round(std::clamp(color.green, 0.0, 1.0) * 255)),
        static_cast<uint8_t>(std::round(std::clamp(color.blue, 0.0, 1.0) * 255))
    );
    
    // 3. Bepaal de bounding box (in integers)
    int ymin = static_cast<int>(std::floor(std::min({pA.y, pB.y, pC.y})));
    int ymax = static_cast<int>(std::ceil(std::max({pA.y, pB.y, pC.y})));
    
    // Clamp aan afbeeldingsgrenzen
    int img_height = static_cast<int>(image.get_height());
    int img_width = static_cast<int>(image.get_width());
    
    ymin = std::max(0, ymin);
    ymax = std::min(img_height - 1, ymax);
    
    // 4. Bereken de oppervlakte van de driehoek (voor barycentrische coördinaten)
    // Area = 0.5 * |det| waar det = (B-A) x (C-A) in 2D
    double denom = (pB.y - pC.y) * (pA.x - pC.x) + (pC.x - pB.x) * (pA.y - pC.y);
    
    if (std::abs(denom) < 1e-8) {
        // Driehoek is gedegenereerd (alle punten op één lijn)
        return;
    }
    
    // 5. Scanline algorithm: voor elke y-rij
    for (int yi = ymin; yi <= ymax; ++yi) {
        double y = yi + 0.5;  // Pixel center
        
        // Bepaal xmin en xmax voor deze scanline
        double xmin_scan = std::numeric_limits<double>::infinity();
        double xmax_scan = -std::numeric_limits<double>::infinity();
        
        // Check intersecties met de drie edges: AB, BC, CA
        auto checkEdge = [&](const ScreenPoint& p1, const ScreenPoint& p2) {
            if ((p1.y <= y && y < p2.y) || (p2.y <= y && y < p1.y)) {
                // Edge kruist deze scanline
                double t = (y - p1.y) / (p2.y - p1.y);
                double x_intersect = p1.x + t * (p2.x - p1.x);
                xmin_scan = std::min(xmin_scan, x_intersect);
                xmax_scan = std::max(xmax_scan, x_intersect);
            }
        };
        
        checkEdge(pA, pB);
        checkEdge(pB, pC);
        checkEdge(pC, pA);
        
        if (xmin_scan > xmax_scan) continue;  // Geen intersectie
        
        // Clamp x-grenzen
        int xi_min = std::max(0, static_cast<int>(std::floor(xmin_scan)));
        int xi_max = std::min(img_width - 1, static_cast<int>(std::ceil(xmax_scan)));
        
        // 6. Teken alle pixels op deze scanline
        for (int xi = xi_min; xi <= xi_max; ++xi) {
            double x = xi + 0.5;  // Pixel center
            
            // Bereken barycentrische coördinaten (λA, λB, λC)
            double lambda_A = ((pB.y - pC.y) * (x - pC.x) + (pC.x - pB.x) * (y - pC.y)) / denom;
            double lambda_B = ((pC.y - pA.y) * (x - pC.x) + (pA.x - pC.x) * (y - pC.y)) / denom;
            double lambda_C = 1.0 - lambda_A - lambda_B;
            
            // Check of punt binnen driehoek ligt
            if (lambda_A < 0 || lambda_B < 0 || lambda_C < 0) continue;
            
            // Interpoleer 1/z waarde
            double inv_z = lambda_A * pA.inv_z + lambda_B * pB.inv_z + lambda_C * pC.inv_z;
            
            // Z-buffer test
            if (inv_z < zbuffer.get_value(xi, yi)) {
                zbuffer.set_value(xi, yi, inv_z);
                image(xi, yi) = img_color;
            }
        }
    }
}