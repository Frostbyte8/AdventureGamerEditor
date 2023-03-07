#include "gameobject.h"
#include "../util/frost.h"
#include "../editor_constants.h"
#include "../compat/std_extras_compat.h"
#include <windows.h>

void GameObject::readObject(std::ifstream& mapFile) {
    
    std::string line;
    std::getline(mapFile, line);
    line = Frost::rtrim(line, 13);

    // Read the header
    // TODO: Header reading is for the Mapfile, read object should strictly read
    // just the object data.

    if(AdventureGamerHeadings::Objects.compare(line)) {
        throw std::runtime_error("Error reading file. Expected \"" + AdventureGamerHeadings::Objects + "\", but got \"" + line + "\".");
    }

    try {
        
        std::getline(mapFile, line);
        const int numObjects = std::stoi(line);
        GameObject::Builder builder;
        
        for(int k = 0; k < numObjects; k++) {

            std::getline(mapFile, line);
            builder.ID(std::stoi(line));

            for(int i = 0; i < GameObjectDescriptions::NumDescriptions; i++) {
                std::getline(mapFile, line);
                const std::string desc = Frost::trim(line, '"');
                builder.description(desc, i);

                if(i == 0) {
                    OutputDebugStringA(line.c_str());
                }
            }

            std::getline(mapFile, line);
            builder.doorColumn(std::stoi(line));

            std::getline(mapFile, line);
            builder.doorRow(std::stoi(line));

            std::getline(mapFile, line);
            builder.flags1(std::stoi(line));

            std::getline(mapFile, line);
            builder.flags2(std::stoi(line));

            std::getline(mapFile, line);
            builder.monetaryWorth(std::stoi(line));

            std::getline(mapFile, line);
            builder.uses(std::stoi(line));

            std::getline(mapFile, line);
            builder.location(Frost::trim(line, '"'));

            for(int i = 0; i < AttributeTypes::NumTypes; i++) {
                std::getline(mapFile, line);
                builder.attributeBase(std::stoi(line), i);

                std::getline(mapFile, line);
                builder.attributeRandom(std::stoi(line), i);
            }

            std::getline(mapFile, line);
            builder.makesSight(std::stoi(line));

            std::getline(mapFile, line);
            builder.makesHearing(std::stoi(line));

            // TODO: Enum Class the remaining enums
            std::getline(mapFile, line);
            builder.description(line, 4);

            std::getline(mapFile, line);
            builder.description(line, 5);


            std::getline(mapFile, line);
            builder.usedWithID(std::stoi(line));

            
        }

        // return builder.build(); 

    }
    catch(std::invalid_argument& e) {
    }
   
}
