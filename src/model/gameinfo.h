#ifndef __GAMEINFO_H__
#define __GAMEINFO_H__

#include <string>
#include <fstream>
#include "../compat/stdint_compat.h"
#include "../editor_constants.h"

class GameMap;

class GameInfo {

    public:
        GameInfo();

        // Only allow certain classes to use these functions
        
        class Key {
            friend class GameMap;
            Key() {};
            Key(Key &t) {};
        };
        

        inline void readHeader(Key, std::ifstream& mapFile) { readHeader(mapFile); }
        inline void readPlayerAttributes(Key, std::ifstream& mapFile) { readPlayerAttributes(mapFile); }
        inline void writeHeader(Key, std::ofstream& mapFile) { writeHeader(mapFile); }

	private:

        void readHeader(std::ifstream& mapFile);
        void readPlayerAttributes(std::ifstream& mapFile);
        void writeHeader(std::ofstream& mapFile);

		std::string         gameName;
		std::string         saveName; // If it isn't "Master", it's a save game
		std::string         currencyName;
        uint8_t	            baseAttributes[AttributeTypes::NumTypes];
		uint8_t	            randomAttributes[AttributeTypes::NumTypes];
        HearingTypes        playerHearing;
        SightTypes          playerSight;
        uint8_t             playerStartX;
        uint8_t             playerStartY;
        bool                isSaveFile;
};

#endif // __GAMEINFO_H__