#include "gametile.h"
#include "../compat/std_extras_compat.h"
#include "../util/frost.h"

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
            Frost::getLineWindows(mapFile, line);
            name(line);
        }

    //}

}

//=============================================================================
// Accessors
//=============================================================================

///----------------------------------------------------------------------------
/// getDrawInfo - Gets information that is useful when drawing the tile.
/// @return a reference to a draw info structure for the tile
///----------------------------------------------------------------------------

const GameTile::DrawInfo GameTile::getDrawInfo() const {
    DrawInfo drawInfo;
    drawInfo.spriteIndex    = base.drawInfo.spriteIndex;
    drawInfo.spriteModifier = base.drawInfo.spriteModifier;
    drawInfo.dark           = base.drawInfo.dark;
    drawInfo.hasGate        = base.drawInfo.hasGate;
    return drawInfo;
}

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
    return base.drawInfo.spriteModifier;
}

///----------------------------------------------------------------------------
/// getSpriteIndex - Get the tile's sprite. This is the X offset on the
/// sprite sheet.
/// @return an integer indicating the sprite as used by the editor
///----------------------------------------------------------------------------

const uint8_t& GameTile::getSpriteIndex() const {
    return base.drawInfo.spriteIndex;
}

//=============================================================================
// Public Functions
//=============================================================================

///----------------------------------------------------------------------------
/// canEntitiesMoveNorth - Checks if the tile allows entities to move north.
///----------------------------------------------------------------------------

const bool GameTile::canEntitiesMoveNorth() const {
    
    if(base.drawInfo.spriteIndex == RoadTypes::Crossroads ||
       base.drawInfo.spriteIndex == RoadTypes::CornerNE ||
       base.drawInfo.spriteIndex == RoadTypes::CornerNW ||
       base.drawInfo.spriteIndex == RoadTypes::DeadEndSouth ||
       base.drawInfo.spriteIndex == RoadTypes::StraightawayVertical ||
       base.drawInfo.spriteIndex == RoadTypes::ThreewayNSE ||
       base.drawInfo.spriteIndex == RoadTypes::ThreewayNWE ||
       base.drawInfo.spriteIndex == RoadTypes::ThreewayNSW) {
        return true;
    }

    return false;
}

///----------------------------------------------------------------------------
/// canEntitiesMoveSouth - Checks if the tile allows entities to move South.
///----------------------------------------------------------------------------

const bool GameTile::canEntitiesMoveSouth() const {
    
    if(base.drawInfo.spriteIndex == RoadTypes::Crossroads ||
       base.drawInfo.spriteIndex == RoadTypes::CornerSE ||
       base.drawInfo.spriteIndex == RoadTypes::CornerSW ||
       base.drawInfo.spriteIndex == RoadTypes::DeadEndNorth ||
       base.drawInfo.spriteIndex == RoadTypes::StraightawayVertical ||
       base.drawInfo.spriteIndex == RoadTypes::ThreewayNSE ||
       base.drawInfo.spriteIndex == RoadTypes::ThreewaySWE ||
       base.drawInfo.spriteIndex == RoadTypes::ThreewayNSW) {
        return true;
    }

    return false;
}

///----------------------------------------------------------------------------
/// canEntitiesMoveEast - Checks if the tile allows entities to move East.
///----------------------------------------------------------------------------

const bool GameTile::canEntitiesMoveEast() const {
    
    if(base.drawInfo.spriteIndex == RoadTypes::Crossroads ||
       base.drawInfo.spriteIndex == RoadTypes::CornerNE ||
       base.drawInfo.spriteIndex == RoadTypes::CornerSE ||
       base.drawInfo.spriteIndex == RoadTypes::DeadEndWest ||
       base.drawInfo.spriteIndex == RoadTypes::StraightawayHorizontal ||
       base.drawInfo.spriteIndex == RoadTypes::ThreewayNSE ||
       base.drawInfo.spriteIndex == RoadTypes::ThreewaySWE ||
       base.drawInfo.spriteIndex == RoadTypes::ThreewayNWE) {
        return true;
    }

    return false;
}

///----------------------------------------------------------------------------
/// canEntitiesMoveWest - Checks if the tile allows entities to move West.
///----------------------------------------------------------------------------

const bool GameTile::canEntitiesMoveWest() const {
    
    if(base.drawInfo.spriteIndex == RoadTypes::Crossroads ||
       base.drawInfo.spriteIndex == RoadTypes::CornerNW ||
       base.drawInfo.spriteIndex == RoadTypes::CornerSW ||
       base.drawInfo.spriteIndex == RoadTypes::DeadEndEast ||
       base.drawInfo.spriteIndex == RoadTypes::StraightawayHorizontal ||
       base.drawInfo.spriteIndex == RoadTypes::ThreewayNSW ||
       base.drawInfo.spriteIndex == RoadTypes::ThreewayNWE ||
       base.drawInfo.spriteIndex == RoadTypes::ThreewaySWE) {
        return true;
    }

    return false;
}


///----------------------------------------------------------------------------
/// isAnyRoadTile - Checks to see if a tile is a road tile and not grass/dirt
/// @return true if it does, false if it does not
///----------------------------------------------------------------------------

const bool GameTile::isAnyRoadTile() const {
    return base.drawInfo.spriteIndex != RoadTypes::Empty ? true : false;
}

///----------------------------------------------------------------------------
/// isCrossroads - Checks if a tile is a Crossroad tile
/// @return true if it is, false if it is not
///----------------------------------------------------------------------------

const bool GameTile::isCrossroads() const {
    return base.drawInfo.spriteIndex == RoadTypes::Crossroads ? true : false;
}

