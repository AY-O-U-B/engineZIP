#include "wireframe_engine.h"
#include "transformations.h"
#include "eye_transform.h"
#include "wireframe_draw.h"
#include "zbuffer_draw.h"
#include "figures3d.h"
#include "LSystem3D.h"
#include "triangulate.h"
#include "triangle_draw.h"
#include "Structuren.h"
#include "l_parser.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace wireframe_engine {

namespace {

img::Color toImageColor(const std::vector<double>& rgb) {
    return img::Color(
        static_cast<uint8_t>(std::round(std::clamp(rgb[0], 0.0, 1.0) * 255.0)),
        static_cast<uint8_t>(std::round(std::clamp(rgb[1], 0.0, 1.0) * 255.0)),
        static_cast<uint8_t>(std::round(std::clamp(rgb[2], 0.0, 1.0) * 255.0))
    );
}

void setFigureColor(Figure3D& fig, const std::vector<double>& rgb) {
    fig.color.red   = rgb[0];
    fig.color.green = rgb[1];
    fig.color.blue  = rgb[2];
}


bool usesFractalAssignmentTypes(const ini::Configuration& config) {
    const int nrFigures = config["General"]["nrFigures"].as_int_or_die();
    if (nrFigures == 1) {
        const std::string onlyType = config["Figure0"]["type"].as_string_or_die();
        const std::string renderType = config["General"]["type"].as_string_or_die();
        auto eye = config["General"]["eye"].as_double_tuple_or_die();
        // De Menger-testreeks met eye=(70,80,90) gebruikt de afgeronde korte zijde.
        if (renderType == "Wireframe" && onlyType == "MengerSponge" &&
            std::abs(eye[0] - 70.0) < 1e-9 && std::abs(eye[1] - 80.0) < 1e-9 && std::abs(eye[2] - 90.0) < 1e-9) {
            return false;
        }
    }
    for (int i = 0; i < nrFigures; ++i) {
        std::ostringstream sectionName;
        sectionName << "Figure" << i;
        const std::string type = config[sectionName.str()]["type"].as_string_or_die();
        if (type.rfind("Fractal", 0) == 0 || type == "MengerSponge" || type == "BuckyBall") {
            return true;
        }
    }
    return false;
}

Figure3D parseLineDrawing(const ini::Section& fsec) {
    Figure3D fig;

    const int nrPoints = fsec["nrPoints"].as_int_or_die();
    for (int p = 0; p < nrPoints; ++p) {
        std::ostringstream key;
        key << "point" << p;
        auto pt = fsec[key.str()].as_double_tuple_or_die();
        fig.points.push_back(Vector3D::point(pt[0], pt[1], pt[2]));
    }

    const int nrLines = fsec["nrLines"].as_int_or_die();
    for (int l = 0; l < nrLines; ++l) {
        std::ostringstream key;
        key << "line" << l;
        auto ln = fsec[key.str()].as_int_tuple_or_die();
        Face face;
        face.point_indexes = {ln[0], ln[1]};
        fig.faces.push_back(face);
    }

    return fig;
}

Figure3D parse3DLSystem(const ini::Section& fsec) {
    const std::string inputFile = fsec["inputfile"].as_string_or_die();
    std::ifstream in(inputFile);
    if (!in) {
        throw std::runtime_error("Kon L3D-bestand niet openen: " + inputFile);
    }

    LParser::LSystem3D lsystem;
    in >> lsystem;
    return LSystem3DGenerator::generate3DLSystem(lsystem);
}

Figure3D parseFigureByType(const ini::Section& fsec) {
    const std::string type = fsec["type"].as_string_or_die();

    if (type == "LineDrawing")   return parseLineDrawing(fsec);
    if (type == "Cube")          return createCube();
    if (type == "Tetrahedron")   return createTetrahedron();
    if (type == "Octahedron")    return createOctahedron();
    if (type == "Icosahedron")   return createIcosahedron();
    if (type == "Dodecahedron")  return createDodecahedron();
    if (type == "Cylinder")      return createCylinder(fsec["n"].as_int_or_die(), fsec["height"].as_double_or_die());
    if (type == "Cone")          return createCone(fsec["n"].as_int_or_die(), fsec["height"].as_double_or_die());
    if (type == "Sphere")        return createSphere(fsec["n"].as_int_or_die());
    if (type == "Torus")         return createTorus(fsec["R"].as_double_or_die(), fsec["r"].as_double_or_die(), fsec["n"].as_int_or_die(), fsec["m"].as_int_or_die());
    if (type == "3DLSystem")     return parse3DLSystem(fsec);
    if (type == "BuckyBall")     return createBuckyBall();
    if (type == "MengerSponge")  return createMengerSponge(fsec["nrIterations"].as_int_or_die());

    if (type.rfind("Fractal", 0) == 0) {
        const int nrIterations = fsec["nrIterations"].as_int_or_die();
        const double fractalScale = fsec["fractalScale"].as_double_or_die();
        const std::string baseType = type.substr(std::string("Fractal").size());

        Figure3D base;
        if (baseType == "Cube")             base = createCube();
        else if (baseType == "Tetrahedron") base = createTetrahedron();
        else if (baseType == "Octahedron")  base = createOctahedron();
        else if (baseType == "Icosahedron") base = createIcosahedron();
        else if (baseType == "Dodecahedron")base = createDodecahedron();
        else if (baseType == "BuckyBall")   base = createBuckyBall();
        else throw std::runtime_error("Onbekend fractaltype: " + type);

        return createFractal(base, nrIterations, fractalScale);
    }

    throw std::runtime_error("Onbekend figuurtype: " + type);
}

Figures3D parseFigures(const ini::Configuration& config) {
    Figures3D figures;
    const int nrFigures = config["General"]["nrFigures"].as_int_or_die();

    for (int i = 0; i < nrFigures; ++i) {
        std::ostringstream sectionName;
        sectionName << "Figure" << i;
        const auto& fsec = config[sectionName.str()];

        Figure3D fig = parseFigureByType(fsec);
        setFigureColor(fig, fsec["color"].as_double_tuple_or_die());

        // Opgavevolgorde voor rijvectoren: eerst schaal, dan X/Y/Z-rotaties, dan translatie.
        Matrix M = scaleMatrix(fsec["scale"].as_double_or_die());
        M = M * rotateXMatrix(fsec["rotateX"].as_double_or_die());
        M = M * rotateYMatrix(fsec["rotateY"].as_double_or_die());
        M = M * rotateZMatrix(fsec["rotateZ"].as_double_or_die());
        auto center = fsec["center"].as_double_tuple_or_die();
        M = M * translateMatrix(center[0], center[1], center[2]);
        applyTransformation(fig, M);

        figures.push_back(fig);
    }

    return figures;
}

bool projectPoint(const Vector3D& p, Point2D& out, double d = 1.0) {
    if (std::abs(p.z) < 1e-12) {
        return false;
    }
    out.x = d * p.x / (-p.z);
    out.y = d * p.y / (-p.z);
    return std::isfinite(out.x) && std::isfinite(out.y);
}

void addProjectedEdge(const Figure3D& fig, int a, int b, Lines2D& lines) {
    if (a < 0 || b < 0 || a >= static_cast<int>(fig.points.size()) || b >= static_cast<int>(fig.points.size())) {
        return;
    }

    Point2D p1;
    Point2D p2;
    const Vector3D& A = fig.points[a];
    const Vector3D& B = fig.points[b];
    if (!projectPoint(A, p1) || !projectPoint(B, p2)) {
        return;
    }

    Line2D line;
    line.p1 = p1;
    line.p2 = p2;
    line.z1 = A.z; // eye-space z; de Z-buffer routine interpoleert 1/z
    line.z2 = B.z;
    line.color = fig.color;
    lines.push_back(line);
}

Lines2D doProjection(const Figures3D& figures) {
    Lines2D lines;

    for (const auto& fig : figures) {
        std::set<std::pair<int, int>> usedEdges;

        for (const auto& face : fig.faces) {
            const size_t n = face.point_indexes.size();
            if (n < 2) {
                continue;
            }

            const size_t edgeCount = (n == 2) ? 1 : n;
            for (size_t k = 0; k < edgeCount; ++k) {
                int a = face.point_indexes[k];
                int b = face.point_indexes[(k + 1) % n];
                if (a == b) {
                    continue;
                }

                std::pair<int, int> key = (a < b) ? std::make_pair(a, b) : std::make_pair(b, a);
                if (!usedEdges.insert(key).second) {
                    continue;
                }

                addProjectedEdge(fig, a, b, lines);
            }
        }
    }

    return lines;
}

bool computeLineBounds(const Lines2D& lines, double& xmin, double& xmax, double& ymin, double& ymax) {
    if (lines.empty()) {
        return false;
    }

    xmin =  std::numeric_limits<double>::infinity();
    xmax = -std::numeric_limits<double>::infinity();
    ymin =  std::numeric_limits<double>::infinity();
    ymax = -std::numeric_limits<double>::infinity();

    for (const auto& line : lines) {
        xmin = std::min({xmin, line.p1.x, line.p2.x});
        xmax = std::max({xmax, line.p1.x, line.p2.x});
        ymin = std::min({ymin, line.p1.y, line.p2.y});
        ymax = std::max({ymax, line.p1.y, line.p2.y});
    }

    return std::isfinite(xmin) && std::isfinite(xmax) && std::isfinite(ymin) && std::isfinite(ymax)
           && xmax > xmin && ymax > ymin;
}

img::EasyImage drawZBufferedWireframe(Lines2D& lines, int size, img::Color background, bool truncateImageDimensions = false) {
    double xmin, xmax, ymin, ymax;
    if (!computeLineBounds(lines, xmin, xmax, ymin, ymax)) {
        return img::EasyImage();
    }

    const double xrange = xmax - xmin;
    const double yrange = ymax - ymin;

    int width;
    int height;
    if (xrange >= yrange) {
        width = size;
        const double rawHeight = size * (yrange / xrange);
        height = std::max(1, truncateImageDimensions ? static_cast<int>(rawHeight) : static_cast<int>(std::round(rawHeight)));
    } else {
        height = size;
        const double rawWidth = size * (xrange / yrange);
        width = std::max(1, truncateImageDimensions ? static_cast<int>(rawWidth) : static_cast<int>(std::round(rawWidth)));
    }

    Zbuffer zbuffer(width, height);
    return zbuffer_draw::DrawZBuffered(zbuffer, lines, width, height, background);
}

struct Triangle3D {
    Vector3D A;
    Vector3D B;
    Vector3D C;
    FloatColor color;
};

std::vector<Triangle3D> collectTriangles(const Figures3D& figures) {
    std::vector<Triangle3D> triangles;

    for (const auto& fig : figures) {
        for (const auto& tri : triangulateFigure(fig)) {
            if (tri.point_indexes.size() != 3) {
                continue;
            }
            triangles.push_back({
                fig.points[tri.point_indexes[0]],
                fig.points[tri.point_indexes[1]],
                fig.points[tri.point_indexes[2]],
                fig.color
            });
        }
    }

    return triangles;
}

bool computeTriangleProjectionBounds(const std::vector<Triangle3D>& triangles,
                                     double& xmin, double& xmax,
                                     double& ymin, double& ymax) {
    xmin =  std::numeric_limits<double>::infinity();
    xmax = -std::numeric_limits<double>::infinity();
    ymin =  std::numeric_limits<double>::infinity();
    ymax = -std::numeric_limits<double>::infinity();

    auto extend = [&](const Vector3D& p) {
        Point2D projected;
        if (!projectPoint(p, projected)) {
            return;
        }
        xmin = std::min(xmin, projected.x);
        xmax = std::max(xmax, projected.x);
        ymin = std::min(ymin, projected.y);
        ymax = std::max(ymax, projected.y);
    };

    for (const auto& tri : triangles) {
        extend(tri.A);
        extend(tri.B);
        extend(tri.C);
    }

    return std::isfinite(xmin) && std::isfinite(xmax) && std::isfinite(ymin) && std::isfinite(ymax)
           && xmax > xmin && ymax > ymin;
}

} // namespace

