#include "gamecharacter.h"
#include "../util/frost.h"
#include "../compat/std_extras_compat.h"

void GameCharacter::readCharacter(std::ifstream& mapFile) {

    std::string line;
    std::getline(mapFile, line);
    line = Frost::rtrim(line, 13);

    // Read the header
    // TODO: Header reading should not be done here either

    // Read the header
    // TODO: Header reading should not be done here, read object should strictly read
    // just the object data.

    if(AdventureGamerHeadings::Characters.compare(line)) {
        throw std::runtime_error("Error reading file. Expected \"" + AdventureGamerHeadings::Characters + "\", but got \"" + line + "\".");
    }

    try {
        
        std::getline(mapFile, line);
        const int numCharacters = std::stoi(line);
        
        
        for(int k = 0; k < numCharacters; k++) {
            GameCharacter::Builder builder;

            std::getline(mapFile, line);
            builder.ID(std::stoi(line));

            for(int i = 0; i < GameCharacterDescriptions::NumDescriptions; i++) {
                std::getline(mapFile, line);
                builder.description(Frost::trim(Frost::rtrim(line, 13), '"'), i);
            }

            std::getline(mapFile, line);
            builder.flags(std::stoi(line));

            std::getline(mapFile, line);
            builder.unused(std::stoi(line));

            std::getline(mapFile, line);
            builder.money(std::stoi(line));

            std::getline(mapFile, line);
            builder.location(line);

            for(int i = 0; i < AttributeTypes::NumTypesForCharacters; i++) {
                std::getline(mapFile, line);
                builder.attribute(std::stoi(line), i);
            }

            std::getline(mapFile, line);
            builder.sight(std::stoi(line));

            std::getline(mapFile, line);
            builder.type(std::stoi(line));

            std::getline(mapFile, line);
            builder.description(line, GameCharacterDescriptions::Icon);

            std::getline(mapFile, line);
            builder.description(line, GameCharacterDescriptions::Sound);

            //return builder.build();

        }

    }
    catch(std::invalid_argument& e) {
    }
}