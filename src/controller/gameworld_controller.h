#ifndef __GAMEWORLD_CONTROLLER_H__
#define __GAMEWORLD_CONTROLLER_H__

#include "../model/gameinfo.h"
#include "../model/gamemap.h"
#include "../model/gameobject.h"
#include "../model/gamecharacter.h"
#include "../interface/mainwindow_interface.h"

class GameWorldController {

    public:

        GameWorldController(MainWindowInterface* inMainWindow);
		~GameWorldController();

        // TODO: these should be "try" functions to make it clear that they
        // try to do the action, but could fail.
        bool newWorld();
        bool loadWorld(const std::string& filePath, const std::string& fileName);
        bool saveWorld(const std::string& filePath, const std::string& fileName);

        const GameMap* getGameMap() const { return gameMap; }
        const int& getMapWidth() const {return gameMap->getWidth(); }
        const int& getMapHeight() const {return gameMap->getHeight(); }

        bool doesCharacterExist(const int& charID) const;
        bool tryRemoveCharacter(const int& charID);
        bool tryPlaceCharacterAtTile(const int& row, const int& col, const int& charID);
        bool tryGetTileCopy(const int& row, const int& col, GameTile& outTile) const;
        bool getCharacterFromID(const int& ID, GameCharacter* outChar);
        
    private:

        GameWorldController() {};

        MainWindowInterface*            mainWindow;

        // Game World is composed of several other objects which I have
        // split up here to make doing undo/redo operations easier.

        

        GameInfo                        gameInfo;
        GameMap*                        gameMap;

        // TODO: Make this constant?
        GameMap::GMKey                  gmKey;

};

#endif // __GAMEWORLD_CONTROLLER_H__
