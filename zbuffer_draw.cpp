#include "zbuffer_draw.h"
#include <cmath>       // Voor std::round, std::abs
#include <algorithm>   // Voor std::min, std::max, std::clamp
#include <limits>      // Voor std::numeric_limits
#include <iostream>    // Voor std::cerr

namespace zbuffer_draw {

void draw_zbuf_line(
    Zbuffer& zbuffer,
    img::EasyImage& image,
    long x0, long y0, double z0,
    long x1, long y1, double z1,
    const FloatColor& color_float
) {
    img::Color color(
        static_cast<uint8_t>(std::round(std::clamp(color_float.red,   0.0, 1.0) * 255.0)),
        static_cast<uint8_t>(std::round(std::clamp(color_float.green, 0.0, 1.0) * 255.0)),
        static_cast<uint8_t>(std::round(std::clamp(color_float.blue,  0.0, 1.0) * 255.0))
    );

    const long img_width = static_cast<long>(image.get_width());
    const long img_height = static_cast<long>(image.get_height());
    if (img_width <= 0 || img_height <= 0 || std::abs(z0) < 1e-12 || std::abs(z1) < 1e-12) {
        return;
    }

    auto putPixel = [&](long x, long y, double inv_z) {
        if (x < 0 || x >= img_width || y < 0 || y >= img_height) {
            return;
        }
        if (inv_z < zbuffer.get_value(static_cast<unsigned int>(x), static_cast<unsigned int>(y))) {
            zbuffer.set_value(static_cast<unsigned int>(x), static_cast<unsigned int>(y), inv_z);
            image(static_cast<unsigned int>(x), static_cast<unsigned int>(y)) = color;
        }
    };

    auto interpInvZ = [&](double alpha) {
        return (1.0 - alpha) / z0 + alpha / z1;
    };

    if (x0 == x1 && y0 == y1) {
        putPixel(x0, y0, 1.0 / z0);
        return;
    }

    if (x0 == x1) {
        if (y0 > y1) {
            std::swap(y0, y1);
            std::swap(z0, z1);
        }
        const long length = y1 - y0;
        for (long i = 0; i <= length; ++i) {
            const double alpha = (length == 0) ? 0.0 : static_cast<double>(i) / static_cast<double>(length);
            putPixel(x0, y0 + i, interpInvZ(alpha));
        }
        return;
    }

    if (y0 == y1) {
        if (x0 > x1) {
            std::swap(x0, x1);
            std::swap(z0, z1);
        }
        const long length = x1 - x0;
        for (long i = 0; i <= length; ++i) {
            const double alpha = (length == 0) ? 0.0 : static_cast<double>(i) / static_cast<double>(length);
            putPixel(x0 + i, y0, interpInvZ(alpha));
        }
        return;
    }

    // Zelfde pixelkeuze als EasyImage::draw_line, maar met 1/z-interpolatie.
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
        std::swap(z0, z1);
    }

    const double m = static_cast<double>(y1 - y0) / static_cast<double>(x1 - x0);

    if (-1.0 <= m && m <= 1.0) {
        const long length = x1 - x0;
        for (long i = 0; i <= length; ++i) {
            const double alpha = (length == 0) ? 0.0 : static_cast<double>(i) / static_cast<double>(length);
            const long x = x0 + i;
            const long y = static_cast<long>(std::round(y0 + m * i));
            putPixel(x, y, interpInvZ(alpha));
        }
    } else if (m > 1.0) {
        const long length = y1 - y0;
        for (long i = 0; i <= length; ++i) {
            const double alpha = (length == 0) ? 0.0 : static_cast<double>(i) / static_cast<double>(length);
            const long x = static_cast<long>(std::round(x0 + i / m));
            const long y = y0 + i;
            putPixel(x, y, interpInvZ(alpha));
        }
    } else { // m < -1.0
        const long length = y0 - y1;
        for (long i = 0; i <= length; ++i) {
            const double alpha = (length == 0) ? 0.0 : static_cast<double>(i) / static_cast<double>(length);
            const long x = static_cast<long>(std::round(x0 - i / m));
            const long y = y0 - i;
            putPixel(x, y, interpInvZ(alpha));
        }
    }
}

img::EasyImage DrawZBuffered(
    Zbuffer& zbuffer,
    Lines2D& lines,
    int width, int height,
    img::Color background
) {
    img::EasyImage image(width, height, background);
    zbuffer.clear(); // Zorg ervoor dat de Z-buffer leeg is voor elke nieuwe afbeelding

    // Bepaal de min/max voor x en y van de genormaliseerde lijnen
    double xmin = std::numeric_limits<double>::max();
    double xmax = std::numeric_limits<double>::lowest();
    double ymin = std::numeric_limits<double>::max();
    double ymax = std::numeric_limits<double>::lowest();

    for (auto &line : lines) {
        xmin = std::min({xmin, line.p1.x, line.p2.x});
        xmax = std::max({xmax, line.p1.x, line.p2.x});
        ymin = std::min({ymin, line.p1.y, line.p2.y});
        ymax = std::max({ymax, line.p1.y, line.p2.y});
    }

    double xrange = xmax - xmin;
    double yrange = ymax - ymin;

    // Voorkom delen door nul voor lege of degeneratieve bereiken
    if (xrange <= 0 || yrange <= 0) {
        std::cerr << "Warning: Degenerate of lege lijnenlijst voor DrawZBuffered. Geen afbeelding gegenereerd." << std::endl;
        return img::EasyImage(width, height, background); // Retourneer een lege afbeelding
    }

    // Gebruik exact dezelfde 2D normalisatie als wireframe_draw::Draw.
    // De referentie centreert op de onafgeronde beeldafmetingen imagex/imagey;
    // centreren op width/height verschuift dichte kegel/torus-lijnen net genoeg
    // om veel pixels 1 positie fout te zetten.
    const double maxRange = std::max(xrange, yrange);
    const double d = 0.95 * std::max(width, height) / maxRange;
    const double xmid = (xmin + xmax) / 2.0;
    const double ymid = (ymin + ymax) / 2.0;
    const double imagex = std::max(width, height) * (xrange / maxRange);
    const double imagey = std::max(width, height) * (yrange / maxRange);

    auto tx = [&](double x) {
        return std::clamp((x - xmid) * d + width / 2.0, 0.0, double(width - 1));
    };
    auto ty = [&](double y) {
        return std::clamp((y - ymid) * d + height / 2.0, 0.0, double(height - 1));
    };

    // Pas de transformatie toe en teken de lijnen
    for (auto &line : lines) {
        long X1 = static_cast<long>(std::round(tx(line.p1.x)));
        long Y1 = static_cast<long>(std::round(ty(line.p1.y)));
        long X2 = static_cast<long>(std::round(tx(line.p2.x)));
        long Y2 = static_cast<long>(std::round(ty(line.p2.y)));

        // Roep de Z-buffered lijntekenfunctie aan
        draw_zbuf_line(zbuffer, image, X1, Y1, line.z1, X2, Y2, line.z2, line.color);
    }

    return image;
}

} // namespace zbuffer_draw