#include "gameinfo.h"
#include "../compat/std_extras_compat.h"
#include <sstream>

namespace AttributeDefaults {
    const int Base      = 6;
    const int Random    = 6;
}

GameInfo::GameInfo() : gameName("Untitled Game"), unusedHeaderString("Master"),
                       currencyName("Dollar"), playerDefaultHearing(HearingTypes::Normal), 
                       playerDefaultSight(SightTypes::Normal), playerStartX(0), 
                       playerStartY(0) {
    
    for(int i = 0; i < AdventureGamerConstants::NumAttributes; i++) {
        baseAttributes[i]   = AttributeDefaults::Base;
        randomAttributes[i] = AttributeDefaults::Random;
    }
}

///----------------------------------------------------------------------------
/// readHeader - Reads the header of map file.
/// @param mapFile an ifstream of the file to be read from.
///
///----------------------------------------------------------------------------

bool GameInfo::readHeader(std::ifstream& mapFile) {

    std::getline(mapFile, gameName);
    std::getline(mapFile, unusedHeaderString);

    if(!!unusedHeaderString.compare("Master")) {
        return false;
    }

    std::getline(mapFile, currencyName);

    return true;
}