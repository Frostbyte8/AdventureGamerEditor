#ifndef __EDITOR_CONSTANTS_H__
#define __EDITOR_CONSTANTS_H__

#include <string>
#include "compat/stdint_compat.h"

///----------------------------------------------------------------------------
/// EditorConstants - Useful constats that are used by the editor
///----------------------------------------------------------------------------

namespace EditorConstants {
    const unsigned int TilesPerRow          = 16;
    const unsigned int TilesPerCol          = 16;
    const unsigned int DirtroadStartRow     = 8;
}

///----------------------------------------------------------------------------
/// LanguageConstants - Used by the translation system of the editor.
///----------------------------------------------------------------------------

namespace LanguageConstants {
    enum {
        FileMenuItem = 100,
        ExitMenuItem
    };
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
/// AdventureGamerConstants - Hard limits of the game itself
///----------------------------------------------------------------------------

namespace AdventureGamerConstants {

    // Rows definately cannot exceed 100, but columns? I think it may be
    // dependant on memory, which given that it was written in Visual Basic 3,
    // is probably not much.
    const unsigned int MaxCols              = 100;
    const unsigned int MaxRows              = 100; // That's 0 to 99.
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

///----------------------------------------------------------------------------
/// HearingTypes - How an object alters the player's hearing, or in the case
/// of a save game, what it was when the game was saved.
///----------------------------------------------------------------------------

#define HEARINGTYPES(name, value) static HearingTypes name() { return HearingTypes(value); }
                                    
class HearingTypes {
    
    public:
        // The game refers to Energy as Stamina in some cases.
        HEARINGTYPES(NoChange, 0); 
        HEARINGTYPES(Deaf, 1);
        HEARINGTYPES(Normal, 2);
        HEARINGTYPES(Ultrasonic, 3);
        static const unsigned int NumTypes = 4;

        bool operator==(const HearingTypes& rhs) const { return this->value == rhs.value; }
        bool operator!=(const HearingTypes& rhs) const { return !(*this == rhs);}
        const unsigned int& asInt() const { return value; }

    private:
        explicit HearingTypes(unsigned int inValue) : value(inValue) {}
        unsigned int value;
};

#undef HEARINGTYPES

///----------------------------------------------------------------------------
/// SightTypes - How an object alters the player's sight, or in the case of a
/// save game, what it was when the game was saved.
///----------------------------------------------------------------------------

#define SIGHTTYPES(name, value) static SightTypes name() { return SightTypes(value); }
                                    
class SightTypes {
    
    public:
        // The game refers to Energy as Stamina in some cases.
        SIGHTTYPES(NoChange, 0); 
        SIGHTTYPES(Blind, 1);
        SIGHTTYPES(Normal, 2);
        SIGHTTYPES(Infrared, 3);
        static const unsigned int NumTypes = 4;

        bool operator==(const SightTypes& rhs) const { return this->value == rhs.value; }
        bool operator!=(const SightTypes& rhs) const { return !(*this == rhs);}
        const unsigned int& asInt() const { return value; }

    private:
        explicit SightTypes(unsigned int inValue) : value(inValue) {}
        unsigned int value;
};

#undef SIGHTTYPES

#endif // __EDITOR_CONSTANTS_H__
