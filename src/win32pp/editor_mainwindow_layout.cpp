#include "editor_mainwindow.h"
#include "shared_functions.h"
#include "../util/languagemapper.h"
#include "../gitinfo.h"
#include "../resources/resource.h"

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

    // Prevent Win32++ from using themes, and other stuff we don't need.
    UseThemes(FALSE);
    UseIndicatorStatus(FALSE);
    UseMenuStatus(FALSE);
    UseReBar(FALSE);
    UseToolBar(FALSE);
    UseOwnerDrawnMenu(FALSE);

    if(!loadTileSet()) {
        MessageBox(_T("Could not find tileset.bmp. Please ensure this file is in the same directory as advedit.exe. The program will now close."), _T("Missing file tileset.bmp"), MB_OK | MB_ICONERROR);
        Close();
        return 1;
    }   

	const int retVal = CDockFrame::OnCreate(cs);

    // Deal with the menu bar and title bar next.

    addKeyboardAccelerators();
    CreateMenuBar();
    updateTitleBar(false);

    CRect windowDims = GetClientRect();

	// Create our Dockers

	DWORD styleFlags = DS_NO_UNDOCK | DS_NO_CAPTION | DS_DEFAULT_CURSORS | DS_CLIENTEDGE;
	SetDockStyle(styleFlags);

    // Game map and road selector need to be aware of the tileset.

    gameMapDocker = static_cast<GameMapDocker*>(AddDockedChild(new GameMapDocker(this, gameWorldController, &tilesetBMP), 
                                                styleFlags | DS_DOCKED_LEFT, 128));

    gameMapPanel = &(reinterpret_cast<GameMapPanel&>(gameMapDocker->GetView()));
    gameMapPanel->setTileset(tilesetBMP);


	roadSelectorDocker = static_cast<RoadSelectorDocker*>(gameMapDocker->AddDockedChild(
                                                          new RoadSelectorDocker(this, gameWorldController),
                                                          styleFlags | DS_DOCKED_LEFT | DS_NO_RESIZE, 0));
    
                                                          
    roadPalettePanel = &(reinterpret_cast<RoadPalettePanel&>(roadSelectorDocker->GetView()));
    roadPalettePanel->setTileset(tilesetBMP);

	entitiesHereDocker = static_cast<EntitiesHereDocker*>(gameMapDocker->AddDockedChild(
                                                          new EntitiesHereDocker(&windowMetrics), styleFlags | DS_DOCKED_BOTTOM, 128));

    entitiesHerePanel = &(reinterpret_cast<EntitiesHerePanel&>(entitiesHereDocker->GetView()));

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

    // TODO: The icon will need to change based on the DPI. Windows 9x/2000/XP 
    // got it right and will use the nearest size, but Vista+ just scales it.
    SetIcon(GetApp()->LoadIcon(IDI_APPICON), false);
    SetIcon(GetApp()->LoadIcon(IDI_APPICON), true);

    // TODO: Move the update to it's proper location when the rest of the code
    // is restructured.
    // Update Strings based on language
    updateControlCaptions();

    updateStatusbar(0);

    return retVal;
}

//-----------------------------------------------------------------------------
// Win32++ Function Overrides
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
/// addKeyboardAccelerators - Creates Keyboard accelerators for the menu items
/// and other useful functions
///----------------------------------------------------------------------------

#define ADD_KEYBOARD_ACCEL(MENU_CMD, MODIFIER_FLAGS, VIRT_CODE) \
    hotKey.cmd = MENU_CMD; \
    hotKey.fVirt = FVIRTKEY | MODIFIER_FLAGS; \
    hotKey.key = VIRT_CODE; \
    keyboardAccelerators.push_back(hotKey);

