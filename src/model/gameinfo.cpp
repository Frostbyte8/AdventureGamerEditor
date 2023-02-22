#include "gameinfo.h"

namespace AttributeDefaults {
    const int Base      = 6;
    const int Random    = 6;
}

GameInfo::GameInfo() : gameName("Untitled Game"), unusedHeaderString("Master"),
                       currencyName("Dollar"), playerDefaultHearing(HearingTypes::Normal), 
                       playerDefaultSight(SightTypes::Normal), playerStartX(0), 
                       playerStartY(0) {
    
    for(int i = 0; i < AdventureGamerConstants::NumAttributes; i++) {
        // TODO: What is the default starting value the builtin
        // editor gives?
        baseAttributes[i]   = AttributeDefaults::Base;
        randomAttributes[i] = AttributeDefaults::Random;
    }
}