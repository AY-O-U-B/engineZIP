#include "LSystem3D.h"
#include <cmath>
#include <stack>
#include <string>
#include <iostream>

namespace LSystem3DGenerator {

namespace {
    constexpr double PI = 3.141592653589793238462643383279502884;

    struct TurtleState {
        Vector3D position = Vector3D::point(0, 0, 0);
        Vector3D H = Vector3D::vector(1, 0, 0); // heading
        Vector3D L = Vector3D::vector(0, 1, 0); // left
        Vector3D U = Vector3D::vector(0, 0, 1); // up
    };
}

Figure3D generate3DLSystem(const LParser::LSystem3D &lsystem) {
    Figure3D figure;
    figure.color.red = 1.0;
    figure.color.green = 1.0;
    figure.color.blue = 1.0;

    // 1. Pas de replacement rules toe. Commando's zoals +, -, ^, &, \, /, |, (, )
    // behoren niet tot het alfabet en blijven dus gewoon staan.
    std::string current = lsystem.get_initiator();
    for (unsigned int i = 0; i < lsystem.get_nr_iterations(); ++i) {
        std::string next;
        for (char c : current) {
            if (lsystem.get_alphabet().count(c) > 0) {
                next += lsystem.get_replacement(c);
            } else {
                next += c;
            }
        }
        current = next;
    }

    // 2. Turtle interpretatie uit de opgave:
    // start in (0,0,0), met H=(1,0,0), L=(0,1,0), U=(0,0,1).
    TurtleState turtle;
    std::stack<TurtleState> stateStack;
    const double delta = lsystem.get_angle() * PI / 180.0;
    const double cosD = std::cos(delta);
    const double sinD = std::sin(delta);

    for (char c : current) {
        switch (c) {
            case '+': { // yaw rond U met +angle
                Vector3D oldH = turtle.H;
                Vector3D oldL = turtle.L;
                turtle.H = oldH * cosD + oldL * sinD;
                turtle.L = oldL * cosD - oldH * sinD;
                break;
            }
            case '-': { // yaw rond U met -angle
                Vector3D oldH = turtle.H;
                Vector3D oldL = turtle.L;
                turtle.H = oldH * cosD - oldL * sinD;
                turtle.L = oldL * cosD + oldH * sinD;
                break;
            }
            case '^': { // pitch rond L met +angle
                Vector3D oldH = turtle.H;
                Vector3D oldU = turtle.U;
                turtle.H = oldH * cosD + oldU * sinD;
                turtle.U = oldU * cosD - oldH * sinD;
                break;
            }
            case '&': { // pitch rond L met -angle
                Vector3D oldH = turtle.H;
                Vector3D oldU = turtle.U;
                turtle.H = oldH * cosD - oldU * sinD;
                turtle.U = oldU * cosD + oldH * sinD;
                break;
            }
            case '\\': { // roll rond H met +angle
                Vector3D oldL = turtle.L;
                Vector3D oldU = turtle.U;
                turtle.L = oldL * cosD - oldU * sinD;
                turtle.U = oldU * cosD + oldL * sinD;
                break;
            }
            case '/': { // roll rond H met -angle
                Vector3D oldL = turtle.L;
                Vector3D oldU = turtle.U;
                turtle.L = oldL * cosD + oldU * sinD;
                turtle.U = oldU * cosD - oldL * sinD;
                break;
            }
            case '|': { // draai 180 graden om
                turtle.H = -turtle.H;
                turtle.L = -turtle.L;
                break;
            }
            case '(':
                stateStack.push(turtle);
                break;
            case ')':
                if (!stateStack.empty()) {
                    turtle = stateStack.top();
                    stateStack.pop();
                }
                break;
            default: {
                if (lsystem.get_alphabet().count(c) == 0) {
                    break; // onbekend symbool: negeren
                }

                Vector3D newPos = turtle.position + turtle.H;
                if (lsystem.draw(c)) {
                    const int startIdx = static_cast<int>(figure.points.size());
                    figure.points.push_back(turtle.position);
                    figure.points.push_back(newPos);

                    Face line;
                    line.point_indexes = {startIdx, startIdx + 1};
                    figure.faces.push_back(line);
                }
                turtle.position = newPos;
                break;
            }
        }
    }

    return figure;
}

} // namespace LSystem3DGenerator
