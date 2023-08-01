#ifndef __GAMEINFO_H__
#define __GAMEINFO_H__

#include <string>
#include <fstream>
#include "../compat/stdint_compat.h"
#include "../editor_constants.h"
#include "../adventuregamer_constants.h"

//-----------------------------------------------------------------------------
// GameInfoConstants - Game Info Constants. They're Constant!
//-----------------------------------------------------------------------------

namespace GameInfoConstants {
    const unsigned int MaxWorldName         = 192;
    const unsigned int MaxCurrencyName      = 64;
}

///----------------------------------------------------------------------------
/// AdventureGamerHeadings - Various headings the File format uses
///----------------------------------------------------------------------------

namespace AdventureGamerHeadings {
    const std::string Row           = "{row ";      // Space is intentional.
    const std::string Jumps         = "{jumps";
    const std::string Switches      = "{swtchs";
    const std::string Attributes    = "{attrb";
    const std::string Objects       = "{objct";
    const std::string Characters    = "{cretr";
}

///----------------------------------------------------------------------------
/// AdventureGamerSubHeadings - Some headings even have sub headings
///----------------------------------------------------------------------------

namespace AdventureGamerSubHeadings {
    const unsigned int NumAttributeSubHeadings = AttributeTypes::NumTypes;
    const std::string Attributes[NumAttributeSubHeadings] = { "Stamina", "Skill", "Willpower", "Luck", "Torch Life" };
}

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

        const std::string& getGameName() const { return gameName; }
        const std::string& getCurrencyName() const { return currencyName; }
        const int getBaseAttribute(const int& which) const { return baseAttributes[which]; }
        const int getRandomAttribute(const int& which) const { return randomAttributes[which]; }

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
