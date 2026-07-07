#ifndef LSYS_3D_GENERATOR_H
#define LSYS_3D_GENERATOR_H

#include "vector3d.h"    // Voor Vector3D en Matrix
#include "l_parser.h"    // Voor de LParser::LSystem3D klasse
#include <vector>
#include <string>
#include <stack>         // Voor het opslaan van de transformatiestack

#include "Structuren.h"  // <<< BELANGRIJK: Voeg deze toe om Figure3D en FloatColor te gebruiken!




namespace LSystem3DGenerator // Let op: dit is een namespace, geen klasse
{
    /**
     * @brief Genereert een 3D-figuur op basis van een 3D L-systeem.
     *
     * Deze functie interpreteert de regels van het L-systeem en genereert
     * de bijbehorende 3D-geometrie. Het gebruikt een "turtle graphics"
     * benadering in 3D.
     *
     * @param lsystem De LSystem3D-object dat de regels en parameters bevat.
     * @return Een Figure3D-object dat de gegenereerde 3D-geometrie bevat.
     */
    Figure3D generate3DLSystem(const LParser::LSystem3D& lsystem); // <<< HIER 'Figure' veranderd naar 'Figure3D'

    // Hier kunnen eventueel helperfuncties of klassen komen die intern worden gebruikt
    // voor de generatie, bijv. een 'Turtle' klasse.
}

#endif // LSYS_3D_GENERATOR_H