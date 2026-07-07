#include "triangulate.h"

std::vector<Face> triangulateFigure(const Figure3D& fig) {
    std::vector<Face> triangles;

    for (const auto& face : fig.faces) {
        const size_t n = face.point_indexes.size();
        if (n < 3) {
            continue;
        }

        if (n == 3) {
            triangles.push_back(face);
            continue;
        }

        const int first_idx = face.point_indexes[0];
        for (size_t i = 1; i + 1 < n; ++i) {
            Face triangle;
            triangle.point_indexes = {first_idx, face.point_indexes[i], face.point_indexes[i + 1]};
            triangles.push_back(triangle);
        }
    }

    return triangles;
}
