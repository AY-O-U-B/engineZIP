#ifndef WIREFRAME_DRAW_H
#define WIREFRAME_DRAW_H

#include "easy_image.h"
#include "Structuren.h"

namespace wireframe_draw {

    // Tekent je 2D-lijnen met behoud van het originele aspect-ratio.
    // - lines: lijst van Line2D (p1,p2,color)
    // - size: de maximumgrootte van de lange zijde
    // - bg: achtergrondkleur
    img::EasyImage Draw(const Lines2D &lines, int size, img::Color bg);

} // namespace wireframe_draw

#endif // WIREFRAME_DRAW_H
