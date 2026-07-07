#include "wireframe_draw.h"
#include <limits>
#include <algorithm>
#include <cmath>
#include <iostream>

namespace wireframe_draw {

img::EasyImage Draw(const Lines2D &lines, int size, img::Color bg, bool truncateImageDimensions) {
    // 1) compute bounding box
    double xmin =  std::numeric_limits<double>::infinity();
    double xmax = -std::numeric_limits<double>::infinity();
    double ymin =  std::numeric_limits<double>::infinity();
    double ymax = -std::numeric_limits<double>::infinity();

    for (auto &L : lines) {
        xmin = std::min({xmin, L.p1.x, L.p2.x});
        xmax = std::max({xmax, L.p1.x, L.p2.x});
        ymin = std::min({ymin, L.p1.y, L.p2.y});
        ymax = std::max({ymax, L.p1.y, L.p2.y});
    }

    double xrange = xmax - xmin;
    double yrange = ymax - ymin;



    if (xrange <= 0 || yrange <= 0) {
        return img::EasyImage();  // niets te tekenen
    }

    // 2) grootste zijde op 'size'
    int width, height;
    if (xrange >= yrange) {
        width  = size;
        const double rawHeight = size * (yrange / xrange);
        height = truncateImageDimensions ? static_cast<int>(rawHeight) : int(std::round(rawHeight));
    } else {
        height = size;
        const double rawWidth = size * (xrange / yrange);
        width  = truncateImageDimensions ? static_cast<int>(rawWidth) : int(std::round(rawWidth));
    }


    if (width <= 0 || height <= 0) {
        return img::EasyImage();
    }

    // 3) afbeelding maken
    img::EasyImage image(width, height, bg);

    // 4) schaal en center
    double d    = 0.95 * size / std::max(xrange, yrange);
    double xmid = (xmin + xmax) / 2.0;
    double ymid = (ymin + ymax) / 2.0;

    // Centreer op de ONAFGERONDE beeldafmetingen (imagex/imagey), niet op de
    // afgeronde width/height. De afronding van width/height verschoof alle
    // punten een fractie van een pixel; bij dichte, bijna-parallelle lijnen
    // (kegel, torus) liet dat de SSIM t.o.v. de referentie kelderen.
    const double imagex = size * (xrange / std::max(xrange, yrange));
    const double imagey = size * (yrange / std::max(xrange, yrange));

    // 5) tekenen
    for (auto &L : lines) {
        auto tx = [&](double x){ return (x - xmid)*d + width / 2.0; };
        auto ty = [&](double y){ return (y - ymid)*d + height / 2.0; };

        int x1 = int(std::round(std::clamp(tx(L.p1.x), 0.0, double(width-1))));
        int y1 = int(std::round(std::clamp(ty(L.p1.y), 0.0, double(height-1))));
        int x2 = int(std::round(std::clamp(tx(L.p2.x), 0.0, double(width-1))));
        int y2 = int(std::round(std::clamp(ty(L.p2.y), 0.0, double(height-1))));

        img::Color c(
            uint8_t(std::clamp(L.color.red, 0.0, 1.0)   * 255),
            uint8_t(std::clamp(L.color.green, 0.0, 1.0) * 255),
            uint8_t(std::clamp(L.color.blue, 0.0, 1.0)  * 255)
        );
        image.draw_line(x1, y1, x2, y2, c);
    }

    return image;
}

} // namespace wireframe_draw
