#ifndef ZBUFFER_DRAW_H
#define ZBUFFER_DRAW_H

#include "easy_image.h"   // Voor img::EasyImage en img::Color
#include "Structuren.h"   // Voor Line2D, Zbuffer, FloatColor, Point2D

namespace zbuffer_draw {


void draw_zbuf_line(
    Zbuffer& zbuffer,
    img::EasyImage& image,
    long x0, long y0, double z0,
    long x1, long y1, double z1,
    const FloatColor& color
);


img::EasyImage DrawZBuffered(
    Zbuffer& zbuffer,
    Lines2D& lines,
    int width, int height,
    img::Color background
);

} // namespace zbuffer_draw

#endif // ZBUFFER_DRAW_H
