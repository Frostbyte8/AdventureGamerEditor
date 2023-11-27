#include "editor_mainwindow.h"
#include "../editor_constants.h"
#include "../model/gametile.h"
#include <vector>
#include "../util/languagemapper.h"
#include <wxx_commondlg.h>

#include "shared_functions.h"



//=============================================================================
// Constructors / Destructor
//=============================================================================

///----------------------------------------------------------------------------
/// MainWindowFrame - Creates a new window frame with entity view as the
/// main view.
///----------------------------------------------------------------------------

MainWindowFrame::MainWindowFrame() : entityView(0), gameMapDocker(0), entitiesHereDocker(0), 
roadSelectorDocker(0), gameWorldController(0), activeWindowHandle(0), editObjectDialog(0),
editCharacterDialog(0), editWorldInfoDialog(0), editStoryDialog(0),
editTileDescriptionDialog(0), resizeWorldDialog(0), tileWidth(0), tileHeight(0) {
    gameWorldController = new GameWorldController(this);
	entityView = new GameEntitiesPanel(gameWorldController, &windowMetrics);
    LanguageMapper::getInstance();
    isSizing = false;
}

///----------------------------------------------------------------------------
/// ~MainWindowFrame - Cleans up resources used by the main window frame.
///----------------------------------------------------------------------------

MainWindowFrame::~MainWindowFrame() {

	if(entityView) {
		delete entityView;
		entityView = NULL;
	}

    if(gameWorldController) {
        delete gameWorldController;
        gameWorldController = NULL;
    }

    if(editObjectDialog) {
        delete editObjectDialog;
        editObjectDialog = NULL;
    }

    if(editCharacterDialog) {
        delete editCharacterDialog;
        editCharacterDialog = NULL;
    }

    if(editWorldInfoDialog) {
        delete editWorldInfoDialog;
        editWorldInfoDialog = NULL;
    }

    if(editStoryDialog) {
        delete editStoryDialog;
        editStoryDialog = NULL;
    }

    if(editTileDescriptionDialog) {
        delete editTileDescriptionDialog;
        editTileDescriptionDialog = NULL;
    }

}

//=============================================================================
// Public Functions
//=============================================================================

//=============================================================================
// Protected / Private Functions
//=============================================================================

///----------------------------------------------------------------------------
/// OnInitialUpdate - 
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

void MainWindowFrame::OnInitialUpdate() {
    activeWindowHandle = GetHwnd(); 
}

///----------------------------------------------------------------------------
/// WndProc - Window Procedure for the Frame.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

LRESULT MainWindowFrame::WndProc(UINT msg, WPARAM wParam, LPARAM lParam) {

	try {
        switch (msg) {

            case WM_ACTIVATEAPP:
                ::BringWindowToTop(activeWindowHandle);
                return 0;

            case WM_ENTERSIZEMOVE:
                isSizing = true;
                break;

            case WM_EXITSIZEMOVE:
                isSizing = false;
                break;

            case WM_HELP:
                // Disable the help dialog. TODO: See if there is a way
                // to stop the exception from occurring if the dialog does not
                // exist.
                return 0;
                break;

        }

	    return WndProcDefault(msg, wParam, lParam);

    }
    catch (const CException& e)
    {
        // Display the exception and continue.
        ::MessageBox(0, e.GetText(), AtoT(e.what()), MB_ICONERROR);

        return 0;
    }
}

///----------------------------------------------------------------------------
/// OnCommand - Processes WM_COMMAND events
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