///----------------------------------------------------------------------------
/// isCorner - Checks if a tile is a corner tile
/// @return true if it is, false if it is not
///----------------------------------------------------------------------------

const bool GameTile::isCorner() const {

    if(base.drawInfo.spriteIndex == RoadTypes::CornerNE || base.drawInfo.spriteIndex == RoadTypes::CornerNW ||
       base.drawInfo.spriteIndex == RoadTypes::CornerSE || base.drawInfo.spriteIndex == RoadTypes::CornerSW) {
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
    return base.drawInfo.dark ? true : false;
}

///----------------------------------------------------------------------------
/// isDeadend - Checks if a tile is a dead end tile.
/// @return true if it is, false if it is not
///----------------------------------------------------------------------------

const bool GameTile::isDeadend() const {

    if(base.drawInfo.spriteIndex == RoadTypes::DeadEndNorth || base.drawInfo.spriteIndex == RoadTypes::DeadEndEast ||
       base.drawInfo.spriteIndex == RoadTypes::DeadEndSouth || base.drawInfo.spriteIndex == RoadTypes::DeadEndWest) {
          return true;
    }

    return false;
}

///----------------------------------------------------------------------------
/// isDirtRoad - Checks if a tile has the dirt sprite flag set
/// @return true if it does, false if it does not
///----------------------------------------------------------------------------

const bool GameTile::isDirtRoad() const {
    return base.drawInfo.spriteModifier & TileModifiers::DirtRoad ? true : false;
}

///----------------------------------------------------------------------------
/// isStraightaway - Checks if a tile is a straightaway tile
/// @return true if it is, false if it is not
///----------------------------------------------------------------------------

const bool GameTile::isStraightaway() const {

    if(base.drawInfo.spriteIndex == RoadTypes::StraightawayHorizontal ||
       base.drawInfo.spriteIndex == RoadTypes::StraightawayVertical) {
          return true;
    }

    return false;

}

///----------------------------------------------------------------------------
/// isHorizontalStraightaway - Checks if a straightway tile is a horizontal
/// @return true if it is, false if it is not
///----------------------------------------------------------------------------

const bool GameTile::isHorizontalStraightaway() const {
    return base.drawInfo.spriteIndex == RoadTypes::StraightawayHorizontal ? true : false;
}

///----------------------------------------------------------------------------
/// isVerticalStraightaway - Checks if a straightway tile is a vertical
/// @return true if it is, false if it is not
///----------------------------------------------------------------------------

const bool GameTile::isVerticalStraightaway() const {
    return base.drawInfo.spriteIndex == RoadTypes::StraightawayVertical ? true : false;
}

///----------------------------------------------------------------------------
/// hasAnyFeature - Checks if a tile has any features applied to it.
/// @return true if it does, false if it does not.
///----------------------------------------------------------------------------

const bool GameTile::hasAnyFeature() const {
    
    // If the index is not Grass/Dirt, and there are no modifiers (DirtRoad not
    // included) on the tile, then it is featureless, so return the inverse of
    // this.

    return !(base.drawInfo.spriteIndex != 0 &&
             base.drawInfo.spriteModifier != TileModifiers::None && 
             base.drawInfo.spriteModifier != TileModifiers::DirtRoad);
}

///----------------------------------------------------------------------------
/// hasConnectionFeature - Checks if a tile has a feature that effects more
/// than one tile. IE: Switches.
///----------------------------------------------------------------------------

const bool GameTile::hasConnectionFeature() const {

    // Dark spaces require a switch, so they're included here too.

    return(hasGate() || hasSwitch() || isDark());
}

///----------------------------------------------------------------------------
/// hasGate - Checks if the tile is straightaway, and has a gate
/// @return true if it is a straightaway and has a gate, false otherwise
///----------------------------------------------------------------------------

const bool GameTile::hasGate() const {
    return (isStraightaway() && base.drawInfo.hasGate);
}

///----------------------------------------------------------------------------
/// hasJumpPad - Checks if the tile is corner, and has a jumppad
/// @return true if it is a corner and has a jumppad, false otherwise
///----------------------------------------------------------------------------

const bool GameTile::hasJumpPad() const {
    return (isDeadend() && base.drawInfo.spriteModifier & TileModifiers::JumpPad);    
}

///----------------------------------------------------------------------------
/// hasOnSwitch - Checks if the tile is a corner and has an on switch
/// @return true if it is a corner and has an on switch, false otherwise.
///----------------------------------------------------------------------------

const bool GameTile::hasOnSwitch() const {
    const bool isSwitch = (base.drawInfo.spriteModifier & TileModifiers::SwitchOn) ? true : false;
    return (isCorner() && isSwitch);
}

///----------------------------------------------------------------------------
/// hasSwitch - Checks if the tile is a corner and has a switch
/// @return true if it is a corner and has a switch, false otherwise.
///----------------------------------------------------------------------------

const bool GameTile::hasSwitch() const {

    const bool isSwitch = (base.drawInfo.spriteModifier & TileModifiers::SwitchOn) ||
                          (base.drawInfo.spriteModifier & TileModifiers::SwitchOff);

    return (isCorner() && isSwitch);
}

///----------------------------------------------------------------------------
/// write - writes the tile to the output file given
/// @param mapFile an ofstream of the map file to be written to
///----------------------------------------------------------------------------

const void GameTile::write(std::ofstream& mapFile) const {

    Frost::writeVBInteger(mapFile, base.sprite);
    Frost::writeVBInteger(mapFile, base.flags);
    if (base.sprite != 0) {
        Frost::writeVBLine(mapFile, base.name);
    }
}