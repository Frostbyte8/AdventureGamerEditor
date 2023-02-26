#ifndef __GAMETILE_H__
#define __GAMETILE_H__

#include <string>
#include <stdexcept>
#include "../compat/stdint_compat.h"

//----------------------------------------------------------------------
// RoadTypes - What each of the 16 values means
//----------------------------------------------------------------------

namespace RoadTypes {

    const uint8_t Empty                     = 0;
    const uint8_t DeadEndSouth              = 1;
    const uint8_t DeadEndWest               = 2;
    const uint8_t CornerNE                  = 3;
    const uint8_t DeadEndNorth              = 4;
    const uint8_t StraightawayVertical      = 5;
    const uint8_t CornerSE                  = 6;
    const uint8_t ThreewayNSE               = 7;
    const uint8_t DeadEndEast               = 8;
    const uint8_t CornerNW                  = 9;
    const uint8_t StraightawayHorizontal    = 10;
    const uint8_t ThreewayNWE               = 11;
    const uint8_t CornerSW                  = 12;
    const uint8_t ThreewayNSW               = 13;
    const uint8_t ThreewaySWE               = 14;
    const uint8_t Crossroads                = 15;

};

//----------------------------------------------------------------------
// TileFlags - Effects How the tile is rendered/options it has
//----------------------------------------------------------------------

namespace TileFlags {

    const uint8_t None          = 0;
    const uint8_t Unknown       = 1 << 0;
    const uint8_t MoreInfo      = 1 << 1;   // Space has a description Set
    const uint8_t Dark          = 1 << 2;

}

//----------------------------------------------------------------------
// TileModifiers - Effects what sprite is drawn for the tile
//----------------------------------------------------------------------

namespace TileModifiers {

    const uint8_t None          = 0;

    const uint8_t MOD1          = 0x01;
    const uint8_t MOD2          = 0x02;
    const uint8_t MOD3          = 0x04;
    const uint8_t ALLMODS       = MOD1 | MOD2 | MOD3;

    // Nice names for each MOD.
    
    // Corner Effects
    const uint8_t SwitchOn      = MOD1;
    const uint8_t SwitchOff     = MOD2;

    // Straightaway Effects
    const uint8_t Start         = 0x01;
    const uint8_t Finish        = 0x02;
    const uint8_t GateClosed    = 0x03;
    const uint8_t LockedDoor    = 0x04;
    const uint8_t GateOpen      = 0x07;

    // Horizontal Straightaway
    const uint8_t BarrierEast   = 0x05;
    const uint8_t BarrierWest   = 0x06;

    // Vertical Straightaway
    const uint8_t BarrierSouth  = 0x05;
    const uint8_t BarrierNorth  = 0x06;

    // Dead-End Effects
    const uint8_t JumpPad       = 0x03;

    // Crossroads
    const uint8_t Hazard        = 0x04;
    const uint8_t SafeHaven     = 0x07;

    // Use Dirt Road Tileset
    const uint8_t DirtRoad      = 0x08;

}

class GameTile {

    private:

    //-------------------------------------------------------------------------
    // Base - Contains the data shared by the builder and our object.
    //-------------------------------------------------------------------------

        struct Base
        {
            public:
                uint8_t         sprite; // The value is Index + Modifier, this is the raw value saved to file.
                uint8_t         flags;
                std::string     name;
                std::string     description;

                // Cached Information
                uint8_t         spriteIndex;      // Sprite Sheet X Offset
                uint8_t         spriteModifier;   // Sprite Sheet Y Offset          
        };

    public:

    //-------------------------------------------------------------------------
    // Builder - Builds Gametiles.
    //-------------------------------------------------------------------------

        class Builder {

            public:

                Builder() {

                    base.sprite         = 0;
                    base.flags          = 0;
                    base.name           = "";
                    base.description    = "";
                    base.spriteIndex    = 0;
                    base.spriteModifier = 0;

                }

                Builder& sprite(const uint8_t& sprite) {

                    base.sprite         = sprite;
                    base.spriteIndex    = sprite & 15;
                    base.spriteModifier = (sprite & 240) >> 4;
                    return *this;

                }

                Builder& flags(const uint8_t& flags) {
                    base.flags = flags;
                    return *this;
                }

                Builder& name(const std::string& name) {
                    base.name = name;
                    return *this;
                }

                Builder& description(const std::string& description) {
                    base.description = description;
                    if(!description.empty()) {
                        base.flags |= TileFlags::MoreInfo;
                    }
                    return *this;
                }

                GameTile build() {

                    if(base.spriteModifier != 0) {

                        bool invalidModifer = false;
                        switch(base.spriteIndex) {

                            case RoadTypes::Empty:
                                if(base.spriteModifier != 0 && base.spriteModifier != TileModifiers::DirtRoad) {
                                    invalidModifer = true;
                                }
                                break;

                            case RoadTypes::DeadEndNorth:
                            case RoadTypes::DeadEndEast:
                            case RoadTypes::DeadEndSouth:
                            case RoadTypes::DeadEndWest:
                                if(base.spriteModifier != TileModifiers::JumpPad) {
                                    invalidModifer = true; 
                                }
                                break;

                            case RoadTypes::CornerNE:
                            case RoadTypes::CornerNW:
                            case RoadTypes::CornerSE:
                            case RoadTypes::CornerSW:
                                if(base.spriteModifier != TileModifiers::SwitchOn ||
                                   base.spriteModifier != TileModifiers::SwitchOff) {
                                   invalidModifer = true;
                                }
                                break;

                            case RoadTypes::StraightawayVertical:
                            case RoadTypes::StraightawayHorizontal:
                                if(base.spriteModifier > TileModifiers::ALLMODS) {
                                    invalidModifer = true;
                                }
                                break;
                            case RoadTypes::Crossroads:
                                if(base.spriteModifier != TileModifiers::SafeHaven ||
                                   base.spriteModifier != TileModifiers::Hazard) {
                                    invalidModifer = true;
                                }
                                break;
                        }
                        if(invalidModifer) {
                            throw std::invalid_argument("Attempted to build a tile with an invalid modifier.");
                        }
                    }

                    return GameTile(*this);
                }

            private:

                Base base;
                friend class GameTile;
        };

    //-------------------------------------------------------------------------
    // The actual class
    //-------------------------------------------------------------------------

    public:

        GameTile(Builder& builder) {
            base.name           = builder.base.name;
            base.description    = builder.base.description;
            base.sprite         = builder.base.sprite;
            base.spriteIndex    = builder.base.spriteIndex;
            base.spriteModifier = builder.base.spriteModifier;
        }

        // Accessors

        const std::string&      getDescription() const;
        const uint8_t&          getSpriteIndex()  const;
        const uint8_t&          getSpriteModifier() const;
        const uint8_t&          getFlags() const;
        const std::string&      getName() const;  

        // Public Function
        
        inline bool             hasAnyFeature() const;

    private:
        Base base;

};

#endif // __GAMETILE_H__
