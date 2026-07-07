#ifndef LIJNTEKENING_H
#define LIJNTEKENING_H

#include "easy_image.h"
#include "ini_configuration.h"
#include "Structuren.h"

// Declaratie van Draw2DLines: geeft een img::EasyImage terug
img::EasyImage Draw2DLines(Lines2D &lines, int size);

// Declaratie van generate_image: geeft een img::EasyImage terug
img::EasyImage generate_image(const ini::Configuration &configuration);

#endif // LIJNTEKENING_H
