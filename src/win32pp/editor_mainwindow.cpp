#include "editor_mainwindow.h"
#include "../editor_constants.h"
#include "../model/gametile.h"
#include <vector>
#include "../util/languagemapper.h"
#include <wxx_commondlg.h>

#include "shared_functions.h"

namespace MenuIDs {

    const WORD LongDescription          = 201;
    const WORD SummaryAndStory          = 202;
    const WORD WorldProperties          = 203;
    const WORD NewFile                  = 204;
    const WORD OpenFile                 = 205;
    const WORD SaveFile                 = 206;
    const WORD SaveFileAs               = 207;
    const WORD ResizeWorld              = 208;

    // For these IDs, we'll just obtain their MOD flag count.

    // Straight Aways

    const WORD AddStart                 = 308;      // MOD1
    const WORD AddFinish                = 309;      // MOD2
    const WORD AddGate                  = 310;      // MOD1+MOD2.
    const WORD AddLockedDoor            = 311;      // MOD3.
    
    const WORD AddBarrierSouth          = 312;      // MOD1+MOD3
    const WORD AddBarrierNorth          = 313;      // MOD2+MOD3

    const WORD AddBarrierEast           = 312;      // MOD1+MOD3
    const WORD AddBarrierWest           = 313;      // MOD2+MOD3

    // Corners

    const WORD AddSwitchOn              = 308;      // MOD1
    const WORD AddSwitchOff             = 309;      // MOD2

    // Dead Ends

    const WORD AddJumpPad               = 310;      // MOD1+MOD2

    // Cross Roads

    const WORD AddHazard                = 311;      // MOD3
    const WORD AddSafeHaven             = 314;      // ALLMODS

    // Additional Menu Items

    const WORD FirstJumpConnection      = 315;
    const WORD SecondJumpConnection     = 316;

    const WORD startSwitchConnection    = 317;
    const WORD endSwitchConnection      = 318;

    const WORD MakeTileDark             = 319;
   
}

//=============================================================================
// Constructors / Destructor
//=============================================================================

void MainWindowFrame::RecalcDockLayout() {

    if (isSizing) {
        CDockFrame::RecalcDockLayout();
    }
    else {
        LockWindowUpdate();
        CDockFrame::RecalcDockLayout();
        UnLockWindowUpdate();
    }

}


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

///----------------------------------------------------------------------------
/// Create - Set View and continue creating the frame.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

HWND MainWindowFrame::Create(HWND parent) {
    SetView(*entityView);
    
    
	return CDockFrame::Create(parent);
}

//=============================================================================
// Protected / Private Functions
//=============================================================================

///----------------------------------------------------------------------------
/// CreateMenuBar - Creates the menu bar.
///----------------------------------------------------------------------------

