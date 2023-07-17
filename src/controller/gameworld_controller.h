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

        bool newWorld();
        bool loadWorld(const std::string& filePath, const std::string& fileName);
        bool saveWorld(const std::string& filePath, const std::string& fileName);

        bool canAddObject() const;
        bool canAddCharacter() const;

        const GameMap* getGameMap() const { return gameMap; }
        const int& getMapWidth() const {return gameMap->getWidth(); }
        const int& getMapHeight() const {return gameMap->getHeight(); }

        bool tryAddCharacter(GameCharacter::Builder& characterBuilder);
        bool tryAddObject(GameObject::Builder& objectBuilder);

        // TODO: These param names don't match the ones in the .cpp file
        bool tryReplaceObject(GameObject::Builder& objectBuilder);
        bool tryReplaceCharacter(GameCharacter::Builder& characterBuilder);

        bool tryRemoveCharacter(const int& charID);
        bool tryRemoveObject(const int& charID);

        bool tryPlaceObjectAtTile(const int& row, const int& col, const int& objectID);
        bool tryPlaceCharacterAtTile(const int& row, const int& col, const int& charID);

        bool tryGetTileCopy(const int& row, const int& col, GameTile& outTile) const;
        bool getCharacterFromID(const int& ID, GameCharacter* outChar);

        bool tryUpdateTileDescription(const int& row, const int& col, const std::string& inName, const std::string& inDescription);
        bool tryUpdateStoryAndSummary(const std::string& inStory, const std::string& inSummary);
        
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
