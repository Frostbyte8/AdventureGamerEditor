#ifndef __GAMEOBJECT_H_
#define __GAMEOBJECT_H_

#include <string>
#include <fstream>
#include "../compat/stdint_compat.h"
#include "../editor_constants.h"

//-----------------------------------------------------------------------------
// GamerObjectConstants - Exactly what it says
//-----------------------------------------------------------------------------

namespace GamerObjectConstants {
    const int NoID              = -1;

    const int NotOnCreature     = 0;
    const int MaxMonetaryValue  = 10000;
    const int MinMonetaryValue  = 0;
    const int MaxNumUses        = 1;
    const int MinNumUses        = 10000;

    // The original game has no apparent limit, so we'll make a
    // sensible one up.

    const int MaxNameLength         = 128;
    const int MaxDescriptionLength  = 256;

    const int LocatedOnGround       = 0;
    const int LocatedOnPlayer       = 1;
    const int LocatedOnCharacter    = 2;

}

//-----------------------------------------------------------------------------
// GamerObjectDescriptions - Nice names for each of the 6 descriptions
//-----------------------------------------------------------------------------

namespace GamerObjectDescriptions {
    const int Name              = 0;
    const int OnSight           = 1;
    const int OnUse             = 2;
    const int OnLastUse         = 3;
    const int Icon              = 4;
    const int Sound             = 5;
    const int NumDescriptions   = 6;
}

//-----------------------------------------------------------------------------
// GamerObjectFlags1 - First set of flags used by objects
//-----------------------------------------------------------------------------

namespace GamerObjectFlags1 {
    const uint8_t None              = 0x00;
    const uint8_t MasterKey         = 0x01;
    const uint8_t Invisible         = 0x02;
    const uint8_t Ladder            = 0x04;
    const uint8_t Protection        = 0x08;
    const uint8_t Torch             = 0x10;
    const uint8_t Worn              = 0x20;
    const uint8_t FixedLocation     = 0x40;
    const uint8_t Money             = 0x80;
    const uint8_t NumFlags          = 8;
}

//-----------------------------------------------------------------------------
// GamerObjectFlags2 - Second set of flags used by objects
//-----------------------------------------------------------------------------

namespace GamerObjectFlags2 {
    const uint8_t None              = 0x00;
    const uint8_t NotOnGround       = 0x01;
    const uint8_t Key               = 0x02; // If it unlocks a specific door
    const uint8_t Unknown1          = 0x04; // Not sure about these
    const uint8_t Unknown2          = 0x08; // Could be "Used with ID" 
    const uint8_t EffectsTemporary  = 0x10;
    const uint8_t NumFlags          = 6;
}

class GameObject {

    private:    

    //-------------------------------------------------------------------------
    // Base - Contains the data shared by the builder and our GameObject class
    //-------------------------------------------------------------------------

        struct Base {
            int             attributeBase[AdventureGamerConstants::NumAttributes];
            int             attributeRandom[AdventureGamerConstants::NumAttributes];
            int             creatureID;
            std::string     description[6];
            int             doorColumn;
            int             doorRow;
            uint8_t         flags1;
            uint8_t         flags2;
            int             ID;
            int             isLocated;
            std::string     location;
            int             makesSight;
            int             makesHearing;
            int             monetaryWorth;
            int             uses;
            int             usedWithID;
            int             x;
            int             y;

        };

    public:

    //-------------------------------------------------------------------------
    // Builder - Builds a game object
    //-------------------------------------------------------------------------

        class Buildler {
            
            public:
                Builder() {

                    for(int i = 0; i < AdventureGamerConstants::NumAttributes; i++) {
                        attributeBase[i] = 0;
                        attributeRandom[i] = 0;
                    } 

                    for(int i = 0; i < GamerObjectDescriptions::NumDescriptions; i++) {
                        description[i] = "";
                    }

                    creatureID      = 0;
                    doorColumn      = 0;
                    doorRow         = 0;
                    flags1          = GamerObjectFlags1::None;
                    flags2          = GamerObjectFlags2::None;
                    ID              = GamerObjectConstants::NoID;
                    isLocated       = GamerObjectConstants::LocatedOnGround;
                    location        = "0,0";
                    makesSight      = SightTypes::NoChange;
                    makesHearing    = HearingTypes::NoChange;
                    monetaryWorth   = GamerObjectConstants::MinMonetaryValue;
                    uses            = GamerObjectConstants::MinNumUses;
                    usedWithID      = GamerObjectConstants::NoID;
                    x               = 0;
                    y               = 0;

                }

                Builder& attributeBase(const int amount&, const unsigned int& whichAttribute) {

                    if(whichAttribute >= AttributeTypes::NumTypes) {
                        throw std::invalid_argument("Attempted to set an invalid base attribute");
                    }

                    base.attributeBase[whichAttribute] = amount;
                    return *this;
                }

                Builder& attributeRandom(const int amount& const unsigned int& whichAttribute) {
                    if(whichAttribute >= AttributeTypes::NumTypes) {
                        throw std::invalid_argument("Attempted to set an invalid random attribute");
                    }
                    base.attributeRandom[whichAttribute] = amount;
                    return *this;
                }

                Builder& creatureID(const int& creatureID) {
                    base.creatureID = creatureID;
                    return *this;
                }

                Builder& doorColumn(const int& doorColumn) {
                    base.doorColumn = doorColumn;
                    return *this;
                }
                
                Builder& doorRow(const int& doorRow) {
                    base.doorRow = doorRow;
                    return *this;
                }

                Builder& flags1(const int& flags1) {
                    base.flags1 = flags1;
                    return *this
                }

            private:
                Base base;
                friend class GamerObject;
        };

    //-------------------------------------------------------------------------
    // The actual class
    //-------------------------------------------------------------------------

    public:

        GamerObject(Builder& builder) {

            for(int i = 0; i < AdventureGamerConstants::NumAttributes; i++) {
                base.attributeBase[i] = builder.base.attributeBase[i];
                base.attributeRandom[i] = builder.base.attributeRandom[i];
            }

            for(int i = 0; i < GamerObjectConstants::NumDescriptions; i++) {
                base.description[i] = builder.base.description[i];
            }

            base.creatureID     = builder.base.creatureID;
            base.doorColumn     = builder.base.doorColumn;
            base.doorRow        = builder.base.doorRow;
            base.flags1         = builder.base.flags1;
            base.flags2         = builder.base.flags2;
            base.ID             = builder.base.ID;
            base.isLocated      = builder.base.isLocated;
            base.location       = builder.base.location;
            base.makesSight     = builder.base.makesSight;
            base.makesHearing   = builder.base.makesHearing;
            base.monetaryWorth  = builder.base.monetaryWorth;
            base.uses           = builder.base.uses;
            base.usedWithID     = builder.base.usedWithID;
            base.x              = builder.base.x;
            base.y              = builder.base.y;
        }

    private:
        Base base;

};

#endif // __GAMEOBJECT_H_
