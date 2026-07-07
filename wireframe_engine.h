#ifndef WIREFRAME_ENGINE_H
#define WIREFRAME_ENGINE_H

#include "vector3d.h"             // Matrix, Vector3D
#include "transformations.h"      // namespace transformations
#include "eye_transform.h"        // namespace eye_transform
#include "projection.h"           // namespace projection
#include "lsystem_draw.h"   // Draw2DLines
#include "ini_configuration.h"
#include "easy_image.h"     // img::EasyImage

namespace wireframe_engine {

    img::EasyImage buildWireframe(const ini::Configuration &config);
    img::EasyImage buildZBufferedTriangles(const ini::Configuration &config);  // ✅ toegevoegd

} // namespace wireframe_engine

#endif // WIREFRAME_ENGINE_H