BOOL MainWindowFrame::OnCommand(WPARAM wParam, LPARAM) {

    const WORD ID = LOWORD(wParam);

    switch(ID) {

        // File Menu

        case MenuIDs::NewFile: gameWorldController->tryNewGameWorld(); break;
        case MenuIDs::OpenFile: return OnFileOpen();

        case MenuIDs::SaveFile:
        case MenuIDs::SaveFileAs:
            gameWorldController->saveWorld(ID == MenuIDs::SaveFileAs
                                           ? true : false);
            break;

        // Tile Menu
        
        case MenuIDs::EditDescription: gameWorldController->tryEditTileDescription(); break;

        // Feature Menu

        case MenuIDs::StartJumpConnection:
        case MenuIDs::EndJumpConnection: 
            gameWorldController->tryCreateJumpConnection(); 
            break;

        case MenuIDs::StartSwitchConnection:
        case MenuIDs::EndSwitchConnection: 
            gameWorldController->tryToggleTileDarkness(); 
            break;

        case MenuIDs::ToggleTileDarkness: gameWorldController->tryToggleTileDarkness(); break;

        // Straight Aways Menu

        case MenuIDs::AddStart: gameWorldController->tryAddFeatureToSelectedTile(TileModifiers::Start); break;
        case MenuIDs::AddFinish: gameWorldController->tryAddFeatureToSelectedTile(TileModifiers::Finish); break;
        case MenuIDs::AddGate: gameWorldController->tryAddFeatureToSelectedTile(TileModifiers::GateClosed); break;
        case MenuIDs::AddLockedDoor: gameWorldController->tryAddFeatureToSelectedTile(TileModifiers::LockedDoor); break;
        
        case MenuIDs::AddBarrierNorth: 
        case MenuIDs::AddBarrierWest: 
            gameWorldController->tryAddFeatureToSelectedTile(TileModifiers::BarrierNorth); 
            break;

        case MenuIDs::AddBarrierSouth:
        case MenuIDs::AddBarrierEast: 

            gameWorldController->tryAddFeatureToSelectedTile(TileModifiers::BarrierSouth);
            break;

        // Corners Menu

        case MenuIDs::AddOnSwitch: gameWorldController->tryAddFeatureToSelectedTile(TileModifiers::SwitchOn); break;
        case MenuIDs::AddOffSwitch: gameWorldController->tryAddFeatureToSelectedTile(TileModifiers::SwitchOff); break;

        // Dead-ends Menu

        case MenuIDs::AddJumpPad: gameWorldController->tryAddFeatureToSelectedTile(TileModifiers::JumpPad); break;

        // Crossroads Menu

        case MenuIDs::AddHazard: gameWorldController->tryAddFeatureToSelectedTile(TileModifiers::Hazard); break;
        case MenuIDs::AddSafeHaven: gameWorldController->tryAddFeatureToSelectedTile(TileModifiers::SafeHaven); break;

        // World Menu

        case MenuIDs::SummaryAndStory: gameWorldController->tryEditSummaryAndStory(); break;
        case MenuIDs::WorldProperties: gameWorldController->tryEditWorldInfo(); break;
        case MenuIDs::ResizeWorld: gameWorldController->tryEditWorldSize(); break;

        default:
            return FALSE;

    }
    
    return TRUE;

}

///----------------------------------------------------------------------------
/// OnFileOpen - Opens a dialog box for the user to select a Adventure Gamer 
/// SG0 file, then passes the file and it's path to the controller to try to
/// open if one is selected.
/// @return Always TRUE to indicate that the message was handled.
///----------------------------------------------------------------------------

BOOL MainWindowFrame::OnFileOpen() {

	CFileDialog fileDialog(TRUE, L"SG0", NULL, OFN_NOLONGNAMES | OFN_FILEMUSTEXIST,
                           L"Adventure Gamer World Files (*.SG0)\0*.SG0\0\0");

	fileDialog.SetTitle(L"Open World File");

	if(fileDialog.DoModal(*this) == IDOK) {

		std::string filePath(WtoA(fileDialog.GetFolderPath().c_str()));
		std::string fileName(WtoA(fileDialog.GetFileName().c_str()));

		gameWorldController->loadWorld(filePath, fileName);
        reinterpret_cast<GameMapPanel&>(gameMapDocker->GetView()).updateBackBuffer();
        
        
        entityView->updateObjectList(gameWorldController->getGameMap()->getGameObjects());
        entityView->updateCharacterList(gameWorldController->getGameMap()->getGameCharacters());

	}

    return TRUE;
}

