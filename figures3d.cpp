#include "figures3d.h"
#include <cmath>
#include <algorithm>
#include <map>
#include <utility>
#include <vector>

namespace {

constexpr double PI = 3.141592653589793238462643383279502884;

Face makeFace(std::initializer_list<int> indices) {
    Face f;
    f.point_indexes.assign(indices.begin(), indices.end());
    return f;
}

Face makeFace(const std::vector<int>& indices) {
    Face f;
    f.point_indexes = indices;
    return f;
}

std::vector<Vector3D> icosahedronPoints() {
    std::vector<Vector3D> points;
    points.push_back(Vector3D::point(0, 0, std::sqrt(5.0) / 2.0));
    for (int i = 0; i < 5; ++i) {
        points.push_back(Vector3D::point(
            std::cos(i * 2.0 * PI / 5.0),
            std::sin(i * 2.0 * PI / 5.0),
            0.5));
    }
    for (int i = 0; i < 5; ++i) {
        points.push_back(Vector3D::point(
            std::cos(PI / 5.0 + i * 2.0 * PI / 5.0),
            std::sin(PI / 5.0 + i * 2.0 * PI / 5.0),
            -0.5));
    }
    points.push_back(Vector3D::point(0, 0, -std::sqrt(5.0) / 2.0));
    return points;
}

std::vector<Face> icosahedronFaces() {
    return {
        makeFace({0, 1, 2}), makeFace({0, 2, 3}), makeFace({0, 3, 4}),
        makeFace({0, 4, 5}), makeFace({0, 5, 1}), makeFace({1, 6, 2}),
        makeFace({2, 6, 7}), makeFace({2, 7, 3}), makeFace({3, 7, 8}),
        makeFace({3, 8, 4}), makeFace({4, 8, 9}), makeFace({4, 9, 5}),
        makeFace({5, 9, 10}), makeFace({5, 10, 1}), makeFace({1, 10, 6}),
        makeFace({11, 7, 6}), makeFace({11, 8, 7}), makeFace({11, 9, 8}),
        makeFace({11, 10, 9}), makeFace({11, 6, 10})
    };
}

int midpointIndex(std::vector<Vector3D>& points,
                  std::map<std::pair<int, int>, int>& cache,
                  int a,
                  int b) {
    if (a > b) {
        std::swap(a, b);
    }
    const std::pair<int, int> key(a, b);
    const auto found = cache.find(key);
    if (found != cache.end()) {
        return found->second;
    }

    Vector3D mid = (points[a] + points[b]) / 2.0;
    points.push_back(mid);
    const int idx = static_cast<int>(points.size()) - 1;
    cache[key] = idx;
    return idx;
}

} // namespace

Figure3D createCube() {
    Figure3D cube;
    cube.points = {
        Vector3D::point( 1, -1, -1),
        Vector3D::point(-1,  1, -1),
        Vector3D::point( 1,  1,  1),
        Vector3D::point(-1, -1,  1),
        Vector3D::point( 1,  1, -1),
        Vector3D::point(-1, -1, -1),
        Vector3D::point( 1, -1,  1),
        Vector3D::point(-1,  1,  1)
    };

    cube.faces = {
        makeFace({0, 4, 2, 6}),
        makeFace({4, 1, 7, 2}),
        makeFace({1, 5, 3, 7}),
        makeFace({5, 0, 6, 3}),
        makeFace({6, 2, 7, 3}),
        makeFace({0, 5, 1, 4})
    };
    return cube;
}

Figure3D createTetrahedron() {
    Figure3D tet;
    tet.points = {
        Vector3D::point( 1, -1, -1),
        Vector3D::point(-1,  1, -1),
        Vector3D::point( 1,  1,  1),
        Vector3D::point(-1, -1,  1)
    };

    tet.faces = {
        makeFace({0, 1, 2}),
        makeFace({1, 3, 2}),
        makeFace({0, 3, 1}),
        makeFace({0, 2, 3})
    };
    return tet;
}

Figure3D createOctahedron() {
    Figure3D oct;
    oct.points = {
        Vector3D::point( 1,  0,  0),
        Vector3D::point( 0,  1,  0),
        Vector3D::point(-1,  0,  0),
        Vector3D::point( 0, -1,  0),
        Vector3D::point( 0,  0, -1),
        Vector3D::point( 0,  0,  1)
    };

    oct.faces = {
        makeFace({0, 1, 5}), makeFace({1, 2, 5}), makeFace({2, 3, 5}), makeFace({3, 0, 5}),
        makeFace({1, 0, 4}), makeFace({2, 1, 4}), makeFace({3, 2, 4}), makeFace({0, 3, 4})
    };
    return oct;
}