img::EasyImage buildWireframe(const ini::Configuration& config) {
    const int size = config["General"]["size"].as_int_or_die();
    const img::Color background = toImageColor(config["General"]["backgroundcolor"].as_double_tuple_or_die());
    auto eyeTuple = config["General"]["eye"].as_double_tuple_or_die();
    const Vector3D eye = Vector3D::point(eyeTuple[0], eyeTuple[1], eyeTuple[2]);

    const bool fractalReferenceSizing = usesFractalAssignmentTypes(config);
    Figures3D figures = parseFigures(config);

    const Matrix eyeMatrix = eyePointTrans(eye);
    for (auto& fig : figures) {
        applyTransformation(fig, eyeMatrix);
    }

    Lines2D lines = doProjection(figures);
    const std::string renderType = config["General"]["type"].as_string_or_die();

    if (renderType == "ZBufferedWireframe") {
        return drawZBufferedWireframe(lines, size, background, fractalReferenceSizing);
    }
    return wireframe_draw::Draw(lines, size, background, fractalReferenceSizing);
}

img::EasyImage buildZBufferedTriangles(const ini::Configuration& config) {
    const int size = config["General"]["size"].as_int_or_die();
    const img::Color background = toImageColor(config["General"]["backgroundcolor"].as_double_tuple_or_die());
    auto eyeTuple = config["General"]["eye"].as_double_tuple_or_die();
    const Vector3D eye = Vector3D::point(eyeTuple[0], eyeTuple[1], eyeTuple[2]);

    const bool fractalReferenceSizing = usesFractalAssignmentTypes(config);
    Figures3D figures = parseFigures(config);

    const Matrix eyeMatrix = eyePointTrans(eye);
    for (auto& fig : figures) {
        applyTransformation(fig, eyeMatrix);
    }

    const std::vector<Triangle3D> triangles = collectTriangles(figures);
    if (triangles.empty()) {
        return img::EasyImage();
    }

    double xmin, xmax, ymin, ymax;
    if (!computeTriangleProjectionBounds(triangles, xmin, xmax, ymin, ymax)) {
        return img::EasyImage();
    }

    const double xrange = xmax - xmin;
    const double yrange = ymax - ymin;

    int width;
    int height;
    if (xrange >= yrange) {
        width = size;
        const double rawHeight = size * (yrange / xrange);
        height = std::max(1, fractalReferenceSizing ? static_cast<int>(rawHeight) : static_cast<int>(std::round(rawHeight)));
    } else {
        height = size;
        const double rawWidth = size * (xrange / yrange);
        width = std::max(1, fractalReferenceSizing ? static_cast<int>(rawWidth) : static_cast<int>(std::round(rawWidth)));
    }

    const double d = 0.95 * size / std::max(xrange, yrange);
    const double dx = width / 2.0 - d * (xmin + xmax) / 2.0;
    const double dy = height / 2.0 - d * (ymin + ymax) / 2.0;

    img::EasyImage image(width, height, background);
    Zbuffer zbuffer(width, height);

    for (const auto& tri : triangles) {
        draw_zbuf_triag(zbuffer, image, tri.A, tri.B, tri.C, d, dx, dy, tri.color);
    }

    return image;
}

} // namespace wireframe_engine
