#include "lsystem_draw.h"
#include <cmath>
#include <list>
#include "l_parser.h"
#include "pi.h"
#include <stack>
#include <iomanip>

Lines2D drawLSystem(const LParser::LSystem2D &l_system,
                    const std::string &expandedString,
                    const img::Color &lineColor)
{
    Lines2D lines;
    Point2D pos = {0.0, 0.0};
    double angle = l_system.get_starting_angle() * M_PI / 180.0;

    // Vaste stapgrootte. De absolute lengte doet er niet toe: Draw2DLines
    // berekent de bounding box en schaalt alles alsnog naar de afbeelding.
    // De vroegere "dynamische" stap (1 / ruleLength^iterations) werd voor veel
    // iteraties zo klein (of 0/inf) dat afbeeldingen degenereerden of leeg bleven.
    const double step = 1.0;

    std::stack<std::pair<Point2D, double>> stateStack;

    for (char c : expandedString) {
        if (l_system.get_alphabet().count(c)) {
            if (l_system.draw(c)) {
                Point2D newPos = {
                    pos.x + step * std::cos(angle),
                    pos.y + step * std::sin(angle)
                };
                Line2D line;
                line.p1 = pos;
                line.p2 = newPos;

                // Genormaliseerde kleur (0-1) voor FloatColor
                line.color.red   = lineColor.red   / 255.0;
                line.color.green = lineColor.green / 255.0;
                line.color.blue  = lineColor.blue  / 255.0;

                lines.push_back(line);
                pos = newPos;
            } else {
                // Vooruit bewegen zonder te tekenen
                pos.x += step * std::cos(angle);
                pos.y += step * std::sin(angle);
            }
        } else {
            switch (c) {
                case '+':
                    angle += l_system.get_angle() * M_PI / 180.0;
                    break;
                case '-':
                    angle -= l_system.get_angle() * M_PI / 180.0;
                    break;

                case '[':
                case '(':
                    stateStack.push({pos, angle});
                    break;

                case ']':
                case ')':
                    if (!stateStack.empty()) {
                        pos   = stateStack.top().first;
                        angle = stateStack.top().second;
                        stateStack.pop();
                    }
                    break;

                default:
                    // Onbekend symbool: negeren
                    break;
            }
        }
    }
    return lines;
}
