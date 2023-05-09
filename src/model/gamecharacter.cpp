#include "gamecharacter.h"
#include "../util/frost.h"
#include "../compat/std_extras_compat.h"

void GameCharacter::Builder::readCharacter(std::ifstream& mapFile) {
       
    std::string line;
    std::getline(mapFile, line);
    ID(std::stoi(line));

    for(int i = 0; i < GameCharacterDescriptions::NumDescriptions; i++) {
        std::getline(mapFile, line);
        description(Frost::trim(Frost::rtrim(line, "\r"), "\""), i);
    }

    std::getline(mapFile, line);
    flags(std::stoi(line));

    std::getline(mapFile, line);
    unused(std::stoi(line));

    std::getline(mapFile, line);
    money(std::stoi(line));

    std::getline(mapFile, line);

    std::vector<std::string> tokens = Frost::split(line, ',');

    if(tokens.size() == 2) {
        // TODO: Try
        location(std::stoi(tokens[0]), std::stoi(tokens[1]));
    }
    else {
        throw std::invalid_argument("Tried to read invalid location type");
    }

    for(int i = 0; i < AttributeTypes::NumTypesForCharacters; i++) {
        std::getline(mapFile, line);
        attribute(std::stoi(line), i);
    }

    std::getline(mapFile, line);
    sight(std::stoi(line));

    std::getline(mapFile, line);
    type(std::stoi(line));

    std::getline(mapFile, line);
    description(line, GameCharacterDescriptions::Icon);

    std::getline(mapFile, line);
    description(line, GameCharacterDescriptions::Sound);

}
