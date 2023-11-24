#include "editor_mainwindow.h"
#include "shared_functions.h"
#include "../util/languagemapper.h"
#include "../gitinfo.h"

//=============================================================================
// Win32++ Definitions
//=============================================================================

//-----------------------------------------------------------------------------
// Window Messages
//-----------------------------------------------------------------------------

///----------------------------------------------------------------------------
/// OnCreate - Set some defaults for the frame, and create remaining child
/// controls.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

int MainWindowFrame::OnCreate(CREATESTRUCT& cs) {

    LanguageMapper& langMap = LanguageMapper::getInstance();
    CString caption;

    // Prevent Win32++ from using themes.
    UseThemes(FALSE);

    if(!loadTileSet()) {
        MessageBox(L"Could not find tileset.bmp. Please ensure this file is in the same directory as advedit.exe. The program will now close.", L"Missing file tileset.bmp", MB_OK | MB_ICONERROR);
        Close();
        return 1;
    }

    

	const int retVal = CDockFrame::OnCreate(cs);

    // Deal with the menubar and title bar next.

    CreateMenuBar();
    updateTitleBar(false);

    CRect windowDims = GetClientRect();

	// Create our Dockers

	DWORD styleFlags = DS_NO_UNDOCK | DS_NO_CAPTION | DS_DEFAULT_CURSORS | DS_CLIENTEDGE;
	SetDockStyle(styleFlags);

    // Game map and road selector need to be aware of the tilest.

    gameMapDocker = static_cast<GameMapDocker*>(AddDockedChild(new GameMapDocker(this, gameWorldController, &tilesetBMP), 
                                                styleFlags | DS_DOCKED_LEFT, 128));

	reinterpret_cast<GameMapPanel&>(gameMapDocker->GetView()).setTileset(tilesetBMP);


	roadSelectorDocker = static_cast<RoadSelectorDocker*>(gameMapDocker->AddDockedChild(
                                                          new RoadSelectorDocker(this, gameWorldController),
                                                          styleFlags | DS_DOCKED_LEFT | DS_NO_RESIZE, 0));
                                                          
	reinterpret_cast<RoadPalettePanel&>(roadSelectorDocker->GetView()).setTileset(tilesetBMP);

	entitiesHereDocker = static_cast<EntitiesHereDocker*>(gameMapDocker->AddDockedChild(
                                                          new EntitiesHereDocker(&windowMetrics), styleFlags | DS_DOCKED_BOTTOM, 128));

    // The Road Selector is the Width of one tile plus the scroll bar.
    // TODO: Zoom Factor
    // TODO: Restore Docker settings.

	CRect rc;

    const int tempTileWidth = tileWidth; // * zoomFactor
	rc.right = tempTileWidth + windowMetrics.GetControlDimensions().X_SCROLLBAR;
	AdjustWindowRectEx(&rc, 0, FALSE, roadSelectorDocker->GetDockClient().GetExStyle());
	const int newWidth = abs(rc.right - rc.left);
	roadSelectorDocker->SetDockSize(newWidth);   

    windowDims.right -= newWidth;

    // In addition to the Road Selector, we will also make it so the Game Map takes up
    // 50% of the space on start up.

    const int mapSize = static_cast<int>(windowDims.right * 0.5);
    gameMapDocker->SetDockSize(mapSize);

    windowDims.right -= mapSize;

    // Set the font to the font specified within window metrics.

    HFONT dialogFont = windowMetrics.GetCurrentFont();
    EnumChildWindows(*this, reinterpret_cast<WNDENUMPROC>(SetProperFont), (LPARAM)dialogFont);


    return retVal;
}

//-----------------------------------------------------------------------------
// Function Overrides
//-----------------------------------------------------------------------------

///----------------------------------------------------------------------------
/// Create - Set View and then finish creating the frame.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

HWND MainWindowFrame::Create(HWND parent) {
    SetView(*entityView);
    return CDockFrame::Create(parent);
}

///----------------------------------------------------------------------------
/// RecalcDockLayout - Lock updating while the dock layout is being
/// recalculated in addition to the default behavior. For more information see
/// the Win32++ documentation.
///----------------------------------------------------------------------------

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

//=============================================================================
// Private Functions
//=============================================================================

///----------------------------------------------------------------------------
/// CreateMenuBar - Creates the menu bar in its default state.
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
/// updateTitleBar - Updates the Window's Caption
///----------------------------------------------------------------------------

void MainWindowFrame::updateTitleBar(const bool changeMadeOnly) {

    if(!changeMadeOnly) {

        LanguageMapper& langMap = LanguageMapper::getInstance();

        std::string windowTitle = gameWorldController->getGameMap()->getGameInfo().getGameName();
        windowTitle.append(" - ");
        windowTitle.append(langMap.get("MainWindowTitle"));
        windowTitle.append(" (");
        windowTitle.append(GIT_VERSION_INFO);
        windowTitle.append(")");

        originalWindowTitle = AtoW(windowTitle.c_str(), CP_UTF8);

    }
    
    if(gameWorldController->hasUnsavedChanges()) {
        CString unsavedTitle = originalWindowTitle + "*";
        SetWindowText(unsavedTitle);
    }
    else {
        SetWindowText(originalWindowTitle);
    }

}