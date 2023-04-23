#include "gameobject.h"
#include "../util/frost.h"
#include "../editor_constants.h"
#include "../compat/std_extras_compat.h"

void GameObject::Builder::readObject(std::ifstream& mapFile) {
    
    std::string line;
    Frost::getLineWindows(mapFile, line);
    ID(std::stoi(line));

    for(int i = 0; i < GameObjectDescriptions::NumDescriptions; i++) {
        //std::getline(mapFile, line);
        Frost::getVBString(mapFile, line);
        //description(Frost::trim(Frost::rtrim(line, "\r"), "\""), i);
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
            // Throw
        }
    }
    else {
        // Throw
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