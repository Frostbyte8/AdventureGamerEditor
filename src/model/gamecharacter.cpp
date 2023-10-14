#include "gamecharacter.h"
#include "../util/frost.h"
#include "../compat/std_extras_compat.h"

void GameCharacter::Builder::readCharacter(std::ifstream& mapFile) {
       
    std::string line;
    Frost::getLineWindows(mapFile, line);
    ID(std::stoi(line));

    for(int i = 0; i < GameCharacterDescriptions::NumDescriptions; ++i) {
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

void GameCharacter::writeCharacter(std::ofstream& mapFile) const {

    Frost::writeVBInteger(mapFile, base.ID);

    for (int i = 0; i < GameCharacterDescriptions::NumDescriptions; ++i) {
        Frost::writeVBString(mapFile, base.description[i]);
    }

    Frost::writeVBInteger(mapFile, base.flags);
    Frost::writeVBInteger(mapFile, base.unused);
    Frost::writeVBInteger(mapFile, base.money);

    Frost::writeVBString(mapFile, base.location);

    for (int k = 0; k < AttributeTypes::NumTypesForCharacters; ++k) {
        Frost::writeVBInteger(mapFile, base.attribute[k]);
    }

    Frost::writeVBInteger(mapFile, base.sight);
    Frost::writeVBInteger(mapFile, base.type);

    Frost::writeVBLine(mapFile, base.description[GameCharacterDescriptions::Icon]);
    Frost::writeVBLine(mapFile, base.description[GameCharacterDescriptions::Sound]);

}
