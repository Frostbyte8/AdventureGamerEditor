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
    worldMenu.CreatePopupMenu();

    tileMenu.CreatePopupMenu();
    
    featureMenu.CreatePopupMenu();

    straightAwayMenu.CreatePopupMenu();
    cornerMenu.CreatePopupMenu();
    deadendMenu.CreatePopupMenu();
    crossroadMenu.CreatePopupMenu();

    LanguageMapper& langMap = LanguageMapper::getInstance();

    
    for (int i = MenuIDs::NewFile; i <= MenuIDs::ExitItem; ++i) {

        if(i == MenuIDs::FileMenuDiv1) {
            fileMenu.AppendMenu(MF_SEPARATOR);
            continue;
        }
        
        fileMenu.AppendMenu(MF_STRING, i);
    }

    for(int k = MenuIDs::SummaryAndStory; k <= MenuIDs::ResizeWorld; ++k) {
        worldMenu.AppendMenu(MF_STRING, k);
    }

    featureMenu.AppendMenu(MF_STRING | MF_POPUP, reinterpret_cast<UINT_PTR>(straightAwayMenu.GetHandle()), LM_toUTF8("StraightAwayMenuItem", langMap));
    featureMenu.AppendMenu(MF_STRING | MF_POPUP, reinterpret_cast<UINT_PTR>(cornerMenu.GetHandle()), LM_toUTF8("CornerMenuItem", langMap));
    featureMenu.AppendMenu(MF_STRING | MF_POPUP, reinterpret_cast<UINT_PTR>(deadendMenu.GetHandle()), LM_toUTF8("DeadEndMenuItem", langMap));
    featureMenu.AppendMenu(MF_STRING | MF_POPUP, reinterpret_cast<UINT_PTR>(crossroadMenu.GetHandle()), LM_toUTF8("CrossroadsMenuItem", langMap));
    
    appendPopupMenuWithID(tileMenu, featureMenu, MenuIDs::FeatureSubMenu);
    tileMenu.AppendMenu(MF_STRING, MenuIDs::EditDescription);

    updateControlCaptions();

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

    caption = AtoW(langMap.get("TileMenu").c_str(), CP_UTF8);

    mainMenu.AppendMenu(MF_STRING | MF_POPUP,
                        reinterpret_cast<UINT_PTR>(tileMenu.GetHandle()), caption);

    caption = AtoW(langMap.get("WorldMenu").c_str(), CP_UTF8);
    
    mainMenu.AppendMenu(MF_STRING | MF_POPUP,
                        reinterpret_cast<UINT_PTR>(worldMenu.GetHandle()), caption);

    SetFrameMenu(mainMenu);

}

///----------------------------------------------------------------------------
/// updateControlCaptions - Updates the controls on the Window
///----------------------------------------------------------------------------

#define CHANGE_MENU_STRING(MENU, ITEMID, LANGID) MENU.ModifyMenu(ITEMID, MF_BYCOMMAND | MF_STRING, 0, LM_toUTF8(LANGID, langMap))

void MainWindowFrame::updateControlCaptions() {

    LanguageMapper& langMap = LanguageMapper::getInstance();

    CHANGE_MENU_STRING(fileMenu, MenuIDs::NewFile, "NewMenuItem");
    CHANGE_MENU_STRING(fileMenu, MenuIDs::OpenFile, "OpenMenuItem");
    CHANGE_MENU_STRING(fileMenu, MenuIDs::SaveFile, "SaveMenuItem");
    CHANGE_MENU_STRING(fileMenu, MenuIDs::SaveFileAs, "SaveAsMenuItem");
    CHANGE_MENU_STRING(fileMenu, MenuIDs::ExitItem, "ExitMenuItem");

    CHANGE_MENU_STRING(worldMenu, MenuIDs::SummaryAndStory, "SummaryMenuItem");
    CHANGE_MENU_STRING(worldMenu, MenuIDs::WorldProperties, "WorldPropertiesMenuItem");
    CHANGE_MENU_STRING(worldMenu, MenuIDs::ResizeWorld, "ResizeWorldMenuItem");

    CHANGE_MENU_STRING(tileMenu, MenuIDs::FeatureSubMenu, "AddFeatureMenuItem");
    CHANGE_MENU_STRING(tileMenu, MenuIDs::EditDescription, "TileDescriptionMenuItem");
    

    //worldMenu.AppendMenu(MF_STRING | MF_POPUP, reinterpret_cast<UINT_PTR>(featureMenu.GetHandle()));

}

#undef CHANGE_MENU_STRING

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

///----------------------------------------------------------------------------
/// appendPopupMenuWithID - Appends a Popup menu WITH an ID
/// @param a reference to a CMenu object that will have the pop up added to it
/// @param a reference to the CMenu to be added and IDed
/// @param a reference to the ID to apply to the menu item.
///----------------------------------------------------------------------------

void MainWindowFrame::appendPopupMenuWithID(CMenu& targetMenu, CMenu& popupMenu, const UINT& id) {

    MENUITEMINFO mii;
    ZeroMemory(&mii, sizeof(MENUITEMINFO));
    mii.cbSize = sizeof(MENUITEMINFO);
    mii.fMask = MIIM_ID;
    mii.wID = id;

    targetMenu.AppendMenu(MF_STRING | MF_POPUP, reinterpret_cast<UINT_PTR>(popupMenu.GetHandle()));
    const int menuPos = targetMenu.GetMenuItemCount() - 1;
    targetMenu.SetMenuItemInfo(menuPos, mii, TRUE);


}