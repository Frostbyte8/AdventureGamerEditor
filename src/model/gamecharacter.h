#ifndef __GAMECHARACTER_H__
#define __GAMECHARACTER_H__

#include <string>
#include <fstream>
#include <assert.h>
#include "../compat/stdint_compat.h"
#include "../editor_constants.h"

//-----------------------------------------------------------------------------
// GameCharacterConstants - Exactly what it says
//-----------------------------------------------------------------------------

namespace GameCharacterConstants {
    const int NoID              = -1;
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
            int             unused; // Seems to always be 0. Flags2?

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
                    base.money      = 0;
                    base.sight      = 0;
                    base.type       = 0;
                    base.unused     = 0;
                    base.x          = 0;
                    base.y          = 0;

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

                Builder& location(const std::string& location) {
                    base.location = location;
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
                

            private:

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

        GameCharacter() {} // TODO: Remove this. Used for testing

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