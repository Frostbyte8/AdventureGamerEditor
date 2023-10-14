#include "gameinfo.h"
#include "../compat/std_extras_compat.h"
#include "../util/frost.h"
#include <sstream>
#include <stdexcept>

namespace AttributeDefaults {
    const int Base      = 6;
    const int Random    = 6;
}

GameInfo::GameInfo() : gameName("Untitled Game"), saveName("Master"), isSaveFile(false),
                       currencyName("Dollar"), playerHearing(HearingTypes::Normal()), 
                       playerSight(SightTypes::Normal()), playerStartX(0), playerStartY(0) {
    
    for(unsigned int i = 0; i < AttributeTypes::NumTypes; i++) {
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

    Frost::getLineWindows(mapFile, gameName);
    Frost::getLineWindows(mapFile, saveName);

    // TODO: If the World doesn't load and this exception runs, it leaks memory.
    // This is a good place to test that!
    // TODO: Master needs to be defined somewhere.

    if(saveName.compare("Master")) {
        throw std::runtime_error("File is not an Adventure Gamer World File.");
        isSaveFile = true; // Maybe in the future we can add this into the editor.
    }

    Frost::getLineWindows(mapFile, currencyName);

}

///----------------------------------------------------------------------------
/// readAttributes - Reads the player's attributes from the file.
/// @param mapFile an ifstream of the file to be read from.
/// @throws std::runtime_error if any of the numbers read are invalid.
///----------------------------------------------------------------------------

void GameInfo::readPlayerAttributes(std::ifstream& mapFile) {
    
    std::string line;
    std::string errorMsg = "Error reading attributes: ";

    // Try and read the Headings for each attribute, and their values
    std::getline(mapFile, line);
    line = Frost::rtrim(line, "\r");

    if(AdventureGamerHeadings::Attributes.compare(line)) {
        errorMsg.append("Expected \"" + AdventureGamerHeadings::Attributes + "\", but got \"" + line + "\".");
        throw std::runtime_error(errorMsg);
    }

    try {

        for(int i = 0; i < AttributeTypes::NumTypes; i++) {
            
            std::getline(mapFile, line);

            // TODO: Handle Energy/Stamina discrepancy
            
            if(!(line.compare(AdventureGamerSubHeadings::Attributes[i]))) {
                errorMsg.append("Expected \"" + AdventureGamerSubHeadings::Attributes[i] + "\", but got \"" + line + "\".");
                throw std::runtime_error(errorMsg);
            }
            

            std::getline(mapFile, line);
            baseAttributes[i]   = std::stoi(line);

            std::getline(mapFile, line);
            randomAttributes[i] = std::stoi(line);

        }

        // Read last 4 remaining integer values

        // Read the integer
        // TODO: If save game editing is added, these values can be used.
        
        std::getline(mapFile, line);
        const int newSight = std::stoi(line);

        std::getline(mapFile, line);
        const int newHearing = std::stoi(line);

        // These have to be ignored if it's not a save file as if you start the
        // game Blind/Ultra-Sonic, etc it will crash the game upon equipping
        // things.

        if(isSaveFile) {
            //playerSight     = newSight;
            //playerHearing   = newHearing;
        }

        std::getline(mapFile, line);
        playerStartX = std::stoi(line);
        std::getline(mapFile, line);
        playerStartY = std::stoi(line);

    }
    catch (const std::invalid_argument& e) {
        errorMsg.append(e.what());
        throw std::runtime_error(errorMsg);
    }
    catch (const std::out_of_range& e) {
        errorMsg.append(e.what());
        throw std::runtime_error(errorMsg);
    }

}

///----------------------------------------------------------------------------
/// writeAttributes - Writes the player's attributes to the file given
/// @param mapFile an ofstream of the file being written to
///----------------------------------------------------------------------------

void GameInfo::writePlayerAttributes(std::ofstream& mapFile) {
    
    Frost::writeVBLine(mapFile, AdventureGamerHeadings::Attributes);

    for (int i = 0; i < AdventureGamerSubHeadings::NumAttributeSubHeadings; ++i) {
        Frost::writeVBLine(mapFile, AdventureGamerSubHeadings::Attributes[i]);
        Frost::writeVBInteger(mapFile, baseAttributes[i]);
        Frost::writeVBInteger(mapFile, randomAttributes[i]);
    }

    Frost::writeVBInteger(mapFile, SightTypes::Normal().asInt());
    Frost::writeVBInteger(mapFile, HearingTypes::Normal().asInt());
    
    Frost::writeVBInteger(mapFile, playerStartX);
    Frost::writeVBInteger(mapFile, playerStartY);
}

///----------------------------------------------------------------------------
/// writeHeader - Writes the header of the map to the file given
/// @param mapFile an ofstream of the file being written to
///----------------------------------------------------------------------------

void GameInfo::writeHeader(std::ofstream& mapFile) {

    Frost::writeVBLine(mapFile, gameName);
    Frost::writeVBLine(mapFile, saveName);
    Frost::writeVBLine(mapFile, currencyName);

}