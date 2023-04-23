#include "gameobject.h"
#include "../util/frost.h"
#include "../editor_constants.h"
#include "../compat/std_extras_compat.h"

void GameObject::Builder::readObject(std::ifstream& mapFile) {
    
    std::string line;
    Frost::getLineWindows(mapFile, line);
    ID(std::stoi(line));

    for(int i = 0; i < GameObjectDescriptions::NumDescriptions; i++) {
        std::getline(mapFile, line);
        description(Frost::trim(Frost::rtrim(line, "\r"), "\""), i);
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

    Frost::getLineWindows(mapFile, line);
    location(Frost::trim(line, "\""));

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