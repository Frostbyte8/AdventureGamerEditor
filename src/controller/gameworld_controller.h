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

        bool canAddObject() const;
        bool canAddCharacter() const;
        bool hasFirstJumpConnectionBeenSet() const;
        bool hasFirstSwitchConnectionBeenSet() const;

        const GameMap* getGameMap() const { return gameMap; }
        const int& getMapWidth() const { return gameMap->getWidth(); }
        const int& getMapHeight() const { return gameMap->getHeight(); }
        const int& getSelectedTileIndex() const { return selectedTileIndex; }
        const int& getSelectedRow() const { return selectedRow; }
        const int& getSelectedCol() const { return selectedCol; }
        const int& getDrawingTileIndex() const { return drawingTileIndex; }
        const GameTile& getSelectedTile() const { return gameMap->getTile(selectedTileIndex); }

        bool hasUnsavedChanges() const;
        
        bool tryAlterObject(const int& alterType, const int& index);
        bool tryAddObject(GameObject::Builder& objectBuilder);

        bool tryReplaceObject(GameObject::Builder& objectBuilder, 
                              const bool shouldNotify = true);

        bool tryDeleteObject(const int& charID);

        bool tryAlterCharacter(const int& alterType, const int& index);
        bool tryAddCharacter(GameCharacter::Builder& characterBuilder);

        bool tryReplaceCharacter(GameCharacter::Builder& objectBuilder,
                                 const bool shouldNotify = true);

        bool tryDeleteCharacter(const int& charID);
       
        bool tryAddFeatureToSelectedTile(const int& featureType);

        bool trySelectNewTile(const int& row, const int& col);
        bool trySelectNewTile(const int& index);

        bool trySetDrawingTile(const int& newDrawTileIndex);
        bool tryDrawOnSelectedTile();

        bool tryToggleTileDarkness();

        bool tryEditTileDescription(const int& row = EditorConstants::IGNORE_ROW,
                                    const int& col = EditorConstants::IGNORE_COL);

        bool tryUpdateTileDescription(std::string tileName, std::string tileDescription,
                                      const int& row = EditorConstants::IGNORE_ROW,
                                      const int& col = EditorConstants::IGNORE_COL);


        bool tryEditSummaryAndStory();
        bool tryUpdateStoryAndSummary(std::string inStory, std::string inSummary);

        bool tryEditWorldInfo();
        bool tryUpdateWorldInfo(const GameInfo& newInfo);

        bool tryEditWorldSize();
        bool tryResizeWorld(const int& numRows, const int& numCols);

        bool tryCreateJumpConnection();
        bool tryCreateSwitchConnection();

        //

        bool newWorld();
        bool loadWorld(const std::string& filePath, const std::string& fileName);
        bool saveWorld(bool saveAs = false);

        
    private:

        GameWorldController() {};

        // void resetDefaults

        inline bool validRequestedTileRowCol(const int& row, const int& col) const;
        inline bool wasRowColSpecified(const int& row, const int& col) const;
        inline void formatCoordinateString(std::string& str, const int& coord1, const int& coord2);
        inline void formatConnectionString(std::string& str, const SimplePoint& coord1, const SimplePoint& coord2);

        inline bool findAndRemoveConnection(const GameTile& tile);

        template <typename T>
        bool vecIndexInRange(const T& vec, const size_t& index) const;

        bool updateSelectionIfValid(const int& row = -1, const int& col = -1, const int& index = -1);

        void sanitizeObjectStrings(GameObject::Builder& objectBuilder);
        void sanitizeCharacterStrings(GameCharacter::Builder& characterBuilder);

        bool tryRemoveSisterJumppad();
        bool tryRemoveSwitch();
        bool tryRemoveSwitchSisterTile();

        inline bool askAndUpdateSisterTile(const std::string& messageID, const std::string& titleID, const int& x, const int& y);
        
        inline const SimplePoint* findConnectionPoint(const GameTile& tile) const;
        
        bool                            changedSinceLastSave;

        MainWindowInterface*            mainWindow;

        std::string                     worldFilePath;
        std::string                     worldFileName;

        GameTile::Builder               drawingTile;
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
