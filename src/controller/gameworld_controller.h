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

        const std::vector<GameTile>& getTiles() const { return gameMap->getTiles(); }
        const std::vector<GameObject>& getGameObjects() const { return gameMap->getGameObjects(); }
        const std::vector<GameTile::DrawInfo> getTileDrawData() { return gameMap->getTileDrawData(); }

        const int& getMapWidth() const {return gameMap->getWidth(); }
        const int& getMapHeight() const {return gameMap->getHeight(); }
        
    private:

        GameWorldController() {};

        MainWindowInterface*            mainWindow;

        // Game World is composed of several other objects which I have
        // split up here to make doing undo/redo operations easier.

        GameInfo                        gameInfo;
        GameMap*                        gameMap;

};

#endif // __GAMEWORLD_CONTROLLER_H__