///----------------------------------------------------------------------------
/// loadTileset - Load's the tileset BMP
/// @return true if it loaded successfully, false if it did not.
///----------------------------------------------------------------------------

bool MainWindowFrame::loadTileSet() {

    // TODO: tileset should be loaded to a temp BMP that is transfered over once it loads
    // successfully.

    tilesetBMP.LoadImage(L"tileset.bmp", LR_LOADFROMFILE | LR_CREATEDIBSECTION);

    if(!tilesetBMP.GetHandle()) {
        return false;
    }

    double tempWidth     = tilesetBMP.GetSize().cx / EditorConstants::TilesPerCol;
    double tempHeight    = tilesetBMP.GetSize().cy / EditorConstants::TilesPerRow;

    tileWidth   = static_cast<int>(tempWidth);
    tileHeight  = static_cast<int>(tempHeight);

    if(tempWidth - tileWidth != 0) {
        MessageBox(L"Bitmap is not perfectly divisible by 16.", L"", MB_ICONWARNING | MB_OK);
    }

    if(tempHeight - tileHeight != 0) {
        MessageBox(L"Bitmap is not perfectly divisible by 16.", L"", MB_ICONWARNING | MB_OK);
    }


    return true;

}

void MainWindowFrame::updateHereLists(const bool objectsHere, const bool charsHere,
                                      const GameMap* inMap, const int* row, const int* col) {

    const GameMap* gameMap = inMap ? inMap : gameWorldController->getGameMap();
    const int& selectedRow = row ? *row : gameWorldController->getSelectedRow();
    const int& selectedCol = col ? *col : gameWorldController->getSelectedCol();
    
    // Update the character's on the new tile.

    EntitiesHerePanel& herePanel = reinterpret_cast<EntitiesHerePanel&>(entitiesHereDocker->GetView());

    if(objectsHere) {
        const std::vector<GameObject>& objectVec = gameMap->getGameObjectsAtRowCol(selectedRow, selectedCol);
        herePanel.updateObjectList(objectVec);
    }

    if(charsHere) {
        const std::vector<GameCharacter>& charVec = gameMap->getGameCharactersAtRowCol(selectedRow, selectedCol);
        herePanel.updateCharacterList(charVec);
    }

}

void MainWindowFrame::updateStatusbar(const int& index) {

    const GameMap* gameMap = gameWorldController->getGameMap();

    CString newCaption = "";

    if(gameMap) {

        const int currentRow = gameWorldController->getSelectedRow();
        const int currentCol = gameWorldController->getSelectedCol();
        const GameTile& currentTile = gameWorldController->getGameMap()->getTile(index);

        newCaption.Format(L"(%d, %d) - ", currentCol, currentRow);
        newCaption += currentTile.getName().c_str();
    }
    else {
        newCaption = LM_toUTF8("NoWorldLoaded", LanguageMapper::getInstance());
    }

    SetStatusText(newCaption);

}