void MainWindowFrame::addKeyboardAccelerators() {
    
    ACCEL hotKey;

    ADD_KEYBOARD_ACCEL(MenuIDs::NewFile, FCONTROL, 'N');
    ADD_KEYBOARD_ACCEL(MenuIDs::OpenFile, FCONTROL, 'O');
    ADD_KEYBOARD_ACCEL(MenuIDs::SaveFile, FCONTROL, 'S');

    ADD_KEYBOARD_ACCEL(MenuIDs::HelpMenuItem, 0, VK_F1);

    // Now to try and add the accelerators

    accelHandle = CreateAcceleratorTable(&keyboardAccelerators[0], keyboardAccelerators.size());

    if(accelHandle) {
        GetApp()->SetAccelerators(accelHandle, *this);
    }
    else {
        
        LanguageMapper& langMap = LanguageMapper::getInstance();

        displayErrorMessage(langMap.get("ErrCreatingAccelsText"),
                            langMap.get("ErrCreatingAccelsTitle"));

    }

}

#undef ADD_KEYBOARD_ACCEL

///----------------------------------------------------------------------------
/// CreateMenuBar - Creates the menu bar in its default state.
///----------------------------------------------------------------------------

void MainWindowFrame::CreateMenuBar() {

    mainMenu.CreateMenu();
    fileMenu.CreatePopupMenu();

    tileMenu.CreatePopupMenu();    
    featureMenu.CreatePopupMenu();
    straightAwayMenu.CreatePopupMenu();
    cornerMenu.CreatePopupMenu();
    deadendMenu.CreatePopupMenu();
    crossroadMenu.CreatePopupMenu();

    worldMenu.CreatePopupMenu();
    zoomMenu.CreatePopupMenu();
    languageMenu.CreatePopupMenu();
    helpMenu.CreatePopupMenu();

    // File Menu
    
    for (int i = MenuIDs::NewFile; i <= MenuIDs::ExitItem; ++i) {

        if(i == MenuIDs::FileMenuDiv1) {
            fileMenu.AppendMenu(MF_SEPARATOR);
            continue;
        }
        
        fileMenu.AppendMenu(MF_STRING, i);
        fileMenu.EnableMenuItem(i, MF_ENABLED);
    }

    // World Menu

    for(int k = MenuIDs::SummaryAndStory; k <= MenuIDs::ResizeWorld; ++k) {
        worldMenu.AppendMenu(MF_STRING, k);
        worldMenu.EnableMenuItem(k, MF_ENABLED);
    }

    // Tile Menu

    appendPopupMenuWithID(tileMenu, featureMenu, MenuIDs::FeatureSubMenu, true);
    tileMenu.AppendMenu(MF_STRING, MenuIDs::EditDescription);
    tileMenu.EnableMenuItem(MenuIDs::EditDescription, MF_ENABLED);

    // Feature Menu
    
    for(int j = MenuIDs::AddStart; j <= MenuIDs::AddBarrierWest; ++j) {
        straightAwayMenu.AppendMenu(MF_STRING, j);
        straightAwayMenu.EnableMenuItem(j, MF_ENABLED);
    }

    for(int l = MenuIDs::AddOnSwitch; l <= MenuIDs::AddOffSwitch; ++l) {
        cornerMenu.AppendMenu(MF_STRING, l);
        cornerMenu.EnableMenuItem(l, MF_ENABLED);
    }

    deadendMenu.AppendMenu(MF_STRING, MenuIDs::AddJumpPad);
    deadendMenu.EnableMenuItem(MenuIDs::AddJumpPad, MF_ENABLED);

    for(int i = MenuIDs::AddHazard; i <= MenuIDs::AddSafeHaven; ++i) {
        crossroadMenu.AppendMenu(MF_STRING, i);
        crossroadMenu.EnableMenuItem(i, MF_ENABLED);
    }

    appendPopupMenuWithID(featureMenu, straightAwayMenu, MenuIDs::StraightAwayMenu, false);
    appendPopupMenuWithID(featureMenu, cornerMenu, MenuIDs::CornerMenu, false);
    appendPopupMenuWithID(featureMenu, deadendMenu, MenuIDs::DeadEndMenu, false);
    appendPopupMenuWithID(featureMenu, crossroadMenu, MenuIDs::CrossroadsMenu, false);

    // The rest of the feature menu

    for(int k = MenuIDs::FeatureMenuDiv1; k <= MenuIDs::LAST_OF_MAP_ALTER_IDS; ++k) {

        if (k == MenuIDs::FeatureMenuDiv1 || k == MenuIDs::FeatureMenuDiv2) {
            featureMenu.AppendMenu(MF_SEPARATOR);
            continue;
        }

        featureMenu.AppendMenu(MF_STRING, k);
        featureMenu.EnableMenuItem(k, MF_ENABLED);
    }


    // Zoom Menu

    for(int j = MenuIDs::Zoom1xItem; j <= MenuIDs::Zoom4xItem; ++j) {
        zoomMenu.AppendMenu(MF_STRING, j);
        zoomMenu.EnableMenuItem(j, MF_ENABLED);
    }

    // Language Menu (Though at some point this might be merged into a settings dialog

    languageMenu.AppendMenu(MF_STRING, MenuIDs::DefaultLangMenuItem);
    languageMenu.EnableMenuItem(MenuIDs::DefaultLangMenuItem, MF_ENABLED);
    
    LanguageMapper& langMap = LanguageMapper::getInstance();

    const std::vector<LanguagePack>& langPacks = langMap.getLanguagePacks();

    const size_t numLangs = langPacks.size() > 20 ? 20 : langPacks.size();

    if(numLangs != (size_t)-1) {
        languageMenu.AppendMenu(MF_SEPARATOR, 1);

        for(size_t p = 0; p < numLangs; ++p) {
            languageMenu.AppendMenu(MF_STRING, MenuIDs::LanguageDiv1 + p + 1);
            languageMenu.EnableMenuItem(MenuIDs::LanguageDiv1 + p + 1, MF_ENABLED);

            languageMenu.ModifyMenu(MenuIDs::LanguageDiv1 + p + 1, MF_ENABLED, MenuIDs::LanguageDiv1 + p + 1, AtoT(langPacks[p].displayName.c_str(), CP_UTF8));

        }
    }
    
    // Help Menu

    for(int l = MenuIDs::HelpMenuItem; l <= MenuIDs::AboutMenuItem; ++l) {

        if (l == MenuIDs::HelpMenuDiv1) {
            helpMenu.AppendMenu(MF_SEPARATOR);
            continue;
        }

        helpMenu.AppendMenu(MF_STRING, l);
        helpMenu.EnableMenuItem(l, MF_ENABLED);

    }

    // Finally deal with the menu bar

    appendPopupMenuWithID(mainMenu, fileMenu, MenuIDs::FilePopupMenu, true);
    appendPopupMenuWithID(mainMenu, tileMenu, MenuIDs::TilePopupMenu, false);
    appendPopupMenuWithID(mainMenu, worldMenu, MenuIDs::WorldPopupMenu, false);
    appendPopupMenuWithID(mainMenu, zoomMenu, MenuIDs::ZoomMenu, true);
    appendPopupMenuWithID(mainMenu, languageMenu, MenuIDs::LanguageMenu, true);
    appendPopupMenuWithID(mainMenu, helpMenu, MenuIDs::HelpMenu, true);

    SetMenu(mainMenu);

}