Figure3D createIcosahedron() {
    Figure3D ico;
    ico.points = icosahedronPoints();
    ico.faces = icosahedronFaces();
    return ico;
}

Figure3D createDodecahedron() {
    Figure3D dodecahedron;
    const std::vector<Vector3D> icoPoints = icosahedronPoints();
    const std::vector<Face> icoFaces = icosahedronFaces();

    for (const Face& face : icoFaces) {
        const Vector3D center = (icoPoints[face.point_indexes[0]] +
                                 icoPoints[face.point_indexes[1]] +
                                 icoPoints[face.point_indexes[2]]) / 3.0;
        dodecahedron.points.push_back(center);
    }

    dodecahedron.faces = {
        makeFace({0, 1, 2, 3, 4}),
        makeFace({0, 5, 6, 7, 1}),
        makeFace({1, 7, 8, 9, 2}),
        makeFace({2, 9, 10, 11, 3}),
        makeFace({3, 11, 12, 13, 4}),
        makeFace({4, 13, 14, 5, 0}),
        makeFace({19, 18, 17, 16, 15}),
        makeFace({19, 14, 13, 12, 18}),
        makeFace({18, 12, 11, 10, 17}),
        makeFace({17, 10, 9, 8, 16}),
        makeFace({16, 8, 7, 6, 15}),
        makeFace({15, 6, 5, 14, 19})
    };
    return dodecahedron;
}

Figure3D createCylinder(int n, double h) {
    Figure3D cylinder;
    if (n < 3) {
        return cylinder;
    }

    for (int i = 0; i < n; ++i) {
        const double angle = 2.0 * PI * i / n;
        cylinder.points.push_back(Vector3D::point(std::cos(angle), std::sin(angle), 0));
    }
    for (int i = 0; i < n; ++i) {
        const double angle = 2.0 * PI * i / n;
        cylinder.points.push_back(Vector3D::point(std::cos(angle), std::sin(angle), h));
    }

    for (int i = 0; i < n; ++i) {
        cylinder.faces.push_back(makeFace({i, (i + 1) % n, n + (i + 1) % n, n + i}));
    }

    std::vector<int> bottom;
    std::vector<int> top;
    for (int i = 0; i < n; ++i) {
        bottom.push_back(n - 1 - i);       // buitenkant van het grondvlak kijkt naar -Z
        top.push_back(n + i);              // buitenkant van het bovenvlak kijkt naar +Z
    }
    cylinder.faces.push_back(makeFace(bottom));
    cylinder.faces.push_back(makeFace(top));

    return cylinder;
}

Figure3D createCone(int n, double h) {
    Figure3D cone;
    if (n < 3) {
        return cone;
    }

    for (int i = 0; i < n; ++i) {
        const double angle = 2.0 * PI * i / n;
        cone.points.push_back(Vector3D::point(std::cos(angle), std::sin(angle), 0));
    }
    const int apex = n;
    cone.points.push_back(Vector3D::point(0, 0, h));

    for (int i = 0; i < n; ++i) {
        cone.faces.push_back(makeFace({i, (i + 1) % n, apex}));
    }

    std::vector<int> base;
    for (int i = 0; i < n; ++i) {
        base.push_back(n - 1 - i);         // buitenkant van het grondvlak kijkt naar -Z
    }
    cone.faces.push_back(makeFace(base));

    return cone;
}

Figure3D createSphere(int n) {
    Figure3D sphere;
    sphere.points = icosahedronPoints();
    sphere.faces = icosahedronFaces();

    for (int iter = 0; iter < n; ++iter) {
        std::map<std::pair<int, int>, int> cache;
        std::vector<Face> newFaces;

        for (const Face& tri : sphere.faces) {
            if (tri.point_indexes.size() != 3) {
                continue;
            }

            const int A = tri.point_indexes[0];
            const int B = tri.point_indexes[1];
            const int C = tri.point_indexes[2];
            const int D = midpointIndex(sphere.points, cache, A, B);
            const int E = midpointIndex(sphere.points, cache, A, C);
            const int F = midpointIndex(sphere.points, cache, B, C);

            newFaces.push_back(makeFace({A, D, E}));
            newFaces.push_back(makeFace({B, F, D}));
            newFaces.push_back(makeFace({C, E, F}));
            newFaces.push_back(makeFace({D, F, E}));
        }

        sphere.faces = newFaces;
    }

    if (n > 0) {
        for (Vector3D& point : sphere.points) {
            point.normalise();
        }
    }

    return sphere;
}

