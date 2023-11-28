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

        bool isWorldLoaded() const;

        const GameMap* getGameMap() const;
        const int& getSelectedTileIndex() const;
        const int& getSelectedCol() const;
        const int& getSelectedRow() const;
        
        const int& getDrawingTileIndex() const;
        const GameTile& getSelectedTile() const;

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
        bool tryToggleSwitchState();

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

        bool tryStopConnection(const bool jumpConnection, const bool switchConnection);

        bool tryNewGameWorld();

        bool tryStartSave(const bool saveAs = false);
        bool tryFinishSave(const std::string& newPath, const std::string& newFileName,
                           const bool updateFilePath = true);

        bool tryStartLoad();
        bool tryFinishLoad(const std::string& newPath, const std::string& newFileName);
        
    private:

        GameWorldController() {};
        
        inline bool askAndUpdateSisterTile(const std::string& messageID, const std::string& titleID, const int& x, const int& y);
        bool checkAndAskToSaveUnsavedChanges();

        inline bool findAndRemoveConnection(const GameTile& tile);
        inline void formatCoordinateString(std::string& str, const int& coord1, const int& coord2);
        inline void formatConnectionString(std::string& str, const SimplePoint& coord1, const SimplePoint& coord2);
        void resetEditingDefaults(const bool resetFilePaths);
        void sanitizeObjectStrings(GameObject::Builder& objectBuilder);
        void sanitizeCharacterStrings(GameCharacter::Builder& characterBuilder);
        bool tryRemoveSisterJumppad();
        bool tryRemoveSwitch();
        bool tryRemoveSwitchSisterTile();
        bool updateSelectionIfValid(const int& row = -1, const int& col = -1, const int& index = -1);       
        inline bool validRequestedTileRowCol(const int& row, const int& col) const;
        
        template <typename T>
        bool vecIndexInRange(const T& vec, const size_t& index) const;

        inline bool wasRowColSpecified(const int& row, const int& col) const;
        

        // Variables

        bool                            changedSinceLastSave;

        std::string                     worldFilePath;
        std::string                     worldFileName;

        GameTile::Builder               drawingTile;
        int                             drawingTileIndex;

        int                             selectedTileIndex;
        int                             selectedRow;
        int                             selectedCol;

        SimplePoint                     firstJumpConnection;
        SimplePoint                     secondJumpConnection;
        SimplePoint                     firstSwitchConnection;
        SimplePoint                     secondSwitchConnection;

        MainWindowInterface*            mainWindow;
        GameMap*                        gameMap;

        // [!] DEPERCATED [!]

        GameMap::GMKey                  gmKey;

};

#endif // __GAMEWORLD_CONTROLLER_H__
