#ifndef __GAMEWORLD_CONTROLLER_H__
#define __GAMEWORLD_CONTROLLER_H__

#include "../model/gameinfo.h"
#include "../model/gamemap.h"
#include "../model/gameobject.h"
#include "../model/gamecharacter.h"
#include "../interface/mainwindow_interface.h"

class GameWorldController {

    public:

        GameWorldController(MainWindowInterface* inMainWindow) : mainWindow (inMainWindow) {};
        bool LoadWorld(const std::string& filePath, const std::string& fileName);
        const std::vector<GameTile>& getTiles() const { return gameMap.getTiles(); }
        const std::vector<GameTile::DrawInfo> getTileDrawData() { return gameMap.getTileDrawData(); }

        const int& getMapWidth() const {return gameMap.getWidth(); }
        const int& getMapHeight() const {return gameMap.getHeight(); }
        
    private:

        MainWindowInterface*            mainWindow;

        // Game World is composed of several other objects which I have
        // split up here to make doing undo/redo operations easier.

        GameInfo                        gameInfo;
        GameMap                         gameMap;

        /*
        GameMap*                        gameMap;
        std::string                     gameStory;
        std::deque<GameCharacters*>     gameCharacters;
        std::deque<GameObjects*>        gameObjects;
        */

};

#endif // __GAMEWORLD_CONTROLLER_H__
