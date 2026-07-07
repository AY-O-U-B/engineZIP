#include "triangle_draw.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <vector>

namespace {

constexpr double EPS = 1e-12;

struct ScreenPoint {
    double x;
    double y;
};

bool projectPointToScreen(const Vector3D& pt, double d, double dx, double dy, ScreenPoint& out) {
    if (std::abs(pt.z) < EPS) {
        return false;
    }
    out.x = d * pt.x / (-pt.z) + dx;
    out.y = d * pt.y / (-pt.z) + dy;
    return std::isfinite(out.x) && std::isfinite(out.y);
}

bool edgeIntersectionX(const ScreenPoint& P, const ScreenPoint& Q, int y, double& xOut) {
    if (std::abs(P.y - Q.y) < EPS) {
        return false;
    }
    const double yD = static_cast<double>(y);
    if ((yD - P.y) * (yD - Q.y) > 0.0) {
        return false;
    }
    xOut = Q.x + (P.x - Q.x) * ((yD - Q.y) / (P.y - Q.y));
    return std::isfinite(xOut);
}

img::Color toImageColorFloor(const FloatColor& color) {
    return img::Color(
        static_cast<uint8_t>(std::clamp(color.red,   0.0, 1.0) * 255.0),
        static_cast<uint8_t>(std::clamp(color.green, 0.0, 1.0) * 255.0),
        static_cast<uint8_t>(std::clamp(color.blue,  0.0, 1.0) * 255.0)
    );
}

img::Color toImageColorRound(const FloatColor& color) {
    return img::Color(
        static_cast<uint8_t>(std::round(std::clamp(color.red,   0.0, 1.0) * 255.0)),
        static_cast<uint8_t>(std::round(std::clamp(color.green, 0.0, 1.0) * 255.0)),
        static_cast<uint8_t>(std::round(std::clamp(color.blue,  0.0, 1.0) * 255.0))
    );
}

bool isBlack(const FloatColor& c) {
    return std::abs(c.red) < EPS && std::abs(c.green) < EPS && std::abs(c.blue) < EPS;
}

void addScaled(FloatColor& acc, const FloatColor& material, const FloatColor& light, double factor) {
    if (factor <= 0.0) {
        return;
    }
    acc.red   += material.red   * light.red   * factor;
    acc.green += material.green * light.green * factor;
    acc.blue  += material.blue  * light.blue  * factor;
}

void addAmbient(FloatColor& acc, const FloatColor& material, const FloatColor& light) {
    acc.red   += material.red   * light.red;
    acc.green += material.green * light.green;
    acc.blue  += material.blue  * light.blue;
}

Vector3D safeNormalise(const Vector3D& v) {
    const double len = v.length();
    if (len < EPS) {
        return Vector3D::vector(0.0, 0.0, 0.0);
    }
    return v / len;
}

FloatColor shadePoint(
    const Vector3D& point,
    const Vector3D& normal,
    const FloatColor& ambientReflection,
    const FloatColor& diffuseReflection,
    const FloatColor& specularReflection,
    double reflectionCoefficient,
    const std::vector<Light>& lights
) {
    FloatColor result;
    const Vector3D view = safeNormalise(-Vector3D::vector(point.x, point.y, point.z));

    for (const Light& light : lights) {
        addAmbient(result, ambientReflection, light.ambientLight);

        const bool hasDiffuse = !isBlack(diffuseReflection) && !isBlack(light.diffuseLight);
        const bool hasSpecular = !isBlack(specularReflection) && !isBlack(light.specularLight) && reflectionCoefficient > 0.0;
        if (!hasDiffuse && !hasSpecular) {
            continue;
        }

        Vector3D l;
        if (light.infinity) {
            l = safeNormalise(light.direction);
        } else {
            l = safeNormalise(light.location - point);
        }

        double cosAlpha = Vector3D::dot(normal, l);
        if (cosAlpha <= 0.0) {
            continue;
        }

        double diffuseFactor = cosAlpha;
        if (!light.infinity && light.hasSpotAngle) {
            if (cosAlpha <= light.spotCos) {
                continue;
            }
            const double denom = 1.0 - light.spotCos;
            diffuseFactor = (denom > EPS) ? (1.0 - (1.0 - cosAlpha) / denom) : 1.0;
            diffuseFactor = std::clamp(diffuseFactor, 0.0, 1.0);
        }

        if (hasDiffuse) {
            addScaled(result, diffuseReflection, light.diffuseLight, diffuseFactor);
        }

        if (hasSpecular) {
            Vector3D reflected = safeNormalise((2.0 * cosAlpha) * normal - l);
            const double cosBeta = Vector3D::dot(reflected, view);
            if (cosBeta > 0.0) {
                addScaled(result, specularReflection, light.specularLight, std::pow(cosBeta, reflectionCoefficient));
            }
        }
    }

    result.red = std::clamp(result.red, 0.0, 1.0);
    result.green = std::clamp(result.green, 0.0, 1.0);
    result.blue = std::clamp(result.blue, 0.0, 1.0);
    return result;
}

template <typename PixelShader>
void rasterizeTriangle(
    Zbuffer& zbuffer,
    img::EasyImage& image,
    const Vector3D& A,
    const Vector3D& B,
    const Vector3D& C,
    double d,
    double dx,
    double dy,
    PixelShader&& shader
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
    if (std::abs(k) < EPS) {
        return;
    }

    const double dzdx = w.x / (-d * k);
    const double dzdy = w.y / (-d * k);
    const double xG = (pA.x + pB.x + pC.x) / 3.0;
    const double yG = (pA.y + pB.y + pC.y) / 3.0;
    const double invZG = (1.0 / A.z + 1.0 / B.z + 1.0 / C.z) / 3.0;

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
            const double invZNoBias = invZG + (static_cast<double>(xPix) - xG) * dzdx + (static_cast<double>(y) - yG) * dzdy;
            const double invZ = 1.0001 * invZG + (static_cast<double>(xPix) - xG) * dzdx + (static_cast<double>(y) - yG) * dzdy;
            if (!std::isfinite(invZNoBias) || std::abs(invZNoBias) < EPS) {
                continue;
            }
            if (invZ < zbuffer.get_value(static_cast<unsigned int>(xPix), static_cast<unsigned int>(y))) {
                zbuffer.set_value(static_cast<unsigned int>(xPix), static_cast<unsigned int>(y), invZ);
                image(static_cast<unsigned int>(xPix), static_cast<unsigned int>(y)) = shader(xPix, y, invZNoBias);
            }
        }
    }
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
    const img::Color imgColor = toImageColorRound(color);
    rasterizeTriangle(zbuffer, image, A, B, C, d, dx, dy,
        [&](int, int, double) {
            return imgColor;
        }
    );
}

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
) {
    Vector3D normal = safeNormalise(Vector3D::cross(B - A, C - A));
    // De zichtbare kant moet met een normaal naar de camera toe belicht worden.
    // Dit maakt de rasterizer robuust voor enkele inconsistent georiënteerde vlakken.
    const Vector3D viewFromA = safeNormalise(-Vector3D::vector(A.x, A.y, A.z));
    if (Vector3D::dot(normal, viewFromA) < 0.0) {
        normal = -normal;
    }

    rasterizeTriangle(zbuffer, image, A, B, C, d, dx, dy,
        [&](int xPix, int yPix, double invZNoBias) {
            const double z = 1.0 / invZNoBias;
            const double x = -z * (static_cast<double>(xPix) - dx) / d;
            const double y = -z * (static_cast<double>(yPix) - dy) / d;
            const Vector3D point = Vector3D::point(x, y, z);
            return toImageColorFloor(shadePoint(point, normal,
                                           ambientReflection, diffuseReflection,
                                           specularReflection, reflectionCoefficient,
                                           lights));
        }
    );
}
