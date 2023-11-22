#ifndef __GAMECHARACTER_H__
#define __GAMECHARACTER_H__

#include <string>
#include <fstream>
#include <assert.h>
#include "../compat/std_extras_compat.h"
#include "../compat/stdint_compat.h"
#include "../editor_constants.h"
#include "../adventuregamer_constants.h"

//-----------------------------------------------------------------------------
// GameCharacterConstants - Exactly what it says
//-----------------------------------------------------------------------------

namespace GameCharacterConstants {
    const int NoID              = -1;

    const unsigned int MaxCharacterName         = 128;
    const unsigned int MaxDescriptionLength     = 1024;
    const unsigned int MaxFightText             = 1024;
    const unsigned int MaxDeathText             = 1024;

}

//-----------------------------------------------------------------------------
// GameCharacterDescriptions - Nice names for each of the 6 descriptions
//-----------------------------------------------------------------------------

namespace GameCharacterDescriptions {
    const int Name                  = 0;
    const int OnSight               = 1;
    const int OnFight               = 2;
    const int OnDeath               = 3;
    const int Icon                  = 4;
    const int Sound                 = 5;
    const int NumDescriptions       = 4;
    const int NumFileNames          = 2;
    const int NumAllDescriptions    = 6;
}

//-----------------------------------------------------------------------------
// GameCharacterFlags - Flags that alter how the Character behaves
//-----------------------------------------------------------------------------

namespace GameCharacterFlags {
    const uint8_t None              = 0x00;
    const uint8_t CanEnterDark      = 0x01;
    const uint8_t CanEnterHazard    = 0x02;
    const uint8_t CanUseJumps       = 0x04;
    const uint8_t BypassGates       = 0x08;
    const uint8_t BypassDoors       = 0x10;
    const uint8_t CanClimb          = 0x20;
    const uint8_t Wanderer          = 0x40;
    const uint8_t FollowPlayer      = 0x80;
    const uint8_t NumFlags          = 8;
}

#ifdef _DEBUG
// There is an unused field in the character data. As far
// as I can tell, it is unused, but incase it is, this here
// is to display a message box as to which one has it set.
#include <windows.h>
#endif

class GameCharacter {

    private:    

    //-------------------------------------------------------------------------
    // Base - Contains the data shared by the builder and our GameCharacter
    // class
    //-------------------------------------------------------------------------

        struct Base {
            int             attribute[AttributeTypes::NumTypesForCharacters];
            std::string     description[GameCharacterDescriptions::NumAllDescriptions];
            uint8_t         flags;
            int             ID;
            std::string     location;
            int             money;
            int             sight;
            int             type;
            int             unused; // Seems to always be 0. Flags2? Possible that save games use this field.

            // Cached information used by the editor
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

                    for(int i = 0; i < AttributeTypes::NumTypesForCharacters; i++) {
                        base.attribute[i] = 0;
                    }

                    for(int i = 0; i < GameCharacterDescriptions::NumAllDescriptions; i++) {
                        base.description[i] = "";
                    }

                    base.flags      = 0;
                    base.ID         = GameCharacterConstants::NoID;
                    base.location   = "0,0";
                    base.money      = AdventureGamerConstants::MinAmountOfMoney;
                    base.sight      = 2; // Normal
                    base.type       = 2; // Trader
                    base.unused     = 0;
                    base.x          = 0;
                    base.y          = 0;

                }

                Builder(const GameCharacter& gc) {

                    for(int i = 0; i < AttributeTypes::NumTypesForCharacters ; i++) {
                        base.attribute[i] = gc.base.attribute[i];
                    }

                    for(int i = 0; i < GameCharacterDescriptions::NumAllDescriptions; i++) {
                        base.description[i] = gc.base.description[i];
                    }

                    base.flags      = gc.base.flags;
                    base.ID         = gc.base.ID;
                    base.location   = gc.base.location;
                    base.money      = gc.base.money;
                    base.sight      = gc.base.sight;
                    base.type       = gc.base.type;
                    base.unused     = gc.base.unused;
                    base.x          = gc.base.x;
                    base.y          = gc.base.y;
                }

                Builder& attribute(const int& amount, const AttributeTypes& type) {
                    assert(type != AttributeTypes::TorchLife());
                    base.attribute[type.asInt()] = amount;
                    return *this;
                }

                Builder& description(const std::string& description, const unsigned int& which) {
                    base.description[which] = description;
                    return *this;
                }

                Builder& flags(const uint8_t& flags) {
                    base.flags = flags;
                    return *this;
                }

                Builder& ID(const int& ID) {
                    base.ID = ID;
                    return *this;
                }

                Builder& location(const int& x, const int& y) {

                    base.location = std::to_string(x) + "," + std::to_string(y);

                    // Cache location for the editor
                    base.x = x;
                    base.y = y;

                    return *this;
                }

                Builder& money(const int& money) {
                    base.money = money;
                    return *this;
                }

                Builder& sight(const SightTypes& sight) {
                    base.sight = sight.asInt();
                    return *this;
                }

                Builder& type(const int& type) {
                    base.type = type;
                    return *this;
                }

                Builder& unused(const int& unused) {

                    #ifdef _DEBUG
                        if(unused != 0) {
                            std::string info = "Character " + base.description[0] + " has the unused value set!";
                            MessageBoxA(NULL, info.c_str(), "Unused value found!", MB_OK | MB_ICONEXCLAMATION);
                        }
                    #endif

                    base.unused = unused;
                    return *this;
                }

                void readCharacter(std::ifstream& mapFile);

                GameCharacter build() {
                    // TOOD: Any additional error checking that must occur, we may also
                    return (*this);
                }

                const int& getID() const {
                    return base.ID;
                }
                

            //private:

                // These ones should only be accessed via the read function

                Builder& attribute(const int& amount, const unsigned int& type) {
                    assert(type < AttributeTypes::TorchLife().asInt());
                    base.attribute[type] = amount;
                    return *this;
                }

                Builder& sight(const int& sight) {
                    assert(sight < SightTypes::NumTypes);
                    base.sight = sight;
                    return *this;
                }

                Base base;
                friend class GameCharacter;
        };

    //-------------------------------------------------------------------------
    // The actual class
    //-------------------------------------------------------------------------

    public:

        const std::string& getDescription(const int& which) const { return base.description[which]; }
        const uint8_t& getFlags() const { return base.flags; }
        const int& getType() const { return base.type; }
        const int& getID() const { return base.ID; }
        const int& getMoney() const { return base.money; }
        const int& getAttribute(const int& which) const { return base.attribute[which]; }
        const int& getSight() const { return base.sight; }
        const int& getX() const { return base.x; }
        const int& getY() const { return base.y; }

        const std::string& getName() const { return base.description[0]; }

        void writeCharacter(std::ofstream& mapFile) const;

    private:

        GameCharacter(Builder& builder) {

            for(int i = 0; i < AttributeTypes::NumTypesForCharacters ; i++) {
                base.attribute[i] = builder.base.attribute[i];
            }

            for(int i = 0; i < GameCharacterDescriptions::NumAllDescriptions; i++) {
                base.description[i] = builder.base.description[i];
            }

            base.flags      = builder.base.flags;
            base.ID         = builder.base.ID;
            base.location   = builder.base.location;
            base.money      = builder.base.money;
            base.sight      = builder.base.sight;
            base.type       = builder.base.type;
            base.unused     = builder.base.unused;
            base.x          = builder.base.x;
            base.y          = builder.base.y;

        }
    
        Base base;

};

#endif // __GAMECHARACTER_H__
