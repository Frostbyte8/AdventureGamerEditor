#include "gametile.h"

//=============================================================================
// Accessors
//=============================================================================

//-----------------------------------------------------------------------------
// getDescription - Gets the long description of the tile
// @return a string containing the description
//-----------------------------------------------------------------------------

const std::string& GameTile::getDescription() const {
    return base.description;
}

//-----------------------------------------------------------------------------
// getSpriteModifier - Get which modifier is set on the tile. This is the
// Y offset on the sprite sheet.
// @return an integer indicating the modifier applied to the tile.
//-----------------------------------------------------------------------------

const uint8_t& GameTile::getSpriteModifier() const {
    return base.spriteModifier;
}

//-----------------------------------------------------------------------------
// getSpriteIndex - Get the tile's sprite. This is the X offset on the
// sprite sheet.
// @return an integer indicating the sprite as used by the editor
//-----------------------------------------------------------------------------

const uint8_t& GameTile::getSpriteIndex() const {
    return base.spriteIndex;
}


//-----------------------------------------------------------------------------
// getFlags - Get the flags set on the tile.
// @return an integer indicating the flags applied to the tile.
//-----------------------------------------------------------------------------

const uint8_t& GameTile::getFlags() const {
    return base.flags;
}

//=============================================================================
// Public Functions
//=============================================================================

//-----------------------------------------------------------------------------
// hasAnyFeature - Checks is a tile has any features applied to it.
// @return true if it does, false if it does not.
//-----------------------------------------------------------------------------

inline bool GameTile::hasAnyFeature() const {
    
    // If the index is not Grass/Dirt, and there are no modifiers (DirtRoad not
    // included) on the tile, then it is featureless, so return the inverse of
    // this.

    return !(base.spriteIndex != 0 &&
             base.spriteModifier != TileModifiers::None && 
             base.spriteModifier != TileModifiers::DirtRoad);
}
