#include "figures3d.h"
#include <cmath>
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
