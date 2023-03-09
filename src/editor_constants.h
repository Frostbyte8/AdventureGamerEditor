#ifndef __EDITOR_CONSTANTS_H__
#define __EDITOR_CONSTANTS_H__

#include <string>
#include "compat/stdint_compat.h"

///----------------------------------------------------------------------------
/// EditorConstants - Useful constats that are used by the editor
///----------------------------------------------------------------------------

namespace EditorConstants {

    // TODO: Calculate the width/height from the source image and remove
    // the 37's.
    //const unsigned int TileWidth            = 37;
    //const unsigned int TileHeight           = 37;
    const unsigned int TilesPerRow          = 16;
    const unsigned int TilesPerCol          = 16;
    const unsigned int DirtroadStartRow     = 8;
}

///----------------------------------------------------------------------------
/// LanguageConstants - Used by the translation system of the editor.
///----------------------------------------------------------------------------

namespace LanguageConstants {
    const unsigned int FileMenuItem         = 100;
    const unsigned int ExitMenuItem         = 101;
}

// TODO: Below this line are Adventure Gamer Constants, not editor ones, so
// split them into their own file.

///----------------------------------------------------------------------------
/// AttributeTypes - Defines the 5 attributes used by the game
///----------------------------------------------------------------------------

#define ATTRIBUTETYPES(name, value) static AttributeTypes name() { return AttributeTypes(value); }
                                    
class AttributeTypes {
    
    public:

        // The game refers to Energy as Stamina in some cases.
        ATTRIBUTETYPES(Energy, 0); 
        ATTRIBUTETYPES(Skill, 1);
        ATTRIBUTETYPES(Willpower, 2);
        ATTRIBUTETYPES(Luck, 3);
        ATTRIBUTETYPES(TorchLife, 4);
        static const unsigned int NumTypes = 5;
        static const unsigned int NumTypesForCharacters = 4; // Characters lack Torch Life

        bool operator==(const AttributeTypes& rhs) const { return this->value == rhs.value; }
        bool operator!=(const AttributeTypes& rhs) const { return !(*this == rhs);}
        const unsigned int& asInt() const { return value; }

    private:
        explicit AttributeTypes(unsigned int inValue) : value(inValue) {}
        unsigned int value;
};

#undef ATTRIBUTETYPES

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

///----------------------------------------------------------------------------
/// HearingTypes - How an object alters the player's hearing, or in the case
/// of a save game, what it was when the game was saved.
///----------------------------------------------------------------------------

namespace HearingTypes {
    enum types {
        NoChange,
        Deaf,
        Normal,
        Ultrasonic
    };
}

///----------------------------------------------------------------------------
/// SightTypes - How an object alters the player's sight, or in the case of a
/// save game, what it was when the game was saved.
///----------------------------------------------------------------------------

namespace SightTypes {
    enum types {
        NoChange,
        Blind,
        Normal,
        Infrared
    };
}

#endif // __EDITOR_CONSTANTS_H__
