#include "gametile.h"
#include "../compat/std_extras_compat.h"

//=============================================================================
// GameTile::Builder
//=============================================================================

///----------------------------------------------------------------------------
/// readTile - Reads a single tile from the map file, and it will also give
/// the tile a long description if it's specified.
/// @param mapFile an ifstream of the map file to be read from
/// @param string containing the long description of the tile.
///----------------------------------------------------------------------------

void GameTile::Builder::readTile(std::ifstream& mapFile, const std::string& tileDescription) {
    
    if(!tileDescription.empty()) {
        description(tileDescription);
    }

    //try {

        std::string line;
        std::getline(mapFile, line);
        sprite(std::stoi(line));

        std::getline(mapFile, line);
        flags(std::stoi(line));

        if(base.sprite != 0) {
            std::getline(mapFile, line);
            name(line);
        }

    //}

}

//=============================================================================
// Accessors
//=============================================================================

///----------------------------------------------------------------------------
/// getDescription - Gets the long description of the tile
/// @return a string containing the description
///----------------------------------------------------------------------------

const std::string& GameTile::getDescription() const {
    return base.description;
}

///----------------------------------------------------------------------------
/// getFlags - Gets the flags set on the tile.
/// @return an integer indicating the flags applied to the tile.
///----------------------------------------------------------------------------

const uint8_t& GameTile::getFlags() const {
    return base.flags;
}

///----------------------------------------------------------------------------
/// getName - Gets the name of the tile
/// @return a string containing the name of the tile
///----------------------------------------------------------------------------

const std::string& GameTile::getName() const {
    return base.name;
}

///----------------------------------------------------------------------------
/// getSpriteModifier - Get which modifier is set on the tile. This is the
/// Y offset on the sprite sheet.
/// @return an integer indicating the modifier applied to the tile.
///----------------------------------------------------------------------------

const uint8_t& GameTile::getSpriteModifier() const {
    return base.spriteModifier;
}

///----------------------------------------------------------------------------
/// getSpriteIndex - Get the tile's sprite. This is the X offset on the
/// sprite sheet.
/// @return an integer indicating the sprite as used by the editor
///----------------------------------------------------------------------------

const uint8_t& GameTile::getSpriteIndex() const {
    return base.spriteIndex;
}

//=============================================================================
// Public Functions
//=============================================================================

///----------------------------------------------------------------------------
/// isCorner - Checks if a tile is a corner tile
/// @return true if it is, false if it is not
///----------------------------------------------------------------------------

const bool GameTile::isCorner() const {

    if(base.spriteIndex == RoadTypes::CornerNE || base.spriteIndex == RoadTypes::CornerNW ||
       base.spriteIndex == RoadTypes::CornerSE || base.spriteIndex == RoadTypes::CornerSW) {
          return true;
    }

    return false;
}

///----------------------------------------------------------------------------
/// isDark - Checks if a tile is marked as being dark when a light switch is
/// off.
/// @return true if it is, false if it is not
///----------------------------------------------------------------------------

const bool GameTile::isDark() const {

    return (base.flags & TileFlags::Dark ? true : false);
}

///----------------------------------------------------------------------------
/// isDeadend - Checks if a tile is a dead end tile.
/// @return true if it is, false if it is not
///----------------------------------------------------------------------------

const bool GameTile::isDeadend() const {

    if(base.spriteIndex == RoadTypes::DeadEndNorth || base.spriteIndex == RoadTypes::DeadEndEast ||
       base.spriteIndex == RoadTypes::DeadEndSouth || base.spriteIndex == RoadTypes::DeadEndWest) {
          return true;
    }

    return false;
}

///----------------------------------------------------------------------------
/// isStraightaway - Checks if a tile is a straightaway tile
/// @return true if it is, false if it is not
///----------------------------------------------------------------------------

const bool GameTile::isStraightaway() const {

    if(base.spriteIndex == RoadTypes::StraightawayHorizontal ||
       base.spriteIndex == RoadTypes::StraightawayVertical) {
          return true;
    }

    return false;

}
///----------------------------------------------------------------------------
/// hasAnyFeature - Checks is a tile has any features applied to it.
/// @return true if it does, false if it does not.
///----------------------------------------------------------------------------

const bool GameTile::hasAnyFeature() const {
    
    // If the index is not Grass/Dirt, and there are no modifiers (DirtRoad not
    // included) on the tile, then it is featureless, so return the inverse of
    // this.

    return !(base.spriteIndex != 0 &&
             base.spriteModifier != TileModifiers::None && 
             base.spriteModifier != TileModifiers::DirtRoad);
}

///----------------------------------------------------------------------------
/// hasGate - Checks if the tile is straightaway, and has a gate
/// @return true if it is a straightaway and has a gate, false otherwise
///----------------------------------------------------------------------------

const bool GameTile::hasGate() const {

    const bool isGate = (base.spriteModifier & TileModifiers::GateClosed) ||
                        (base.spriteModifier & TileModifiers::GateOpen);

    return (isStraightaway() && isGate);
}

///----------------------------------------------------------------------------
/// hasJumpPad - Checks if the tile is corner, and has a jumppad
/// @return true if it is a corner and has a jumppad, false otherwise
///----------------------------------------------------------------------------

const bool GameTile::hasJumpPad() const {
    return (isDeadend() && base.spriteModifier & TileModifiers::JumpPad);    
}

///----------------------------------------------------------------------------
/// hasSwitch - Checks if the tile is a corner and has a switch
/// @return true if it is a corner and has a switch, false otherwise.
///----------------------------------------------------------------------------

const bool GameTile::hasSwitch() const {

    const bool isSwitch = (base.spriteModifier & TileModifiers::SwitchOn) ||
                          (base.spriteModifier & TileModifiers::SwitchOff);

    return (isCorner() && isSwitch);
}