///----------------------------------------------------------------------------
/// updateControlCaptions - Updates the controls on the Window
///----------------------------------------------------------------------------

#define CHANGE_MENU_STRING(MENU, ITEMID, LANGID) MENU.ModifyMenu(ITEMID, MENU.GetMenuState(ITEMID, 0) & (MF_GRAYED | MF_DISABLED), ITEMID, LM_toUTF8(LANGID, langMap))
#define CHANGE_MENU_STRING_ACCEL(MENU, ITEMID, LANGID, ACCELKEY) MENU.ModifyMenu(ITEMID, MENU.GetMenuState(ITEMID, 0) & (MF_GRAYED | MF_DISABLED), ITEMID, LM_toUTF8(LANGID, langMap) + "\t" + ACCELKEY)

void MainWindowFrame::updateControlCaptions() {

    LanguageMapper& langMap = LanguageMapper::getInstance();

    // TODO: Make the keyboard accelerator and option, and obtain which one
    // it is.

    // Main Menu Bar

    CHANGE_MENU_STRING(mainMenu, MenuIDs::FilePopupMenu, "FileMenu");
    CHANGE_MENU_STRING(mainMenu, MenuIDs::TilePopupMenu, "TileMenu");
    CHANGE_MENU_STRING(mainMenu, MenuIDs::WorldPopupMenu, "WorldMenu");
    CHANGE_MENU_STRING(mainMenu, MenuIDs::LanguageMenu, "LanguageMenu");
    CHANGE_MENU_STRING(mainMenu, MenuIDs::ZoomMenu, "ZoomMenu");
    CHANGE_MENU_STRING(mainMenu, MenuIDs::HelpMenu, "HelpMenu");

    // File Menu

    CHANGE_MENU_STRING_ACCEL(fileMenu, MenuIDs::NewFile, "NewMenuItem", "Ctrl+N");
    CHANGE_MENU_STRING_ACCEL(fileMenu, MenuIDs::OpenFile, "OpenMenuItem", "Ctrl+O");
    CHANGE_MENU_STRING(fileMenu, MenuIDs::SaveFile, "SaveMenuItem");
    CHANGE_MENU_STRING(fileMenu, MenuIDs::SaveFileAs, "SaveAsMenuItem");
    CHANGE_MENU_STRING_ACCEL(fileMenu, MenuIDs::ExitItem, "ExitMenuItem", "Alt+F4");

    // World Menu

    CHANGE_MENU_STRING(worldMenu, MenuIDs::SummaryAndStory, "SummaryMenuItem");
    CHANGE_MENU_STRING(worldMenu, MenuIDs::WorldProperties, "WorldPropertiesMenuItem");
    CHANGE_MENU_STRING(worldMenu, MenuIDs::ResizeWorld, "ResizeWorldMenuItem");

    // Tile Menu

    CHANGE_MENU_STRING(tileMenu, MenuIDs::FeatureSubMenu, "AddFeatureMenuItem");
    CHANGE_MENU_STRING(tileMenu, MenuIDs::EditDescription, "TileDescriptionMenuItem");

    // Add Feature Menu

    CHANGE_MENU_STRING(featureMenu, MenuIDs::StraightAwayMenu, "StraightAwayMenuItem");
    CHANGE_MENU_STRING(featureMenu, MenuIDs::CornerMenu, "CornerMenuItem");
    CHANGE_MENU_STRING(featureMenu, MenuIDs::DeadEndMenu, "DeadEndMenuItem");
    CHANGE_MENU_STRING(featureMenu, MenuIDs::CrossroadsMenu, "CrossroadsMenuItem");

    CHANGE_MENU_STRING(featureMenu, MenuIDs::StartJumpConnection, "JumppadConnectFirst");
    CHANGE_MENU_STRING(featureMenu, MenuIDs::EndJumpConnection, "JumppadConnectSecond");
    CHANGE_MENU_STRING(featureMenu, MenuIDs::StartSwitchConnection, "StartSwitchMenuItem");
    CHANGE_MENU_STRING(featureMenu, MenuIDs::EndSwitchConnection, "EndSwitchMenuItem");
    CHANGE_MENU_STRING(featureMenu, MenuIDs::StopJumpConnection, "StopMakingJumpMenuItem");
    CHANGE_MENU_STRING(featureMenu, MenuIDs::StopSwitchConnection, "StopMakingSwitchMenuItem");

    CHANGE_MENU_STRING(featureMenu, MenuIDs::ToggleTileDarkness, "ToggleDarknessMenuItem");
    CHANGE_MENU_STRING(featureMenu, MenuIDs::ToggleSwitchState, "ToggleSwitchStateMenuItem");

    // Straight Away Menu

    CHANGE_MENU_STRING(straightAwayMenu, MenuIDs::AddStart, "StartMenuItem");
    CHANGE_MENU_STRING(straightAwayMenu, MenuIDs::AddFinish, "FinishMenuItem");
    CHANGE_MENU_STRING(straightAwayMenu, MenuIDs::AddGate, "GateMenuItem");
    CHANGE_MENU_STRING(straightAwayMenu, MenuIDs::AddLockedDoor, "LockedDoorMenuItem");
    CHANGE_MENU_STRING(straightAwayMenu, MenuIDs::AddBarrierNorth, "NorthBarrMenuItem");
    CHANGE_MENU_STRING(straightAwayMenu, MenuIDs::AddBarrierSouth, "SouthBarrMenuItem");
    CHANGE_MENU_STRING(straightAwayMenu, MenuIDs::AddBarrierEast, "WestBarrMenuItem");
    CHANGE_MENU_STRING(straightAwayMenu, MenuIDs::AddBarrierWest, "EastBarrMenuItem");

    // Corner Menu

    CHANGE_MENU_STRING(cornerMenu, MenuIDs::AddOnSwitch, "SwitchOnMenuItem");
    CHANGE_MENU_STRING(cornerMenu, MenuIDs::AddOffSwitch, "SwitchOffMenuItem");

    // Dead-end Menu

    CHANGE_MENU_STRING(deadendMenu, MenuIDs::AddJumpPad, "JumppadMenuItem");

    // Crossroads Menu

    CHANGE_MENU_STRING(crossroadMenu, MenuIDs::AddHazard, "HazardMenuItem");
    CHANGE_MENU_STRING(crossroadMenu, MenuIDs::AddSafeHaven, "SafeHavenMenuItem");

    // Language Menu

    // TODO: Fix this later
    CHANGE_MENU_STRING(languageMenu, MenuIDs::DefaultLangMenuItem, "HazardMenuItem");

    // Zoom Menu

    CHANGE_MENU_STRING(zoomMenu, MenuIDs::Zoom1xItem, "1xMenuItem");
    CHANGE_MENU_STRING(zoomMenu, MenuIDs::Zoom2xItem, "2xMenuItem");
    CHANGE_MENU_STRING(zoomMenu, MenuIDs::Zoom3xItem, "3xMenuItem");
    CHANGE_MENU_STRING(zoomMenu, MenuIDs::Zoom4xItem, "4xMenuItem");

    // Help Menu

    CHANGE_MENU_STRING(helpMenu, MenuIDs::HelpMenuItem, "HelpMenuItem");
    CHANGE_MENU_STRING(helpMenu, MenuIDs::AboutMenuItem, "AboutMenuItem");
    

}