Figure3D createTorus(double R, double r, int n, int m) {
    Figure3D torus;
    if (n < 3 || m < 3) {
        return torus;
    }


    auto idx = [m, n](int i, int j) {
        i = (i % n + n) % n;
        j = (j % m + m) % m;
        return i * m + j;
    };

    for (int i = 0; i < n; ++i) {
        const double u = 2.0 * PI * i / n;
        for (int j = 0; j < m; ++j) {
            const double v = 2.0 * PI * j / m;
            const double x = (R + r * std::cos(v)) * std::cos(u);
            const double y = (R + r * std::cos(v)) * std::sin(u);
            const double z = r * std::sin(v);
            torus.points.push_back(Vector3D::point(x, y, z));
        }
    }

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            torus.faces.push_back(makeFace({idx(i, j), idx(i + 1, j), idx(i + 1, j + 1), idx(i, j + 1)}));
        }
    }

    return torus;
}

namespace {

void appendFigureWithOffset(Figure3D& target, const Figure3D& source, double tx, double ty, double tz) {
    const int offset = static_cast<int>(target.points.size());
    target.points.reserve(target.points.size() + source.points.size());
    for (const Vector3D& p : source.points) {
        target.points.push_back(Vector3D::point(p.x + tx, p.y + ty, p.z + tz));
    }

    target.faces.reserve(target.faces.size() + source.faces.size());
    for (const Face& face : source.faces) {
        Face shifted;
        shifted.point_indexes.reserve(face.point_indexes.size());
        for (int index : face.point_indexes) {
            shifted.point_indexes.push_back(index + offset);
        }
        target.faces.push_back(shifted);
    }
}

Figure3D scaledFigure(const Figure3D& source, double factor) {
    Figure3D scaled = source;
    for (Vector3D& p : scaled.points) {
        p = Vector3D::point(p.x * factor, p.y * factor, p.z * factor);
    }
    return scaled;
}

std::vector<int> sortedIncidentBuckyVertices(
    const Vector3D& center,
    const std::vector<std::pair<Vector3D, int>>& vertices
) {
    if (vertices.size() < 3) {
        std::vector<int> result;
        for (const auto& entry : vertices) {
            result.push_back(entry.second);
        }
        return result;
    }

    Vector3D normal = Vector3D::vector(center.x, center.y, center.z);
    normal.normalise();

    Vector3D u = Vector3D::vector(vertices[0].first.x - center.x,
                                  vertices[0].first.y - center.y,
                                  vertices[0].first.z - center.z);
    // Projecteer u op het raakvlak, zodat atan2 stabiel wordt.
    const double dotUN = Vector3D::dot(u, normal);
    u = Vector3D::vector(u.x - dotUN * normal.x, u.y - dotUN * normal.y, u.z - dotUN * normal.z);
    if (u.length() < 1e-12) {
        u = Vector3D::vector(1, 0, 0);
    }
    u.normalise();
    Vector3D v = Vector3D::cross(normal, u);
    v.normalise();

    std::vector<std::pair<double, int>> angles;
    angles.reserve(vertices.size());
    for (const auto& entry : vertices) {
        Vector3D r = Vector3D::vector(entry.first.x - center.x,
                                      entry.first.y - center.y,
                                      entry.first.z - center.z);
        const double angle = std::atan2(Vector3D::dot(r, v), Vector3D::dot(r, u));
        angles.push_back({angle, entry.second});
    }

    std::sort(angles.begin(), angles.end(), [](const auto& a, const auto& b) {
        return a.first < b.first;
    });

    std::vector<int> result;
    result.reserve(angles.size());
    for (const auto& entry : angles) {
        result.push_back(entry.second);
    }
    return result;
}

} // namespace

