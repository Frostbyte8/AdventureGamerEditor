#include "gamecharacter.h"
#include "../util/frost.h"
#include "../compat/std_extras_compat.h"

void GameCharacter::Builder::readCharacter(std::ifstream& mapFile) {
       
    std::string line;
    Frost::getLineWindows(mapFile, line);
    ID(std::stoi(line));

    for(int i = 0; i < GameCharacterDescriptions::NumDescriptions; i++) {
        std::getline(mapFile, line);
        description(Frost::trim(Frost::rtrim(line, "\r"), "\""), i);
    }

    Frost::getLineWindows(mapFile, line);
    flags(std::stoi(line));

    Frost::getLineWindows(mapFile, line);
    unused(std::stoi(line));

    Frost::getLineWindows(mapFile, line);
    money(std::stoi(line));

    Frost::getVBString(mapFile, line);

    std::vector<std::string> tokens = Frost::split(line, ',');   

    if(tokens.size() == 2) {
        location(std::stoi(tokens[0]), std::stoi(tokens[1]));        
    }
    else {
        throw std::invalid_argument("Tried to read invalid location type.");
    }

    for(int i = 0; i < AttributeTypes::NumTypesForCharacters; i++) {
        Frost::getLineWindows(mapFile, line);
        attribute(std::stoi(line), i);
    }

    Frost::getLineWindows(mapFile, line);
    sight(std::stoi(line));

    Frost::getLineWindows(mapFile, line);
    type(std::stoi(line));

    Frost::getLineWindows(mapFile, line);
    description(line, GameCharacterDescriptions::Icon);

    Frost::getLineWindows(mapFile, line);
    description(line, GameCharacterDescriptions::Sound);

}
