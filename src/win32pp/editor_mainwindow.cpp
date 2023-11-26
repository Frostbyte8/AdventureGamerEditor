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
    
    if (ID >= MenuIDs::AddStart && ID <= MenuIDs::AddSafeHaven) {
        gameWorldController->tryAddFeatureToSelectedTile((ID - MenuIDs::AddStart) + 1);
        return TRUE;
    }  

    switch (ID) {

        // TODO: On New and Open need to be interface functions
        case MenuIDs::NewFile: return OnFileNew();
        case MenuIDs::OpenFile: return OnFileOpen();

        case MenuIDs::SaveFile:
        case MenuIDs::SaveFileAs:

            gameWorldController->saveWorld(ID == MenuIDs::SaveFileAs 
                                           ? true : false);
            break;

        
        //case MenuIDs::LongDescription: gameWorldController->tryEditTileDescription(); break;
        case MenuIDs::SummaryAndStory: gameWorldController->tryEditSummaryAndStory(); break;
        case MenuIDs::WorldProperties: gameWorldController->tryEditWorldInfo(); break;
        case MenuIDs::ResizeWorld: gameWorldController->tryEditWorldSize(); break;


        case MenuIDs::FirstJumpConnection:
        case MenuIDs::SecondJumpConnection:
            gameWorldController->tryCreateJumpConnection();
            break;

        case MenuIDs::startSwitchConnection:
        case MenuIDs::endSwitchConnection:
            gameWorldController->tryCreateSwitchConnection();
            break;

        case MenuIDs::MakeTileDark:
            gameWorldController->tryToggleTileDarkness();
            break;

        default: return FALSE;

    }

    return TRUE;
}


///----------------------------------------------------------------------------
/// OnFileNew - Create a new file when File > New is selected.
/// @return Always TRUE to indicate that the message was handled.
///----------------------------------------------------------------------------

BOOL MainWindowFrame::OnFileNew() {
    gameWorldController->newWorld();
    reinterpret_cast<GameMapPanel&>(gameMapDocker->GetView()).updateBackBuffer();
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

    const int currentRow = gameWorldController->getSelectedRow();
    const int currentCol = gameWorldController->getSelectedCol();
    const GameTile& currentTile = gameWorldController->getGameMap()->getTile(index);

    CString newCaption;
    newCaption.Format(L"(%d, %d) - ", currentCol, currentRow);
    newCaption += currentTile.getName().c_str();
    SetStatusText(newCaption);

}

void MainWindowFrame::updateFeatureMenu(const int& index) {

    // Broken

    /*
    const GameMap* gameMap = gameWorldController->getGameMap();
    const GameTile& gameTile = gameMap->getTile(index);

    const uint8_t roadType = gameTile.getSpriteIndex();

    const int numSubMenus = 4;

    UINT enableDarkness = MF_ENABLED;

    for (int i = 0; i < numSubMenus; ++i) {

        if(i != 2) {
            featureMenu.EnableMenuItem(i, MF_GRAYED | MF_BYPOSITION);
        }
    }

    deadendMenu.EnableMenuItem(0, MF_ENABLED | MF_BYPOSITION);

    switch (roadType) {

        case RoadTypes::StraightawayHorizontal:
        case RoadTypes::StraightawayVertical:
            featureMenu.EnableMenuItem(0, MF_ENABLED | MF_BYPOSITION);

            if (roadType == RoadTypes::StraightawayHorizontal) {
                straightAwayMenu.EnableMenuItem(2, MF_GRAYED | MF_BYPOSITION);
                straightAwayMenu.EnableMenuItem(3, MF_GRAYED | MF_BYPOSITION);
                straightAwayMenu.EnableMenuItem(4, MF_ENABLED | MF_BYPOSITION);
                straightAwayMenu.EnableMenuItem(5, MF_ENABLED | MF_BYPOSITION);
            }
            else {
                straightAwayMenu.EnableMenuItem(2, MF_ENABLED | MF_BYPOSITION);
                straightAwayMenu.EnableMenuItem(3, MF_ENABLED | MF_BYPOSITION);
                straightAwayMenu.EnableMenuItem(4, MF_GRAYED | MF_BYPOSITION);
                straightAwayMenu.EnableMenuItem(5, MF_GRAYED | MF_BYPOSITION);
            }

            if(gameTile.hasGate()) {
                enableDarkness = MF_GRAYED;
            }

            break;

        case RoadTypes::CornerNE:
        case RoadTypes::CornerNW:
        case RoadTypes::CornerSE:
        case RoadTypes::CornerSW:
            featureMenu.EnableMenuItem(1, MF_ENABLED | MF_BYPOSITION);
            break;

        case RoadTypes::DeadEndEast:
        case RoadTypes::DeadEndNorth:
        case RoadTypes::DeadEndSouth:
        case RoadTypes::DeadEndWest:
            //featureMenu.EnableMenuItem(2, MF_ENABLED | MF_BYPOSITION);
            deadendMenu.EnableMenuItem(0, MF_ENABLED | MF_BYPOSITION);
            break;

        case RoadTypes::Crossroads:
            featureMenu.EnableMenuItem(3, MF_ENABLED | MF_BYPOSITION);
            break;
    }

    featureMenu.EnableMenuItem(7, enableDarkness | MF_BYPOSITION);
    */

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