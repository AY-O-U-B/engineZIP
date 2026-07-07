#include "triangle_draw.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

namespace {

struct ScreenPoint {
    double x;
    double y;
};

bool projectPointToScreen(const Vector3D& pt, double d, double dx, double dy, ScreenPoint& out) {
    if (std::abs(pt.z) < 1e-12) {
        return false;
    }
    out.x = d * pt.x / (-pt.z) + dx;
    out.y = d * pt.y / (-pt.z) + dy;
    return std::isfinite(out.x) && std::isfinite(out.y);
}

bool edgeIntersectionX(const ScreenPoint& P, const ScreenPoint& Q, int y, double& xOut) {
    if (std::abs(P.y - Q.y) < 1e-12) {
        return false;
    }
    const double yD = static_cast<double>(y);
    if ((yD - P.y) * (yD - Q.y) > 0.0) {
        return false;
    }
    xOut = Q.x + (P.x - Q.x) * ((yD - Q.y) / (P.y - Q.y));
    return std::isfinite(xOut);
}

} // namespace

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
    ScreenPoint pA, pB, pC;
    if (!projectPointToScreen(A, d, dx, dy, pA)) return;
    if (!projectPointToScreen(B, d, dx, dy, pB)) return;
    if (!projectPointToScreen(C, d, dx, dy, pC)) return;

    const int width = static_cast<int>(image.get_width());
    const int height = static_cast<int>(image.get_height());
    if (width <= 0 || height <= 0) return;

    const int yMin = std::max(0, static_cast<int>(std::round(std::min({pA.y, pB.y, pC.y}) + 0.5)));
    const int yMax = std::min(height - 1, static_cast<int>(std::round(std::max({pA.y, pB.y, pC.y}) - 0.5)));
    if (yMin > yMax) return;

    const Vector3D AB = B - A;
    const Vector3D AC = C - A;
    const Vector3D w = Vector3D::cross(AB, AC);
    const double k = Vector3D::dot(w, A);
    if (std::abs(k) < 1e-12) {
        return;
    }

    const double dzdx = w.x / (-d * k);
    const double dzdy = w.y / (-d * k);
    const double xG = (pA.x + pB.x + pC.x) / 3.0;
    const double yG = (pA.y + pB.y + pC.y) / 3.0;
    const double invZG = (1.0 / A.z + 1.0 / B.z + 1.0 / C.z) / 3.0;

    const img::Color imgColor(
        static_cast<uint8_t>(std::round(std::clamp(color.red,   0.0, 1.0) * 255.0)),
        static_cast<uint8_t>(std::round(std::clamp(color.green, 0.0, 1.0) * 255.0)),
        static_cast<uint8_t>(std::round(std::clamp(color.blue,  0.0, 1.0) * 255.0))
    );

    for (int y = yMin; y <= yMax; ++y) {
        std::vector<double> xs;
        xs.reserve(3);
        double x;
        if (edgeIntersectionX(pA, pB, y, x)) xs.push_back(x);
        if (edgeIntersectionX(pA, pC, y, x)) xs.push_back(x);
        if (edgeIntersectionX(pB, pC, y, x)) xs.push_back(x);
        if (xs.size() < 2) continue;

        const auto [minIt, maxIt] = std::minmax_element(xs.begin(), xs.end());
        int xMin = static_cast<int>(std::round(*minIt + 0.5));
        int xMax = static_cast<int>(std::round(*maxIt - 0.5));
        xMin = std::max(0, xMin);
        xMax = std::min(width - 1, xMax);
        if (xMin > xMax) continue;

        for (int xPix = xMin; xPix <= xMax; ++xPix) {
            const double invZ = 1.0001 * invZG + (static_cast<double>(xPix) - xG) * dzdx + (static_cast<double>(y) - yG) * dzdy;
            if (invZ < zbuffer.get_value(static_cast<unsigned int>(xPix), static_cast<unsigned int>(y))) {
                zbuffer.set_value(static_cast<unsigned int>(xPix), static_cast<unsigned int>(y), invZ);
                image(static_cast<unsigned int>(xPix), static_cast<unsigned int>(y)) = imgColor;
            }
        }
    }
}
