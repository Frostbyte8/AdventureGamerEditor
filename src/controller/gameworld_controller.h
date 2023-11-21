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
        const int& getDrawingTileIndex() const { return drawingTileIndex; }
        const GameTile& getSelectedTile() const { return gameMap->getTile(selectedTileIndex); }

        void setDrawingTileIndex(const int& newDrawIndex) {
            // TODO: Caps
            drawingTileIndex = newDrawIndex;
        }

        
        
        bool tryAlterObject(const int& alterType, const int& index);
        bool tryAddObject(GameObject::Builder& objectBuilder);
        bool tryReplaceObject(GameObject::Builder& objectBuilder, const bool shouldNotify = true);
        bool tryDeleteObject(const int& charID);

        bool tryAlterCharacter(const int& alterType, const int& index);
        bool tryAddCharacter(GameCharacter::Builder& characterBuilder);
        bool tryReplaceCharacter(GameCharacter::Builder& objectBuilder, const bool shouldNotify = true);
        bool tryDeleteCharacter(const int& charID);
        
        void tryUpdateTileDescription(const std::string& inName, const std::string& inDescription);
        
        
        //
       
        bool tryGetTileCopy(const int& row, const int& col, GameTile& outTile) const;
        
        bool tryChangeSelectedTile();
        bool tryMakeTileDark();

        bool tryUpdateGameInfo(const GameInfo& newInfo);
        bool tryUpdateSelectedTile(const int& newIndex);
        bool tryUpdateSelectedTile(const int& newRow, const int& newCol);
        
        bool tryAddFeatureToTile(const int& modType);
        bool tryAddFirstJumpConnection();
        bool tryAddSecondJumpConnection();
        
        bool tryStartSwitchConnection();
        bool tryEndSwitchConnection();

        bool tryUpdateStoryAndSummary(const std::string& inStory, const std::string& inSummary);

        void resize(const int& newWidth, const int& newHeight) {
            gameMap->resizeMap(newWidth, newHeight);
        }
        
    private:

        GameWorldController() {};
        
        template <typename T>
        bool vecIndexInRange(const T& vec, const size_t& index) const;
        

        void showErrorMessage(const std::string& errTextID, const std::string& errTextTitle) const;
        
        bool tryUpdateConnectedTile(const GameTile& firstTile);
        inline const SimplePoint* findConnectionPoint(const GameTile& tile) const;
        


        MainWindowInterface*            mainWindow;

        std::string                     worldFilePath;
        std::string                     worldFileName;

        int                             selectedTileIndex;
        int                             selectedRow;
        int                             selectedCol;
        int                             drawingTileIndex;

        SimplePoint                     firstJumpConnection;
        SimplePoint                     secondJumpConnection;
        SimplePoint                     firstSwitchConnection;
        SimplePoint                     secondSwitchConnection;


        // Game World is composed of several other objects which I have
        // split up here to make doing undo/redo operations easier.

        GameInfo                        gameInfo;
        GameMap*                        gameMap;

        GameMap::GMKey                  gmKey;

};

#endif // __GAMEWORLD_CONTROLLER_H__