void MainWindowFrame::updateFeatureMenu(const int& index) {

    const GameMap* gameMap = gameWorldController->getGameMap();
    const GameTile& gameTile = gameMap->getTile(index);

    for (int i = MenuIDs::StraightAwayMenu; i <= MenuIDs::CrossroadsMenu; ++i) {
        featureMenu.EnableMenuItem(i, MF_GRAYED | MF_DISABLED);
    }

    if(gameTile.isStraightaway()) {
        featureMenu.EnableMenuItem(MenuIDs::StraightAwayMenu, MF_ENABLED);

        if(gameTile.isHorizontalStraightaway()) {
            straightAwayMenu.EnableMenuItem(MenuIDs::AddBarrierNorth, MF_ENABLED);
            straightAwayMenu.EnableMenuItem(MenuIDs::AddBarrierSouth, MF_ENABLED);
            straightAwayMenu.EnableMenuItem(MenuIDs::AddBarrierEast, MF_GRAYED | MF_DISABLED);
            straightAwayMenu.EnableMenuItem(MenuIDs::AddBarrierWest, MF_GRAYED | MF_DISABLED);
        }
        else {
            straightAwayMenu.EnableMenuItem(MenuIDs::AddBarrierNorth, MF_GRAYED | MF_DISABLED);
            straightAwayMenu.EnableMenuItem(MenuIDs::AddBarrierSouth, MF_GRAYED | MF_DISABLED);
            straightAwayMenu.EnableMenuItem(MenuIDs::AddBarrierEast, MF_ENABLED);
            straightAwayMenu.EnableMenuItem(MenuIDs::AddBarrierWest, MF_ENABLED);
        }

    }
    else if(gameTile.isCorner()) {
        featureMenu.EnableMenuItem(MenuIDs::CornerMenu, MF_ENABLED);
    }
    else if(gameTile.isDeadend()) {
        featureMenu.EnableMenuItem(MenuIDs::DeadEndMenu, MF_ENABLED);
    }
    else if(gameTile.isCrossroads()) {
        featureMenu.EnableMenuItem(MenuIDs::CrossroadsMenu, MF_ENABLED);
    }

    if(!gameWorldController->hasFirstJumpConnectionBeenSet()) {

        if(gameTile.hasJumpPad()) {
            featureMenu.EnableMenuItem(MenuIDs::StartJumpConnection, MF_ENABLED);
        }
        else {
            featureMenu.EnableMenuItem(MenuIDs::StartJumpConnection, MF_GRAYED | MF_DISABLED);
        }

        featureMenu.EnableMenuItem(MenuIDs::EndJumpConnection, MF_GRAYED | MF_DISABLED);

    }
    else {

        featureMenu.EnableMenuItem(MenuIDs::StartJumpConnection, MF_GRAYED | MF_DISABLED);
        featureMenu.EnableMenuItem(MenuIDs::EndJumpConnection, MF_ENABLED);

    }

    if(!gameWorldController->hasFirstSwitchConnectionBeenSet()) {

        if(gameTile.hasSwitch()) {
             featureMenu.EnableMenuItem(MenuIDs::StartSwitchConnection, MF_ENABLED);
        }
        else {
            featureMenu.EnableMenuItem(MenuIDs::StartSwitchConnection, MF_GRAYED | MF_DISABLED);
        }

        featureMenu.EnableMenuItem(MenuIDs::EndSwitchConnection, MF_GRAYED | MF_DISABLED);

    }
    else {
        
        featureMenu.EnableMenuItem(MenuIDs::StartSwitchConnection, MF_GRAYED | MF_DISABLED);

        if(gameTile.hasGate() || gameTile.isDark()) {
            featureMenu.EnableMenuItem(MenuIDs::EndSwitchConnection, MF_ENABLED);
        }
        else {
            featureMenu.EnableMenuItem(MenuIDs::EndSwitchConnection, MF_GRAYED | MF_DISABLED);
        }
        
    }

}

//=============================================================================
// Public Interface Functions
// Refer to the interface header file for more information.
//=============================================================================

//-----------------------------------------------------------------------------
// onSaveFileDialog
//-----------------------------------------------------------------------------

int MainWindowFrame::onSaveFileDialog(std::string& filePath, std::string& fileName) {
    CFileDialog fileDialog(FALSE, L"SG0", NULL, OFN_NOLONGNAMES | OFN_FILEMUSTEXIST,
                           L"Adventure Gamer World Files (*.SG0)\0*.SG0\0\0");

    //fileDialog.SetTitle(L"Open World File");

    if (fileDialog.DoModal(*this) == IDOK) {

        filePath = WtoA(fileDialog.GetFolderPath().c_str());
        fileName = WtoA(fileDialog.GetFileName().c_str());

        return GenericInterfaceResponses::Ok;
    }

    return GenericInterfaceResponses::Cancel;
}