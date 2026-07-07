#include "eye_transform.h" // Bevat Figure3D, Matrix, Vector3D
#include <cmath>            // Voor std::sqrt, std::atan2, std::acos, std::sin, std::cos

// Zorg ervoor dat M_PI beschikbaar is
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Matrix eyePointTrans(const Vector3D &eye) {
    // Bereken r, theta, phi zoals in de PDF (p36-37)
    double r = std::sqrt(eye.x * eye.x + eye.y * eye.y + eye.z * eye.z);

    double theta, phi;

    // Special case: eye is at the origin.
    // The view matrix is undefined in this case by the formulas.
    // Return an identity matrix, or handle as an error, or set a default view.
    // For now, returning identity if r is very small.
    if (r < 1e-9) {
        // Optional: print a warning or log this special case.
        // std::cerr << "Warning: eyePointTrans called with eye at origin." << std::endl;
        Matrix id_matrix; // Assuming default constructor gives identity
        return id_matrix;
    }

    theta = std::atan2(eye.y, eye.x); // Azimuthal angle (around Z-axis)
    phi   = std::acos(eye.z / r);     // Polar (zenith) angle (from Z-axis)

    double st = std::sin(theta);
    double ct = std::cos(theta);
    double sp = std::sin(phi);
    double cp = std::cos(phi);

    Matrix V_transpose; // Start als identiteitsmatrix (aanname)

    // Vul V_transpose (getransponeerde van V uit PDF p38)
    // V_pdf (p38) =
    // | -st      ct      0     0 |
    // | -ct*cp  -st*cp   sp    0 |
    // |  ct*sp   st*sp   cp    0 |
    // |   0       0     -r     1 |

    // V_transpose (voor P_eye = P_world * V_transpose) =
    // | -st    -ct*cp   ct*sp  0 |
    // |  ct    -st*cp   st*sp  0 |
    // |  0       sp      cp    0 |
    // |  0       0      -r     1 |

    // Kolom 1 van V_transpose (Rij 1 van V_pdf)
    V_transpose(1,1) = -st;
    V_transpose(2,1) =  ct;
    V_transpose(3,1) =  0.0;
    V_transpose(4,1) =  0.0; // Translatie component

    // Kolom 2 van V_transpose (Rij 2 van V_pdf)
    V_transpose(1,2) = -ct * cp;
    V_transpose(2,2) = -st * cp;
    V_transpose(3,2) =  sp;
    V_transpose(4,2) =  0.0; // Translatie component

    // Kolom 3 van V_transpose (Rij 3 van V_pdf)
    V_transpose(1,3) =  ct * sp;
    V_transpose(2,3) =  st * sp;
    V_transpose(3,3) =  cp;
    V_transpose(4,3) = -r;   // Dit is de belangrijke translatiecomponent.
                           // Het plaatst de wereld-oorsprong op (0,0,-r) in eye-coördinaten,
                           // en het eye-point zelf op (0,0,0) in eye-coördinaten.

    // Kolom 4 van V_transpose (Rij 4 van V_pdf)
    V_transpose(1,4) = 0.0; // Homogene coördinaat
    V_transpose(2,4) = 0.0; // Homogene coördinaat
    V_transpose(3,4) = 0.0; // Homogene coördinaat
    V_transpose(4,4) = 1.0; // Homogene coördinaat

    return V_transpose;
}

// Pas de gegeven transformatiematrix toe op alle punten van een 3D figuur.
void applyTransformation(Figure3D &fig, const Matrix &matrix) {
    for (unsigned int i = 0; i < fig.points.size(); ++i) {
        fig.points[i] = fig.points[i] * matrix; // Vector3D * Matrix operatie
    }
}