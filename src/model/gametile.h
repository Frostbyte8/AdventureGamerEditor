#ifndef __GAMETILE_H__
#define __GAMETILE_H__

#include <string>
#include <stdexcept>
#include <fstream>
#include "../compat/stdint_compat.h"

//-----------------------------------------------------------------------------
// RoadTypes - What each of the 16 values means
//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------
// TileFlags - Effects How the tile is rendered/options it has
//-----------------------------------------------------------------------------

namespace TileFlags {

    const uint8_t None          = 0;
    const uint8_t Unknown       = 1 << 0;
    const uint8_t MoreInfo      = 1 << 1;   // Space has a description Set
    const uint8_t Dark          = 1 << 2;

}

//-----------------------------------------------------------------------------
// TileModifiers - Effects what sprite is drawn for the tile
//-----------------------------------------------------------------------------

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

    public:

        struct DrawInfo {
            uint8_t spriteIndex;
            uint8_t spriteModifier;
            uint8_t dark;
        };

    private:

    //-------------------------------------------------------------------------
    // Base - Contains the data shared by the builder and our game tile class.
    //-------------------------------------------------------------------------

        struct Base {

            public:
                uint8_t         sprite; // The value is Index + Modifier, this is the raw value saved to file.
                uint8_t         flags;
                std::string     name;
                std::string     description;

                // Cached Information
                DrawInfo        drawInfo;
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

                    base.drawInfo.spriteIndex    = 0;
                    base.drawInfo.spriteModifier = 0;
                    base.drawInfo.dark = 0;
                }

                Builder(const GameTile& gameTile) {

                    base.name                    = gameTile.base.name;
                    base.flags                   = gameTile.base.flags;
                    base.description             = gameTile.base.description;
                    base.sprite                  = gameTile.base.sprite;
                    base.drawInfo.spriteIndex    = gameTile.base.drawInfo.spriteIndex;
                    base.drawInfo.spriteModifier = gameTile.base.drawInfo.spriteModifier;
                    base.drawInfo.dark           = gameTile.base.drawInfo.dark;

                }

                Builder& sprite(const uint8_t& sprite) {

                    base.sprite         = sprite;

                    // Update Cache Info too
                    base.drawInfo.spriteIndex    = sprite & 15;
                    base.drawInfo.spriteModifier = (sprite & 240) >> 4;

                    // And if the tile is empty, make sure to clear it's
                    // description

                    if (base.drawInfo.spriteIndex == RoadTypes::Empty &&
                        !(base.drawInfo.spriteModifier & TileModifiers::DirtRoad)) {
                        *this = description("");
                        *this = name("");
                    }

                    return *this;

                }

                Builder& clearModifers() {

                    // The (& 8) << 4 will keep the dirt road set.

                    *this = sprite(base.drawInfo.spriteIndex +
                                   ((base.drawInfo.spriteModifier & 8) << 4));

                    return *this;
                }

                Builder& flags(const uint8_t& flags) {
                    base.flags = flags;
                    base.drawInfo.dark = base.flags & TileFlags::Dark;
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
                    else if(base.flags & TileFlags::MoreInfo) {
                        base.flags &= ~TileFlags::MoreInfo;
                    }
                    return *this;
                }

                static const uint8_t calculateSprite(const uint8_t& index, const uint8_t& modifer) {
                    return index + (modifer << 4);
                }

                void readTile(std::ifstream& mapFile, const std::string& tileDescription);

                bool isModiferValid() const {

                    const uint8_t modifier = (base.drawInfo.spriteModifier & TileModifiers::ALLMODS);
                    
                    if (modifier != 0) {

                        switch (base.drawInfo.spriteIndex) {

                            case RoadTypes::Empty:
                            case RoadTypes::ThreewayNSE:
                            case RoadTypes::ThreewayNSW:
                            case RoadTypes::ThreewayNWE:
                            case RoadTypes::ThreewaySWE:
                                if (modifier != 0) {
                                    return false;
                                }
                                break;

                            case RoadTypes::DeadEndNorth:
                            case RoadTypes::DeadEndEast:
                            case RoadTypes::DeadEndSouth:
                            case RoadTypes::DeadEndWest:
                                if (modifier != TileModifiers::JumpPad) {
                                    return false;
                                }
                                break;

                            case RoadTypes::CornerNE:
                            case RoadTypes::CornerNW:
                            case RoadTypes::CornerSE:
                            case RoadTypes::CornerSW:
                                if (modifier != TileModifiers::SwitchOn &&
                                    modifier != TileModifiers::SwitchOff) {
                                    return false;
                                }
                                break;

                            case RoadTypes::StraightawayVertical:
                            case RoadTypes::StraightawayHorizontal:
                                if (modifier > TileModifiers::ALLMODS) {
                                    return false;
                                }
                                break;

                            case RoadTypes::Crossroads:
                                if (modifier != TileModifiers::SafeHaven &&
                                    modifier != TileModifiers::Hazard) {
                                    return false;
                                }
                                break;
                        }
                    }

                    return true;
                }

                GameTile build() {

                    if (!isModiferValid()) {
                        throw std::invalid_argument("Attempted to build a tile with an invalid modifier.");
                    }

                    if(base.flags & TileFlags::MoreInfo && base.description.empty()) {
                        throw std::invalid_argument("Attempted to build a tile that indicates it has a description, but no description was set.");
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

        // Accessors

        const std::string&      getDescription() const;
        const uint8_t&          getSpriteIndex()  const;
        const uint8_t&          getSpriteModifier() const;
        const uint8_t&          getFlags() const;
        const std::string&      getName() const;  
        const DrawInfo          getDrawInfo() const;

        // Information Functions
        
        const bool hasConnectionFeature() const;
        const bool hasAnyFeature() const;
        const bool hasJumpPad() const;
        const bool hasGate() const;
        const bool hasSwitch() const;
        
        const bool isCorner() const;
        const bool isDark() const;
        const bool isDeadend() const;
        const bool isDirtRoad() const;
        const bool isAnyRoadTile() const;
        const bool isStraightaway() const;

        // IO Functions

        const void write(std::ofstream& mapFile) const;

    private:

        GameTile(Builder& builder) {
            base.name                    = builder.base.name;
            base.flags                   = builder.base.flags;
            base.description             = builder.base.description;
            base.sprite                  = builder.base.sprite;
            base.drawInfo.spriteIndex    = builder.base.drawInfo.spriteIndex;
            base.drawInfo.spriteModifier = builder.base.drawInfo.spriteModifier;
            base.drawInfo.dark           = builder.base.drawInfo.dark;
        }

        Base base;

};

#endif // __GAMETILE_H__
