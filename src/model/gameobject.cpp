#include "gameobject.h"
#include "../util/frost.h"
#include "../editor_constants.h"
#include "../compat/std_extras_compat.h"

void GameObject::Builder::readObject(std::ifstream& mapFile) {
    
    std::string line;
    Frost::getLineWindows(mapFile, line);
    ID(std::stoi(line));

    for(int i = 0; i < GameObjectDescriptions::NumDescriptions; ++i) {
        Frost::getVBString(mapFile, line);
        description(line, i);
    }

    Frost::getLineWindows(mapFile, line);
    doorColumn(std::stoi(line));

    Frost::getLineWindows(mapFile, line);
    doorRow(std::stoi(line));

    Frost::getLineWindows(mapFile, line);
    flags1(std::stoi(line));

    Frost::getLineWindows(mapFile, line);
    flags2(std::stoi(line));

    Frost::getLineWindows(mapFile, line);
    monetaryWorth(std::stoi(line));

    Frost::getLineWindows(mapFile, line);
    uses(std::stoi(line));

    Frost::getVBString(mapFile, line);

    std::vector<std::string> tokens = Frost::split(line, ',');

    if(tokens.size() == 2) {
        if(Frost::endsWith(tokens[1], GameObjectConstants::OnCharacterString)) {
            location(std::stoi(tokens[0]));
        }
        else {
            location(std::stoi(tokens[0]), std::stoi(tokens[1]));
        }
    }
    else if(tokens.size() == 1) {
        if(Frost::startsWith(tokens[0], GameObjectConstants::OnPlayerString)) {
            location(); 
        }
        else {
            throw std::invalid_argument("Tried to read invalid location type");
        }
    }
    else {
        throw std::invalid_argument("Too many tokens for location type");
    }

    for(int i = 0; i < AttributeTypes::NumTypes; i++) {
        Frost::getLineWindows(mapFile, line);
        attributeBase(std::stoi(line), i);

        Frost::getLineWindows(mapFile, line);
        attributeRandom(std::stoi(line), i);
    }

    Frost::getLineWindows(mapFile, line);
    makesSight(std::stoi(line));

    Frost::getLineWindows(mapFile, line);
    makesHearing(std::stoi(line));

    Frost::getLineWindows(mapFile, line);
    description(line, 4);

    Frost::getLineWindows(mapFile, line);
    description(line, 5);

    Frost::getLineWindows(mapFile, line);
    usedWithID(std::stoi(line));

}

void GameObject::writeObject(std::ofstream& mapFile) const {

    Frost::writeVBInteger(mapFile, base.ID);

    for (int i = 0; i < GameObjectDescriptions::NumDescriptions; ++i) {
        Frost::writeVBString(mapFile, base.description[i]);
    }

    Frost::writeVBInteger(mapFile, base.doorColumn);
    Frost::writeVBInteger(mapFile, base.doorRow);
    Frost::writeVBInteger(mapFile, base.flags1);
    Frost::writeVBInteger(mapFile, base.flags2);
    Frost::writeVBInteger(mapFile, base.monetaryWorth);
    Frost::writeVBInteger(mapFile, base.uses);

    Frost::writeVBString(mapFile, base.location);

    for (int k = 0; k < AttributeTypes::NumTypes; ++k) {
        Frost::writeVBInteger(mapFile, base.attributeBase[k]);
        Frost::writeVBInteger(mapFile, base.attributeRandom[k]);
    }

    Frost::writeVBInteger(mapFile, base.makesSight);
    Frost::writeVBInteger(mapFile, base.makesHearing);

    Frost::writeVBLine(mapFile,
                       base.description[GameObjectDescriptions::Icon]);

    Frost::writeVBLine(mapFile,
                       base.description[GameObjectDescriptions::Sound]);

    Frost::writeVBInteger(mapFile, base.usedWithID);

}