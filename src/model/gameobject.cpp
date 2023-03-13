#include "gameobject.h"
#include "../util/frost.h"
#include "../editor_constants.h"
#include "../compat/std_extras_compat.h"

void GameObject::Builder::readObject(std::ifstream& mapFile) {
    
    std::string line;
    std::getline(mapFile, line);
    ID(std::stoi(line));

    for(int i = 0; i < GameObjectDescriptions::NumDescriptions; i++) {
        std::getline(mapFile, line);
        description(Frost::trim(Frost::rtrim(line, 13), '"'), i);
    }

    std::getline(mapFile, line);
    doorColumn(std::stoi(line));

    std::getline(mapFile, line);
    doorRow(std::stoi(line));

    std::getline(mapFile, line);
    flags1(std::stoi(line));

    std::getline(mapFile, line);
    flags2(std::stoi(line));

    std::getline(mapFile, line);
    monetaryWorth(std::stoi(line));

    std::getline(mapFile, line);
    uses(std::stoi(line));

    std::getline(mapFile, line);
    location(Frost::trim(line, '"'));

    for(int i = 0; i < AttributeTypes::NumTypes; i++) {
        std::getline(mapFile, line);
        attributeBase(std::stoi(line), i);

        std::getline(mapFile, line);
        attributeRandom(std::stoi(line), i);
    }

    std::getline(mapFile, line);
    makesSight(std::stoi(line));

    std::getline(mapFile, line);
    makesHearing(std::stoi(line));

    // TODO: Enum Class the remaining enums
    std::getline(mapFile, line);
    description(line, 4);

    std::getline(mapFile, line);
    description(line, 5);


    std::getline(mapFile, line);
    usedWithID(std::stoi(line));

}