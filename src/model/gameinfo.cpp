#include "gameinfo.h"
#include "../compat/std_extras_compat.h"
#include <sstream>
#include <stdexcept>

namespace AttributeDefaults {
    const int Base      = 6;
    const int Random    = 6;
}

GameInfo::GameInfo() : gameName("Untitled Game"), saveName("Master"), isSaveFile(false),
                       currencyName("Dollar"), playerHearing(HearingTypes::Normal), 
                       playerSight(SightTypes::Normal), playerStartX(0), playerStartY(0) {
    
    for(unsigned int i = 0; i < AdventureGamerConstants::NumAttributes; i++) {
        baseAttributes[i]   = AttributeDefaults::Base;
        randomAttributes[i] = AttributeDefaults::Random;
    }

}

///----------------------------------------------------------------------------
/// readHeader - Reads the header of map file.
/// @param mapFile an ifstream of the file to be read from.
/// @throws std::runtime_error if the saveName isn't "Master".
/// TODO: Save game editing?
///----------------------------------------------------------------------------

void GameInfo::readHeader(std::ifstream& mapFile) {

    std::getline(mapFile, gameName);
    std::getline(mapFile, saveName);

    if(!saveName.compare("Master")) {
        throw std::runtime_error("File is not an Adventure Gamer World File.");
        isSaveFile = true; // Maybe in the future we can add this into the editor.
    }

    std::getline(mapFile, currencyName);

}

///----------------------------------------------------------------------------
/// readAttributes - Reads the player's attributes from the file.
/// @param mapFile an ifstream of the file to be read from.
/// @throws std::runtime_error if any of the numbers read are invalid.
///----------------------------------------------------------------------------

void GameInfo::readPlayerAttributes(std::ifstream &mapFile) {
    
    std::string lineRead;

    // Try and read the Headings for each attribute, and their values

    try {

        for(int i = 0; i < AdventureGamerConstants::NumAttributes; i++) {
            
            std::getline(mapFile, lineRead);

            if(!(lineRead.compare(AdventureGamerSubHeadings::Attributes[i]))) {
                throw std::runtime_error("Subheading read error. Expected \"" + 
                                         AdventureGamerSubHeadings::Attributes[i] + 
                                         "\", but got \"" + lineRead + "\".");
            }

            std::getline(mapFile, lineRead);
            baseAttributes[i]   = std::stoi(lineRead);

            std::getline(mapFile, lineRead);
            randomAttributes[i] = std::stoi(lineRead);

        }

        // Read last 4 remaining integer values

        // Read the integer
        // TODO: If save game editing is added, these values can be used.
        
        std::getline(mapFile, lineRead);
        const int newSight = std::stoi(lineRead);

        std::getline(mapFile, lineRead);
        const int newHearing = std::stoi(lineRead);

        if(isSaveFile) {
            playerSight     = newSight;
            playerHearing   = newHearing;
        }

        std::getline(mapFile, lineRead);
        playerStartX = std::stoi(lineRead);
        std::getline(mapFile, lineRead);
        playerStartY = std::stoi(lineRead);

    }
    catch (const std::invalid_argument& e) {
        throw std::runtime_error("Tried to read a attribute value, but did not get a valid integer.");
    }
    catch (const std::out_of_range& e) {
        throw std::runtime_error("Tried to read an attribute value, but the value was outside the vaild integer range.");
    }

}