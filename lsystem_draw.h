#ifndef LSYSTEM_DRAW_H
#define LSYSTEM_DRAW_H

#include "Structuren.h"
#include <string>
#include "l_parser.h"
#include "easy_image.h"


Lines2D drawLSystem(const LParser::LSystem2D &l_system,
                    const std::string &expandedString,
                    const img::Color &lineColor);;


#endif