Figure3D createBuckyBall() {
    const Figure3D ico = createIcosahedron();
    Figure3D bucky;

    std::map<std::pair<int, int>, int> directedEdgePoint;
    std::vector<std::vector<std::pair<Vector3D, int>>> incident(ico.points.size());

    auto addDirectedPoint = [&](int from, int to) -> int {
        const std::pair<int, int> key(from, to);
        const auto found = directedEdgePoint.find(key);
        if (found != directedEdgePoint.end()) {
            return found->second;
        }

        const Vector3D& A = ico.points[from];
        const Vector3D& B = ico.points[to];
        Vector3D P = Vector3D::point((2.0 * A.x + B.x) / 3.0,
                                     (2.0 * A.y + B.y) / 3.0,
                                     (2.0 * A.z + B.z) / 3.0);
        const int index = static_cast<int>(bucky.points.size());
        bucky.points.push_back(P);
        directedEdgePoint[key] = index;
        incident[from].push_back({P, index});
        return index;
    };

    // Maak per ribbe twee punten: op 1/3 en 2/3 van de ribbe.
    for (const Face& face : ico.faces) {
        if (face.point_indexes.size() != 3) {
            continue;
        }
        for (int i = 0; i < 3; ++i) {
            const int a = face.point_indexes[i];
            const int b = face.point_indexes[(i + 1) % 3];
            addDirectedPoint(a, b);
            addDirectedPoint(b, a);
        }
    }

    // De 20 oorspronkelijke driehoeksvlakken worden zeshoeken.
    for (const Face& face : ico.faces) {
        const int a = face.point_indexes[0];
        const int b = face.point_indexes[1];
        const int c = face.point_indexes[2];
        bucky.faces.push_back(makeFace({
            directedEdgePoint[{a, b}], directedEdgePoint[{b, a}],
            directedEdgePoint[{b, c}], directedEdgePoint[{c, b}],
            directedEdgePoint[{c, a}], directedEdgePoint[{a, c}]
        }));
    }

    // Rond elk oorspronkelijk hoekpunt ontstaat een vijfhoek.
    for (size_t v = 0; v < ico.points.size(); ++v) {
        bucky.faces.push_back(makeFace(sortedIncidentBuckyVertices(ico.points[v], incident[v])));
    }

    return bucky;
}

Figure3D createFractal(const Figure3D& base, int nrIterations, double fractalScale) {
    if (nrIterations <= 0 || base.points.empty() || fractalScale == 0.0) {
        return base;
    }

    const int h = static_cast<int>(base.points.size());
    Figure3D current = base;

    for (int iteration = 1; iteration <= nrIterations; ++iteration) {
        const double factor = 1.0 / std::pow(fractalScale, iteration);
        const Figure3D addBase = scaledFigure(base, factor);
        Figure3D next;

        next.points.reserve(current.points.size() * base.points.size());
        next.faces.reserve(current.points.size() * base.faces.size());

        for (size_t k = 0; k < current.points.size(); ++k) {
            const int j = static_cast<int>(k % h);
            const Vector3D& targetPoint = current.points[k];
            const Vector3D& anchorPoint = addBase.points[j];
            appendFigureWithOffset(next, addBase,
                                   targetPoint.x - anchorPoint.x,
                                   targetPoint.y - anchorPoint.y,
                                   targetPoint.z - anchorPoint.z);
        }

        current = std::move(next);
    }

    current.color = base.color;
    return current;
}

Figure3D createMengerSponge(int nrIterations) {
    struct Center {
        double x;
        double y;
        double z;
    };

    std::vector<Center> centers = {{0.0, 0.0, 0.0}};
    double halfSize = 1.0;

    for (int iter = 0; iter < nrIterations; ++iter) {
        const double childHalfSize = halfSize / 3.0;
        const double step = 2.0 * childHalfSize;
        std::vector<Center> next;
        next.reserve(centers.size() * 20);

        for (const Center& c : centers) {
            for (int dx = -1; dx <= 1; ++dx) {
                for (int dy = -1; dy <= 1; ++dy) {
                    for (int dz = -1; dz <= 1; ++dz) {
                        const int zeros = (dx == 0 ? 1 : 0) + (dy == 0 ? 1 : 0) + (dz == 0 ? 1 : 0);
                        if (zeros <= 1) {
                            next.push_back({c.x + dx * step, c.y + dy * step, c.z + dz * step});
                        }
                    }
                }
            }
        }

        centers = std::move(next);
        halfSize = childHalfSize;
    }

    const Figure3D cubePart = scaledFigure(createCube(), halfSize);
    Figure3D sponge;
    sponge.points.reserve(centers.size() * cubePart.points.size());
    sponge.faces.reserve(centers.size() * cubePart.faces.size());

    for (const Center& c : centers) {
        appendFigureWithOffset(sponge, cubePart, c.x, c.y, c.z);
    }

    return sponge;
}