void MainWindowFrame::CreateMenuBar() {

    mainMenu.CreateMenu();
    fileMenu.CreatePopupMenu();
    editMenu.CreatePopupMenu();
    featureMenu.CreatePopupMenu();
    straightAwayMenu.CreatePopupMenu();
    cornerMenu.CreatePopupMenu();
    deadendMenu.CreatePopupMenu();
    crossroadMenu.CreatePopupMenu();

    LanguageMapper& langMap = LanguageMapper::getInstance();

    fileMenu.AppendMenu(MF_STRING, MenuIDs::NewFile, LM_toUTF8("NewMenuItem", langMap));
    fileMenu.AppendMenu(MF_STRING, MenuIDs::OpenFile, LM_toUTF8("OpenMenuItem", langMap));
    fileMenu.AppendMenu(MF_STRING, MenuIDs::SaveFile, LM_toUTF8("SaveMenuItem", langMap));
    fileMenu.AppendMenu(MF_STRING, MenuIDs::SaveFileAs, LM_toUTF8("SaveAsMenuItem", langMap));
    fileMenu.AppendMenu(MF_STRING, 0, LM_toUTF8("ExitMenuItem", langMap));

    editMenu.AppendMenu(MF_STRING, MenuIDs::LongDescription, LM_toUTF8("LongTileMenuItem", langMap));
    editMenu.AppendMenu(MF_STRING, MenuIDs::SummaryAndStory, LM_toUTF8("SummaryMenuItem", langMap));
    editMenu.AppendMenu(MF_STRING, MenuIDs::WorldProperties, LM_toUTF8("WorldMenuItem", langMap));
    editMenu.AppendMenu(MF_STRING, MenuIDs::ResizeWorld, LM_toUTF8("ResizeWorldMenuItem", langMap));

    featureMenu.AppendMenu(MF_STRING | MF_POPUP, reinterpret_cast<UINT_PTR>(straightAwayMenu.GetHandle()), LM_toUTF8("StraightAwayMenuItem", langMap));
    featureMenu.AppendMenu(MF_STRING | MF_POPUP, reinterpret_cast<UINT_PTR>(cornerMenu.GetHandle()), LM_toUTF8("CornerMenuItem", langMap));
    featureMenu.AppendMenu(MF_STRING | MF_POPUP, reinterpret_cast<UINT_PTR>(deadendMenu.GetHandle()), LM_toUTF8("DeadEndMenuItem", langMap));
    featureMenu.AppendMenu(MF_STRING | MF_POPUP, reinterpret_cast<UINT_PTR>(crossroadMenu.GetHandle()), LM_toUTF8("CrossroadsMenuItem", langMap));

    // Deal with all the Feature Sub-Menus

    straightAwayMenu.AppendMenu(MF_STRING, MenuIDs::AddStart, LM_toUTF8("StartMenuItem", langMap));
    straightAwayMenu.AppendMenu(MF_STRING, MenuIDs::AddFinish, LM_toUTF8("FinishMenuItem", langMap));
    straightAwayMenu.AppendMenu(MF_STRING, MenuIDs::AddBarrierNorth, LM_toUTF8("NorthBarrMenuItem", langMap));
    straightAwayMenu.AppendMenu(MF_STRING, MenuIDs::AddBarrierSouth, LM_toUTF8("SouthBarrMenuItem", langMap));
    straightAwayMenu.AppendMenu(MF_STRING, MenuIDs::AddBarrierEast, LM_toUTF8("EastBarrMenuItem", langMap));
    straightAwayMenu.AppendMenu(MF_STRING, MenuIDs::AddBarrierWest, LM_toUTF8("WestBarrMenuItem", langMap));
    straightAwayMenu.AppendMenu(MF_STRING, MenuIDs::AddGate, LM_toUTF8("GateMenuItem", langMap));
    straightAwayMenu.AppendMenu(MF_STRING, MenuIDs::AddLockedDoor, LM_toUTF8("LockedDoorMenuItem", langMap));

    cornerMenu.AppendMenu(MF_STRING, MenuIDs::AddSwitchOn, LM_toUTF8("SwitchOnMenuItem", langMap));
    cornerMenu.AppendMenu(MF_STRING, MenuIDs::AddSwitchOff, LM_toUTF8("SwitchOffMenuItem", langMap));

    deadendMenu.AppendMenu(MF_STRING, MenuIDs::AddJumpPad, LM_toUTF8("JumppadMenuItem", langMap));

    crossroadMenu.AppendMenu(MF_STRING, MenuIDs::AddHazard, LM_toUTF8("HazardMenuItem", langMap));
    crossroadMenu.AppendMenu(MF_STRING, MenuIDs::AddSafeHaven, LM_toUTF8("SafeHavenMenuItem", langMap));

    // Append extra items where needed

    deadendMenu.AppendMenu(MF_STRING, MenuIDs::FirstJumpConnection, LM_toUTF8("JumppadConnectFirst", langMap));
    deadendMenu.AppendMenu(MF_STRING | MF_GRAYED, MenuIDs::SecondJumpConnection, LM_toUTF8("JumppadConnectSecond", langMap));

    featureMenu.AppendMenu(MF_SEPARATOR);
    featureMenu.AppendMenu(MF_STRING, MenuIDs::startSwitchConnection, LM_toUTF8("StartSwitchMenuItem", langMap));
    featureMenu.AppendMenu(MF_STRING | MF_GRAYED, MenuIDs::endSwitchConnection, LM_toUTF8("EndSwitchMenuItem", langMap));
    featureMenu.AppendMenu(MF_STRING, MenuIDs::MakeTileDark, LM_toUTF8("MakeTileDarkMenuItem", langMap));


    // Finally deal with the menu bar

    CString caption = AtoW(langMap.get("FileMenu").c_str(), CP_UTF8);

    mainMenu.AppendMenu(MF_STRING | MF_POPUP,
                        reinterpret_cast<UINT_PTR>(fileMenu.GetHandle()), caption);

    caption = AtoW(langMap.get("EditMenu").c_str(), CP_UTF8);

    mainMenu.AppendMenu(MF_STRING | MF_POPUP,
                        reinterpret_cast<UINT_PTR>(editMenu.GetHandle()), caption);

    caption = AtoW(langMap.get("AddMenuItem").c_str(), CP_UTF8);

    mainMenu.AppendMenu(MF_STRING | MF_POPUP,
                        reinterpret_cast<UINT_PTR>(featureMenu.GetHandle()), caption);

    SetFrameMenu(mainMenu);

}