#undef CHANGE_MENU_STRING_ACCEL
#undef CHANGE_MENU_STRING

///----------------------------------------------------------------------------
/// updateTitleBar - Updates the Window's Caption
///----------------------------------------------------------------------------

void MainWindowFrame::updateTitleBar(const bool changeMadeOnly) {

    if(!changeMadeOnly) {

        LanguageMapper& langMap = LanguageMapper::getInstance();
        const GameMap* gameMap = gameWorldController->getGameMap();

        std::string windowTitle = "";

        if(gameMap) {

            windowTitle = gameMap->getGameInfo().getGameName();
            windowTitle.append(" - ");

        }

        windowTitle.append(langMap.get("ProgramName"));
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

void MainWindowFrame::appendPopupMenuWithID(CMenu& targetMenu, CMenu& popupMenu, const UINT& id, const bool& enabled) {

    MENUITEMINFO mii;
    ZeroMemory(&mii, sizeof(MENUITEMINFO));
    mii.cbSize = sizeof(MENUITEMINFO);
    mii.fMask = MIIM_ID;
    mii.wID = id;

    targetMenu.AppendMenu(MF_STRING | MF_POPUP, reinterpret_cast<UINT_PTR>(popupMenu.GetHandle()));
    const int menuPos = targetMenu.GetMenuItemCount() - 1;
    targetMenu.SetMenuItemInfo(menuPos, mii, TRUE);
    targetMenu.EnableMenuItem(id, enabled ? MF_ENABLED : MF_GRAYED | MF_DISABLED); // Otherwise GetMenuState fails.

}