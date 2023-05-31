#ifndef __EDITOR_CONSTANTS_H__
#define __EDITOR_CONSTANTS_H__

#include <string>
#include "compat/stdint_compat.h"

///----------------------------------------------------------------------------
/// EditorConstants - Useful constats that are used by the editor
///----------------------------------------------------------------------------

namespace EditorConstants {
    unsigned int const TilesPerRow          = 16;
    unsigned int const TilesPerCol          = 16;
    unsigned int const DirtroadStartRow     = 8;

	const unsigned int DefaultCols			= 10;
	const unsigned int DefaultRows			= 10;
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

///----------------------------------------------------------------------------
/// AlterTypes - Add / Edit / Place / Delete an Object
///----------------------------------------------------------------------------

namespace AlterType {
    const unsigned int Add      = 0;
    const unsigned int Edit     = 1;
    const unsigned int Place    = 2;
    const unsigned int Delete   = 3;
}

#endif // __EDITOR_CONSTANTS_H__
