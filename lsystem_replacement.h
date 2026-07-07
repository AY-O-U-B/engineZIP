#ifndef LSYSTEM_REPLACEMENT_H
#define LSYSTEM_REPLACEMENT_H

#include <string>
#include "l_parser.h"  // Zorg dat hierin de LSystem2D klasse gedefinieerd is

// Functie om de vervangingsregels toe te passen op de initiator-string
std::string applyReplacements(const LParser::LSystem2D &l_system);
#endif //LSYSTEM_REPLACEMENT_H
