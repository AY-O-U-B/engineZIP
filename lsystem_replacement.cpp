#include "lsystem_replacement.h"

std::string applyReplacements(const LParser::LSystem2D &l_system) {
    std::string result = l_system.get_initiator(); // Start met de initiator
    unsigned int iterations = l_system.get_nr_iterations();

    for (unsigned int i = 0; i < iterations; i++) {
        std::string next;
        for (char c : result) {
            if (l_system.get_alphabet().count(c) > 0) {  // Controleer of het symbool in het alfabet zit
                if (l_system.get_replacement(c) != "") { // Controleer of er een vervangingsregel is
                    next += l_system.get_replacement(c);
                } else {
                    next += c;  // Als geen vervanging, gewoon toevoegen
                }
            } else {
                next += c; // Als het niet in het alfabet zit, laat het ongewijzigd
            }
        }
        result = next; // De gegenereerde string wordt de nieuwe basis voor de volgende iteratie
    }
    return result;
}
