#ifndef __ADVENTUREGAMER_CONSTANTS_H__
#define  __ADVENTUREGAMER_CONSTANTS_H__

///----------------------------------------------------------------------------
/// AdventureGamerConstants - Hard limits of the game itself
///----------------------------------------------------------------------------

namespace AdventureGamerConstants {

	const std::string FileNameExtension         = "SG0";
    const unsigned int MaxFileNameLength        = 12;
    const unsigned int MaxCurrencyNameLength    = 64;
    const unsigned int MaxWorldNameLength       = 96;

    const int MinAttributeValue             = 0;
    const int MaxAttributeValue             = 12;
    const int MinAmountOfMoney              = 0;
    const int MaxAmountOfMoney              = 10000;
    
    const int MaxNumObjects                 = 100;
    const int MaxNumCharacters              = 25;
    
    const int MaxObjectMonetaryValue        = 10000;
    const int MinObjectMonetaryValue        = 0;
    const int MaxNumUses                    = 10000;
    const int MinNumUses                    = 1;
    
}


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

#endif //  __ADVENTUREGAMER_CONSTANTS_H__