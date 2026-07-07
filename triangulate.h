#ifndef TRIANGULATE_H
#define TRIANGULATE_H

#include "Structuren.h"  // Voor Figure3D en Face
#include <vector>

/**
 * @brief Converteert alle faces van een figuur naar driehoeken.
 * 
 * Faces met 3 punten blijven ongewijzigd.
 * Faces met 4+ punten worden opgesplitst in driehoeken via "fan triangulation":
 * Voor face [0,1,2,3,4] → triangles: [0,1,2], [0,2,3], [0,3,4]
 * 
 * @param fig De Figure3D waarvan de faces getrianguleerd moeten worden
 * @return Vector met Face objecten die allemaal exact 3 punten bevatten
 */
std::vector<Face> triangulateFigure(const Figure3D& fig);

#endif // TRIANGULATE_H