///----------------------------------------------------------------------------
/// OnCreate - Set some defaults for the frame, and create remaining child
/// controls.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

int MainWindowFrame::OnCreate(CREATESTRUCT& cs) {
	
	UseThemes(FALSE);				// Don't use themes
    //m_bUseCustomDraw = FALSE;     // Don't use custom draw for menu items (?)

    SetStyle(GetStyle() | WS_CLIPCHILDREN);

    // TODO: The tileset image should be created here and pass to the view on creation.

    if(!loadTileSet()) {
        MessageBox(L"Could not find tileset.bmp. Please ensure this file is in the same directory as advedit.exe. The program will now close.", L"Missing file tileset.bmp", MB_OK | MB_ICONERROR);
        Close();
        return 1;
    }

    /*
    if(!(reinterpret_cast<GameMapView&>(gameMapDocker->GetView()).isBMPLoaded())) {
    }
    */

	const int retVal = CDockFrame::OnCreate(cs);

    LanguageMapper& langMap = LanguageMapper::getInstance();
    CString caption = AtoW(langMap.get("MainWindowTitle").c_str(), CP_UTF8);
    SetWindowText(caption);

    CreateMenuBar();

    CRect windowDims = GetClientRect();

	// Create our Dockers

	DWORD styleFlags = DS_NO_UNDOCK | DS_NO_CAPTION | DS_DEFAULT_CURSORS | DS_CLIENTEDGE;
	SetDockStyle(styleFlags);

    gameMapDocker = static_cast<GameMapDocker*>(AddDockedChild(new GameMapDocker(this, gameWorldController, &tilesetBMP), 
                                                styleFlags | DS_DOCKED_LEFT, 128));
	
	reinterpret_cast<GameMapPanel&>(gameMapDocker->GetView()).setTileset(tilesetBMP);


	roadSelectorDocker = static_cast<RoadSelectorDocker*>(gameMapDocker->AddDockedChild(
                                                          new RoadSelectorDocker(this, gameWorldController),
                                                          styleFlags | DS_DOCKED_LEFT | DS_NO_RESIZE, 0));
                                                          
	reinterpret_cast<RoadPalettePanel&>(roadSelectorDocker->GetView()).setTileset(tilesetBMP);

	entitiesHereDocker = static_cast<EntitiesHereDocker*>(gameMapDocker->AddDockedChild(
                                                          new EntitiesHereDocker(&windowMetrics), styleFlags | DS_DOCKED_BOTTOM, 128));

    // The Road Selector is the Width of one tile plus the scroll bar
    // TODO: Zoom Factor

	CRect rc;

    const int tempTileWidth = tileWidth;
	rc.right = tempTileWidth + windowMetrics.GetControlDimensions().X_SCROLLBAR;
	AdjustWindowRectEx(&rc, 0, FALSE, roadSelectorDocker->GetDockClient().GetExStyle());
	const int newWidth = abs(rc.right - rc.left);
	roadSelectorDocker->SetDockSize(newWidth);   

    windowDims.right -= newWidth;

    const int mapSize = static_cast<int>(windowDims.right * 0.5);
    gameMapDocker->SetDockSize(mapSize);

    windowDims.right -= mapSize;

    // Set the font to the font specified within window metrics.

    HFONT dialogFont = windowMetrics.GetCurrentFont();
    EnumChildWindows(*this, reinterpret_cast<WNDENUMPROC>(SetProperFont), (LPARAM)dialogFont);


    return retVal;
}

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

	/*
    // Catch all CException types.
    catch (const CException& e)
    {
        // Display the exception and continue.
        ::MessageBox(0, e.GetText(), AtoT(e.what()), MB_ICONERROR);

        return 0;
    }
	*/


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

    }


	return WndProcDefault(msg, wParam, lParam);
}

///----------------------------------------------------------------------------
/// OnCommand - Processes WM_COMMAND events
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

