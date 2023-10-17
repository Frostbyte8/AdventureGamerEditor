#ifndef __GAMEWORLD_CONTROLLER_H__
#define __GAMEWORLD_CONTROLLER_H__

#include "../model/gameinfo.h"
#include "../model/gamemap.h"
#include "../model/gameobject.h"
#include "../model/gamecharacter.h"
#include "../interface/mainwindow_interface.h"

// TODO: This really should be named MainWindowController

class GameWorldController {

    public:

        GameWorldController(MainWindowInterface* inMainWindow);
		~GameWorldController();

        bool newWorld();
        bool loadWorld(const std::string& filePath, const std::string& fileName);
        bool saveWorld(bool saveAs = false);

        bool canAddObject() const;
        bool canAddCharacter() const;

        const GameMap* getGameMap() const { return gameMap; }
        const int& getMapWidth() const { return gameMap->getWidth(); }
        const int& getMapHeight() const { return gameMap->getHeight(); }
        const int& getSelectedTileIndex() const { return selectedTileIndex; }
        const int& getSelectedRow() const { return selectedRow; }
        const int& getSelectedCol() const { return selectedCol; }

        bool tryAddCharacter(GameCharacter::Builder& characterBuilder);
        bool tryAddObject(GameObject::Builder& objectBuilder);

        bool tryReplaceObject(GameObject::Builder& objectBuilder);
        bool tryReplaceCharacter(GameCharacter::Builder& characterBuilder);

        bool tryRemoveCharacter(const int& charID);
        bool tryRemoveObject(const int& charID);

        bool tryPlaceObjectAtTile(const int& row, const int& col, const int& objectID);
        bool tryPlaceCharacterAtTile(const int& row, const int& col, const int& charID);

        bool tryGetTileCopy(const int& row, const int& col, GameTile& outTile) const;
        //bool getCharacterFromID(const int& ID, GameCharacter* outChar);
        
        bool tryUpdateGameInfo(const GameInfo& newInfo);

        bool tryUpdateSelectedTile(const int& newIndex);
        bool tryUpdateSelectedTile(const int& newRow, const int& newCol);

        bool tryUpdateTileType(const int& row, const int& col, const int& type);
        bool tryAddFeatureToTile(const int& row, const int& col, const uint8_t& modType);

        bool tryUpdateTileDescription(const int& row, const int& col, const std::string& inName, const std::string& inDescription);
        bool tryUpdateTileDescription(const int& index, const std::string& inName, const std::string& inDescription);
        bool tryUpdateStoryAndSummary(const std::string& inStory, const std::string& inSummary);
        
    private:

        GameWorldController() {};
        
        void showErrorMessage(const std::string& errTextID, const std::string& errTextTitle) const;

        MainWindowInterface*            mainWindow;

        std::string                     worldFilePath;
        std::string                     worldFileName;

        int                             selectedTileIndex;
        int                             selectedRow;
        int                             selectedCol;
        int                             drawingTileIndex;


        // Game World is composed of several other objects which I have
        // split up here to make doing undo/redo operations easier.

        GameInfo                        gameInfo;
        GameMap*                        gameMap;

        GameMap::GMKey                  gmKey;

};

#endif // __GAMEWORLD_CONTROLLER_H__
