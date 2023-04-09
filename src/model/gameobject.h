#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include <string>
#include <fstream>
#include <assert.h>
#include "../util/enumflags.h"
#include "../compat/stdint_compat.h"
#include "../editor_constants.h"

//-----------------------------------------------------------------------------
// GameObjectConstants - Exactly what it says
//-----------------------------------------------------------------------------

namespace GameObjectConstants {
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
// GameObjectDescriptions - Nice names for each of the 6 descriptions
//-----------------------------------------------------------------------------

namespace GameObjectDescriptions {
    const int Name                  = 0;
    const int OnSight               = 1;
    const int OnUse                 = 2;
    const int OnLastUse             = 3;
    const int Icon                  = 4;
    const int Sound                 = 5;
    const int NumDescriptions       = 4;
    const int NumFileNames          = 2;
    const int NumAllDescriptions    = 6;
}

//-----------------------------------------------------------------------------
// GameObjectFlags1 - First set of flags used by objects
//-----------------------------------------------------------------------------

namespace GameObjectFlags1 {

    enum flags {
        MasterKey         = 0x01,
        Invisible         = 0x02,
        Ladder            = 0x04,
        Protection        = 0x08,
        Torch             = 0x10,
        Worn              = 0x20,
        FixedLocation     = 0x40,
        Money             = 0x80,
        NumFlags          = 8
    };
}

typedef EnumFlags<enum GameObjectFlags1::flags, GameObjectFlags1::NumFlags> GameObjectFlags1_T;

//-----------------------------------------------------------------------------
// GameObjectFlags2 - Second set of flags used by objects
//-----------------------------------------------------------------------------

namespace GameObjectFlags2 {
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
            int             attributeBase[AttributeTypes::NumTypes];
            int             attributeRandom[AttributeTypes::NumTypes];
            std::string     description[6];
            int             doorColumn;
            int             doorRow;
            GameObjectFlags1_T         flags1;
            uint8_t         flags2;
            int             ID;
            std::string     location;
            unsigned int    makesSight;
            unsigned int    makesHearing;
            int             monetaryWorth;
            int             uses;
            int             usedWithID;

            // Cached information used by the editor
            int             creatureID;
            int             isLocated;
            int             x;
            int             y;

        };

    //-------------------------------------------------------------------------
    // Builder - Builds a game object
    //-------------------------------------------------------------------------
    
    public:
        
        class Builder {
            
            public:
                Builder() {

                    for(int i = 0; i < AttributeTypes::NumTypes; i++) {
                        base.attributeBase[i] = 0;
                        base.attributeRandom[i] = 0;
                    } 

                    for(int i = 0; i < AttributeTypes::NumTypes; i++) {
                        base.description[i] = "";
                    }

                    base.creatureID     = 0;
                    base.doorColumn     = 0;
                    base.doorRow        = 0;
                    //base.flags1         = GameObjectFlags1::None;
                    base.flags2         = GameObjectFlags2::None;
                    base.ID             = GameObjectConstants::NoID;
                    base.isLocated      = GameObjectConstants::LocatedOnGround;
                    base.location       = "0,0";
                    base.makesSight     = SightTypes::NoChange().asInt();
                    base.makesHearing   = HearingTypes::NoChange().asInt();
                    base.monetaryWorth  = GameObjectConstants::MinMonetaryValue;
                    base.uses           = GameObjectConstants::MinNumUses;
                    base.usedWithID     = GameObjectConstants::NoID;
                    base.x              = 0;
                    base.y              = 0;

                }

                Builder& attributeBase(const int& amount, const AttributeTypes& type) {
                    base.attributeBase[type.asInt()] = amount;
                    return *this;
                }

                Builder& attributeRandom(const int& amount, const AttributeTypes& type) {
                    base.attributeRandom[type.asInt()] = amount;
                    return *this;
                }

                Builder& creatureID(const int& creatureID) {
                    base.creatureID = creatureID;
                    return *this;
                }

                Builder& description(const std::string& description, const unsigned int& which) {
                    base.description[which] = description;
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

                Builder& flags1(const uint8_t& flags1) {
                    base.flags1.from_uint(flags1);
                    return *this;
                }

                Builder& flags2(const int& flags2) {
                    base.flags2 = flags2;
                    return *this;
                }

                Builder& ID(const int& ID) {
                    base.ID = ID;
                    return *this;
                }

                Builder& isLocated(const int& isLocated) {
                    base.isLocated = isLocated;
                    return *this;
                }

                Builder& location(const std::string& location) {
                    // TODO: Location also sets X, Y, and IsLocated
                    // for editor use.
                    base.location = location;
                    return *this;
                }

                Builder& makesSight(const SightTypes& makesSight) {
                    base.makesSight = makesSight.asInt();
                    return *this;
                }
                
                Builder& makesHearing(const HearingTypes& makesHearing) {
                    base.makesHearing = makesHearing.asInt();
                    return *this;
                }

                Builder& monetaryWorth(const int& monetaryWorth) {
                    base.monetaryWorth = monetaryWorth;
                    return *this;
                }

                Builder& uses(const int& uses) {
                    base.uses = uses;
                    return *this;
                }

                Builder& usedWithID(const int& usedWithID) {
                    base.usedWithID = usedWithID;
                    return *this;
                }

                Builder& x(const int& x) {
                    base.x = x;
                    return *this;
                }

                Builder& y(const int& y) {
                    base.y = y;
                    return *this;
                }

                void readObject(std::ifstream& mapFile);

                GameObject build() {
                    // TOOD: Any additional error checking that must occur, we may also
                    return (*this);
                }
                

            private:

                // These ones should only be accessed via the read function
                Builder& attributeBase(const int& amount, const unsigned int& type) {
                    assert(type < AttributeTypes::NumTypes);
                    base.attributeBase[type] = amount;
                    return *this;
                }

                Builder& attributeRandom(const int& amount, const unsigned int& type) {
                    assert(type < AttributeTypes::NumTypes);
                    base.attributeRandom[type] = amount;
                    return *this;
                }

                Builder& makesSight(const int& makesSight) {
                    assert(makesSight < SightTypes::NumTypes);
                    base.makesSight = makesSight;
                    return *this;
                }

                Builder& makesHearing(const int& makesHearing) {
                    assert(makesHearing < SightTypes::NumTypes);
                    base.makesHearing = makesHearing;
                    return *this;
                }

                Base base;
                friend class GameObject;
        };

    //-------------------------------------------------------------------------
    // The actual class
    //-------------------------------------------------------------------------

    public:

        GameObject() {} // TODO: Remove this. Used for testing

        const std::string& getName() const {
            return base.description[0];
        }

    private:

        GameObject(Builder& builder) {

            for(int i = 0; i < AttributeTypes::NumTypes; i++) {
                base.attributeBase[i] = builder.base.attributeBase[i];
                base.attributeRandom[i] = builder.base.attributeRandom[i];
            }

            for(int i = 0; i < GameObjectDescriptions::NumAllDescriptions; i++) {
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

    
        Base base;

};

#endif // __GAMEOBJECT_H__