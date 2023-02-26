#include "gamemap.h"
#include "../compat/std_extras_compat.h"

///----------------------------------------------------------------------------
/// readTile - Reads a single tile from the map file, and it will also give
/// the tile a long description if it's specified.
/// @param mapFile an ifstream of the map file to be read from
/// @param string containing the long description of the tile.
/// @throws runtime_error if any of the numbers read are invalid
/// @throws runtime_error if the builder tries to build an invalid tile.
/// @returns a GameTile object
///----------------------------------------------------------------------------

GameTile GameMap::readTile(std::ifstream& mapFile, const std::string& description) {

    std::string line;
    GameTile::Builder tileBuilder;

    try {

        std::getline(in, line);
        tileBuilder.sprite(std::stoi(line));

        std::getline(in, line);
        tileBuilder.flags(std::stoi(line));

        if(tileBuilder.base.sprite != 0) {
            std::getline(in, line);
            tileBuilder.name(line);
        }

    }
    catch (const std::invalid_argument& e) {
        throw std::runtime_error("Tried to read a sprite number, but did not get a valid integer");
    }

    if(!description.empty()) {
        tileBuilder.description(description);
    }

    try {
        return tileBuilder.build();
    }
    catch (const std::invalid_argument& e) {
        throw std::runtime_error(e.what());
    }
}