BOOL MainWindowFrame::OnCommand(WPARAM wParam, LPARAM) {

    const WORD ID = LOWORD(wParam);
    
    if (ID >= MenuIDs::AddStart && ID <= MenuIDs::AddSafeHaven) {
        if (gameWorldController->tryAddFeatureToTile((ID - MenuIDs::AddStart) + 1)) {
            reinterpret_cast<GameMapPanel&>(gameMapDocker->GetView()).updateBackBuffer();
        }
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

        // TODO: These all need to call the controller instead.
        case MenuIDs::LongDescription: gameWorldController->tryEditTileDescription(); break;
        case MenuIDs::SummaryAndStory: gameWorldController->tryEditSummaryAndStory(); break;
        case MenuIDs::WorldProperties: gameWorldController->tryEditWorldInfo(); break;
        case MenuIDs::ResizeWorld: gameWorldController->tryEditWorldSize(); break;


        case MenuIDs::FirstJumpConnection:
        case MenuIDs::SecondJumpConnection:
            addJumpConnection(ID);
            break;

        case MenuIDs::startSwitchConnection:
        case MenuIDs::endSwitchConnection:
            addSwitchConnection(ID);
            break;

        case MenuIDs::MakeTileDark:
            gameWorldController->tryMakeTileDark();
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

    const GameMap* gameMap = gameWorldController->getGameMap();
    const GameTile& gameTile = gameMap->getTile(index);

    const uint8_t roadType = gameTile.getSpriteIndex();

    const int numSubMenus = 4;

    for (int i = 0; i < numSubMenus; ++i) {
        featureMenu.EnableMenuItem(i, MF_GRAYED | MF_BYPOSITION);
    }

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
            featureMenu.EnableMenuItem(2, MF_ENABLED | MF_BYPOSITION);
            break;

        case RoadTypes::Crossroads:
            featureMenu.EnableMenuItem(3, MF_ENABLED | MF_BYPOSITION);
            break;
    }
}

//=============================================================================
// Public Interface Functions
// Refer to the interface header file for more information.
//=============================================================================

//-----------------------------------------------------------------------------
// onSelectedTileChanged
//-----------------------------------------------------------------------------

bool MainWindowFrame::onSelectedTileChanged(const int& row, const int& col) {
    
    const GameMap* gameMap = gameWorldController->getGameMap();

    const std::vector<GameObject>& objectVec = gameMap->getGameObjectsAtRowCol(row, col);
    const std::vector<GameCharacter>& charVec = gameMap->getGameCharactersAtRowCol(row, col);

    EntitiesHerePanel& herePanel = reinterpret_cast<EntitiesHerePanel&>(entitiesHereDocker->GetView());

    herePanel.updateObjectList(objectVec);
    herePanel.updateCharacterList(charVec);

    updateFeatureMenu(gameMap->indexFromRowCol(row, col));
    updateStatusbar(gameMap->indexFromRowCol(row, col));

    return true;
}

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


void MainWindowFrame::addJumpConnection(const int& whichPoint) {
    
    if (whichPoint == MenuIDs::FirstJumpConnection) {
        if (gameWorldController->tryAddFirstJumpConnection()) {
            deadendMenu.EnableMenuItem(1, MF_GRAYED | MF_BYPOSITION);
            deadendMenu.EnableMenuItem(2, MF_ENABLED | MF_BYPOSITION);
            return;
        }
    }
    else if (whichPoint == MenuIDs::SecondJumpConnection) {
        if (gameWorldController->tryAddSecondJumpConnection()) {
            MessageBox(L"Jump connection added successfully.", L"Jump Added", MB_OK);
        }
        else {
            MessageBox(L"Unable to add Jump Connection.", L"Error adding Jump", MB_ICONERROR);
        }
    }

    // If any failures happens, start over.

    deadendMenu.EnableMenuItem(2, MF_GRAYED | MF_BYPOSITION);
    deadendMenu.EnableMenuItem(1, MF_ENABLED | MF_BYPOSITION);

}

void MainWindowFrame::addSwitchConnection(const int& whichPoint) {
    
    if (whichPoint == MenuIDs::startSwitchConnection) {
        if (gameWorldController->tryStartSwitchConnection()) {
            featureMenu.EnableMenuItem(5, MF_GRAYED | MF_BYPOSITION);
            featureMenu.EnableMenuItem(6, MF_ENABLED | MF_BYPOSITION);
            return;
        }
    }
    else if (whichPoint == MenuIDs::endSwitchConnection) {
        if (gameWorldController->tryEndSwitchConnection()) {
            MessageBox(L"Switch connection added successfully.", L"Switch Added", MB_OK);
        }
        else {
            MessageBox(L"Unable to add Switch Connection.", L"Error adding Switch", MB_ICONERROR);
        }
    }

    featureMenu.EnableMenuItem(6, MF_GRAYED | MF_BYPOSITION);
    featureMenu.EnableMenuItem(5, MF_ENABLED | MF_BYPOSITION);
}