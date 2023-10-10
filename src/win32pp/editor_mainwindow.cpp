#include "editor_mainwindow.h"
#include "../editor_constants.h"
#include "../model/gametile.h"
#include <vector>
#include "../util/languagemapper.h"
#include <wxx_commondlg.h>

#include "shared_functions.h"

namespace MenuIDs {
    const WORD LongDescription      = 201;
    const WORD SummaryAndStory      = 202;
    const WORD WorldProperties      = 203;
    const WORD NewFile              = 204;
    const WORD OpenFile             = 205;
    const WORD SaveFile             = 206;
}

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
editTileDescriptionDialog(0), tileWidth(0), tileHeight(0), selectedTileIndex(0) {
    gameWorldController = new GameWorldController(this);
	entityView = new GameEntitiesView(this, &windowMetrics);
    LanguageMapper::getInstance();
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

    LanguageMapper& langMap = LanguageMapper::getInstance();

    fileMenu.AppendMenu(MF_STRING, MenuIDs::NewFile, LM_toUTF8("NewMenuItem", langMap));
    fileMenu.AppendMenu(MF_STRING, MenuIDs::OpenFile, LM_toUTF8("OpenMenuItem", langMap));
    fileMenu.AppendMenu(MF_STRING, MenuIDs::SaveFile, LM_toUTF8("SaveMenuItem", langMap));
    fileMenu.AppendMenu(MF_STRING, 0, LM_toUTF8("SaveAsMenuItem", langMap));
    fileMenu.AppendMenu(MF_STRING, 0, LM_toUTF8("ExitMenuItem", langMap));

    editMenu.AppendMenu(MF_STRING, MenuIDs::LongDescription, LM_toUTF8("LongTileMenuItem", langMap));
    editMenu.AppendMenu(MF_STRING, MenuIDs::SummaryAndStory, LM_toUTF8("SummaryMenuItem", langMap));
    editMenu.AppendMenu(MF_STRING, MenuIDs::WorldProperties, LM_toUTF8("WorldMenuItem", langMap));

    CString caption = AtoW(langMap.get("FileMenu").c_str(), CP_UTF8);

    mainMenu.AppendMenu(MF_STRING | MF_POPUP,
                        reinterpret_cast<UINT_PTR>(fileMenu.GetHandle()), caption);

    caption = AtoW(langMap.get("EditMenu").c_str(), CP_UTF8);

    mainMenu.AppendMenu(MF_STRING | MF_POPUP,
                        reinterpret_cast<UINT_PTR>(editMenu.GetHandle()), caption);

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
	
	reinterpret_cast<GameMapView&>(gameMapDocker->GetView()).setTileset(tilesetBMP);


	roadSelectorDocker = static_cast<RoadSelectorDocker*>(gameMapDocker->AddDockedChild(
                                                          new RoadSelectorDocker(this),
                                                          styleFlags | DS_DOCKED_LEFT | DS_NO_RESIZE, 0));
                                                          
	reinterpret_cast<RoadSelectorView&>(roadSelectorDocker->GetView()).SetTileset(tilesetBMP);

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

    //switch(msg) {
    //    case WM_ERASEBKGND: return TRUE;
    //}

    switch (msg) {
        case WM_ACTIVATEAPP:
            ::BringWindowToTop(activeWindowHandle);
            return 0;
    }


	return WndProcDefault(msg, wParam, lParam);
}

///----------------------------------------------------------------------------
/// OnCommand - Processes WM_COMMAND events
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

BOOL MainWindowFrame::OnCommand(WPARAM wParam, LPARAM) {
    
    switch (LOWORD(wParam)) {

        // TODO: On New and Open need to be interface functions
        case MenuIDs::NewFile: return OnFileNew();
        case MenuIDs::OpenFile: return OnFileOpen();
        case MenuIDs::SaveFile:
            gameWorldController->saveWorld("D:\\Dump\\Adv\\debug\\", "DEBUG.SG0");
            return 0;
            break;
        case MenuIDs::SummaryAndStory: onEditStory(); break;
        case MenuIDs::LongDescription: onEditTileDescription(); break;
        case MenuIDs::WorldProperties: onEditWorldInfo(); break;

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
    reinterpret_cast<GameMapView&>(gameMapDocker->GetView()).UpdateBackBuffer();
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
        reinterpret_cast<GameMapView&>(gameMapDocker->GetView()).UpdateBackBuffer();
        entityView->updateLists(gameWorldController->getGameMap()->getGameObjects(), gameWorldController->getGameMap()->getGameCharacters());
	}

    return TRUE;
}

///----------------------------------------------------------------------------
/// loadTileset - Load's the tileset BMP
/// @return true if it loaded succesfully, false if it did not.
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

//=============================================================================
// Public Interface Functions
// Refer to the interface header file for more information.
//=============================================================================

//-----------------------------------------------------------------------------
// askYesNoQuestion
//-----------------------------------------------------------------------------

int MainWindowFrame::askYesNoQuestion(const std::string& inQuestion, const std::string& inTitle,
                                      bool allowCancel) {

    const CString question      = AtoW(inQuestion.c_str(), CP_UTF8);
    const CString title         = AtoW(inTitle.c_str(), CP_UTF8);
    const UINT messageBoxFlags  = MB_ICONQUESTION | (allowCancel ? MB_YESNOCANCEL : MB_YESNO);

    const int retVal = MessageBox(question, title, messageBoxFlags);

    if (retVal == IDYES) {
        return GenericInterfaceResponses::Yes;
    }

    return GenericInterfaceResponses::No;
}

//-----------------------------------------------------------------------------
// displayErrorMessage
//-----------------------------------------------------------------------------

void MainWindowFrame::displayErrorMessage(const std::string& inMessage,
                                          const std::string& inTitle) {

    const CString message       = AtoW(inMessage.c_str(), CP_UTF8);
    const CString title         = AtoW(inTitle.c_str(), CP_UTF8);
    const UINT messageBoxFlags  = MB_ICONERROR;

    MessageBox(message, title, messageBoxFlags);

}

//-----------------------------------------------------------------------------
// getTileToDrawWith
//-----------------------------------------------------------------------------

int MainWindowFrame::getTileToDrawWith() {
    return reinterpret_cast<RoadSelectorView&>(roadSelectorDocker->GetView()).getSelectedTile();
}

//-----------------------------------------------------------------------------
// onSelectedTileChanged
//-----------------------------------------------------------------------------

bool MainWindowFrame::onSelectedTileChanged(const int& row, const int& col) {
    
    const GameMap* gameMap = gameWorldController->getGameMap();

    if (!gameMap->isRowColInMapBounds(row, col)) {
        return false;
    }

    const int newTileIndex = gameMap->indexFromRowCol(row, col);

    if (newTileIndex == selectedTileIndex) {
        return false;
    }

    selectedTileIndex = newTileIndex;

    const std::vector<GameObject>& objectVec = gameWorldController->getGameMap()->getGameObjectsAtRowCol(row, col);
    const std::vector<GameCharacter>& charVec = gameWorldController->getGameMap()->getGameCharactersAtRowCol(row, col);
    reinterpret_cast<EntitiesHereView&>(entitiesHereDocker->GetView()).updateLists(objectVec, charVec);
    return true;
}

//-----------------------------------------------------------------------------
// onAlterObject
//-----------------------------------------------------------------------------

void MainWindowFrame::onAlterObject(const int& alterType, const size_t& index) {

    if(editObjectDialog || activeWindowHandle != GetHwnd()) {
        return;
    }

    const std::vector<GameObject>& gameObjects = gameWorldController->getGameMap()->getGameObjects();
    const bool editingObject = (alterType == AlterType::Edit) ? true : false;

    LanguageMapper& langMap = LanguageMapper::getInstance();

    if(editingObject) {
        
        if(gameObjects.empty() || index > gameObjects.size() - 1) {
            displayErrorMessage(langMap.get("ErrInvalidObjIndexText"), langMap.get("ErrInvalidObjIndexTitle"));
            return;
        }
    }
    else {
        // TODO: Prevent the editor from getting here outside the controller so this check isn't done twice.
        if(gameWorldController->canAddObject() == false) {
            displayErrorMessage(langMap.get("ErrObjLimitReachedText"), langMap.get("ErrObjLimitReachedTitle"));
            return;
        }
    }

    editObjectDialog = new (std::nothrow) EditObjectDialog(this, gameWorldController->getGameMap(), GetHwnd(), editingObject);

    if(!editObjectDialog) {
        displayErrorMessage(langMap.get("ErrAllocatingDialogText"), langMap.get("ErrAllocatingDialogTitle"));
        return;
    }

    editObjectDialog->Create(GetHwnd(), WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT, WS_POPUPWINDOW | WS_DLGFRAME);

    if(!editObjectDialog->IsWindow()) {
        displayErrorMessage(langMap.get("ErrCreatingDialogText"), langMap.get("ErrCreatingDialogTitle"));
        delete editObjectDialog;
        return;
    }

    editObjectDialog->SetExStyle(editObjectDialog->GetExStyle() | WS_EX_DLGMODALFRAME);
    
    activeWindowHandle = editObjectDialog->GetHwnd();


    CString caption;

    if(alterType == AlterType::Add) {
        GameObject::Builder bd;
        editObjectDialog->setObjectToEdit(bd.build());
        CString caption = LM_toUTF8("CreateObjectTitle", langMap);
        editObjectDialog->setDefaultDialogTitle(caption);
        
    }
    else if (alterType == AlterType::Edit) {
        // TODO: make sure the character exists before doing this.
        const GameObject& gameObject = gameWorldController->getGameMap()->getGameObjects().at(index);
        GameObject::Builder objectToEdit(gameObject);
        editObjectDialog->setObjectToEdit(objectToEdit.build());
        caption = LM_toUTF8("EditObjectTitle", langMap);       
        caption.Format(caption, AtoW(gameObject.getName().c_str(), CP_UTF8).c_str());
        editObjectDialog->setDefaultDialogTitle(caption);
    }

    // Dialog is ready to go modal
    editObjectDialog->goModal();
    centerWindowOnCurrentMonitor(MonitorFromWindow(GetHwnd(), 0), reinterpret_cast<CWnd&>(*editObjectDialog));
    editObjectDialog->ShowWindow(SW_SHOW);

}

//-----------------------------------------------------------------------------
// onAlterCharacter
//-----------------------------------------------------------------------------

void MainWindowFrame::onAlterCharacter(const int& alterType, const size_t& index) {

    // Make sure that the dialog isn't already running, or that
    // another modal window isn't already running.

    if(editCharacterDialog || activeWindowHandle != GetHwnd()) {
        return;
    }

    LanguageMapper& langMap = LanguageMapper::getInstance();

    const std::vector<GameCharacter>& gameCharacters = gameWorldController->getGameMap()->getGameCharacters();
    const bool editingChar = (alterType == AlterType::Edit) ? true : false;

    if(editingChar) {
        
        if(gameCharacters.empty() || index > gameCharacters.size() - 1) {
            displayErrorMessage("Invalid character index", "error");
            return;
        }
    }
    else {
        if(gameWorldController->canAddCharacter() == false) {
            displayErrorMessage(langMap.get("ErrCharLimitReachedText"), langMap.get("ErrCharLimitReachedTitle"));
            return;
        }
    }


    editCharacterDialog = new EditCharacterDialog(this, gameWorldController->getGameMap(), GetHwnd(), editingChar);
    editCharacterDialog->Create(GetHwnd(), WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT, WS_POPUPWINDOW | WS_DLGFRAME);

    if(!editCharacterDialog->IsWindow()) {
        displayErrorMessage(langMap.get("ErrCreatingDialogText"), langMap.get("ErrCreatingDialogTitle"));
        delete editCharacterDialog;
        return;
    }

    editCharacterDialog->SetExStyle(editCharacterDialog->GetExStyle() | WS_EX_DLGMODALFRAME);
    activeWindowHandle = editCharacterDialog->GetHwnd();

    // TODO: Set Caption

    CString caption;

    if(alterType == AlterType::Add) {
        GameCharacter::Builder bd;
        editCharacterDialog->setCharacterToEdit(bd.build());
        CString caption = LM_toUTF8("CreateCharacterTitle", langMap);
        editCharacterDialog->setDefaultDialogTitle(caption);
    }
    else if(alterType == AlterType::Edit) {

        const GameCharacter& gameCharacter = gameWorldController->getGameMap()->getGameCharacters().at(index);
        caption = LM_toUTF8("EditCharacterTitle", langMap);
		caption.Format(caption, AtoW(gameCharacter.getName().c_str(), CP_UTF8).c_str());
        GameCharacter::Builder charToEdit(gameCharacter);
        editCharacterDialog->setCharacterToEdit(charToEdit.build());
        editCharacterDialog->setDefaultDialogTitle(caption);
    }
    
    editCharacterDialog->goModal();
    centerWindowOnCurrentMonitor(MonitorFromWindow(GetHwnd(), 0), reinterpret_cast<CWnd&>(*editCharacterDialog));
    editCharacterDialog->ShowWindow(SW_SHOW);

}

//-----------------------------------------------------------------------------
// finishedEditCharacterDialog
//-----------------------------------------------------------------------------

void MainWindowFrame::finishedEditCharacterDialog() {

    if(!editCharacterDialog) {
        return;
    }

    const int alterType = editCharacterDialog->isEditingCharacter() ? AlterType::Edit : AlterType::Add;

    if(editCharacterDialog->hasSavedChanges() && (alterType == AlterType::Add || alterType == AlterType::Edit)) {
   
        GameCharacter::Builder bd = editCharacterDialog->getAlteredCharacter();

        if(alterType == AlterType::Add) {
            gameWorldController->tryAddCharacter(bd);
        }
        else {
            gameWorldController->tryReplaceCharacter(bd);
        }

        entityView->updateLists(gameWorldController->getGameMap()->getGameObjects(), gameWorldController->getGameMap()->getGameCharacters());

    }

    delete editCharacterDialog;
    editCharacterDialog = NULL;
    activeWindowHandle = GetHwnd();

}

//-----------------------------------------------------------------------------
// finishedEditObjectDialog
//-----------------------------------------------------------------------------

void MainWindowFrame::finishedEditObjectDialog() {

    if(!editObjectDialog) {
        return;
    }

    const int alterType = editObjectDialog->isEditingObject() ? AlterType::Edit : AlterType::Add; 

    if(editObjectDialog->hasSavedChanges() && (alterType == AlterType::Add || alterType == AlterType::Edit)) {

        GameObject::Builder bd = editObjectDialog->getAlteredObject();

        if(alterType == AlterType::Add) {
            gameWorldController->tryAddObject(bd);
        }
        else {
            gameWorldController->tryReplaceObject(bd);
        }

        entityView->updateLists(gameWorldController->getGameMap()->getGameObjects(), gameWorldController->getGameMap()->getGameCharacters());

    }    

    delete editObjectDialog;
    editObjectDialog = NULL;
    activeWindowHandle = GetHwnd();

}

//-----------------------------------------------------------------------------
// onEditWorldInfo
//-----------------------------------------------------------------------------

void MainWindowFrame::onEditWorldInfo() {

    // Make sure that the dialog isn't already running, or that
    // another modal window isn't already running.

    if(editWorldInfoDialog || activeWindowHandle != GetHwnd()) {
        return;
    }
    
    LanguageMapper& langMap = LanguageMapper::getInstance();

    editWorldInfoDialog = new EditWorldInfoDialog(this, GetHwnd());
    editWorldInfoDialog->Create(GetHwnd(), WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT, WS_POPUPWINDOW | WS_DLGFRAME);

    if(!editWorldInfoDialog->IsWindow()) {
        displayErrorMessage(langMap.get("ErrCreatingDialogText"), langMap.get("ErrCreatingDialogTitle"));
        delete editWorldInfoDialog;
        return;
    }

    editWorldInfoDialog->SetExStyle(editWorldInfoDialog->GetExStyle() | WS_EX_DLGMODALFRAME);
    editWorldInfoDialog->setWorldInfo(gameWorldController->getGameMap()->getGameInfo());

    activeWindowHandle = editWorldInfoDialog->GetHwnd();
    
    editWorldInfoDialog->setDefaultDialogTitle(LM_toUTF8("EditWorldInfoTitle", langMap));
    editWorldInfoDialog->goModal();
    centerWindowOnCurrentMonitor(MonitorFromWindow(GetHwnd(), 0), reinterpret_cast<CWnd&>(*editWorldInfoDialog));
    editWorldInfoDialog->ShowWindow(SW_SHOW);
}

//-----------------------------------------------------------------------------
// finishedEditWorldInfoDialog
//-----------------------------------------------------------------------------

void MainWindowFrame::finishedEditWorldInfoDialog() {

    if(!editWorldInfoDialog) {
        return;
    }
    
    if(editWorldInfoDialog->hasSavedChanges()) {
        gameWorldController->tryUpdateGameInfo(editWorldInfoDialog->getGameInfo());
    }

    delete editWorldInfoDialog;
    editWorldInfoDialog = NULL;
    activeWindowHandle = GetHwnd();

}

//-----------------------------------------------------------------------------
// onEditStory
//-----------------------------------------------------------------------------

void MainWindowFrame::onEditStory() {

    if(editStoryDialog || activeWindowHandle != GetHwnd()) {
        return;
    }

    LanguageMapper& langMap = LanguageMapper::getInstance();

    editStoryDialog = new EditStoryDialog(this, GetHwnd());
    editStoryDialog->Create(GetHwnd(), WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT, WS_POPUPWINDOW | WS_DLGFRAME);

    if(!editStoryDialog->IsWindow()) {
        displayErrorMessage(langMap.get("ErrCreatingDialogText"), langMap.get("ErrCreatingDialogTitle"));
        delete editStoryDialog;
        return;
    }

    editStoryDialog->SetExStyle(editStoryDialog->GetExStyle() | WS_EX_DLGMODALFRAME);
    editStoryDialog->setStoryAndSummary(gameWorldController->getGameMap()->getStory(), gameWorldController->getGameMap()->getSummary());

    activeWindowHandle = editStoryDialog->GetHwnd();

    const CString dialogCaption = LM_toUTF8("EditWorldStoryTitle", langMap);
    editStoryDialog->setDefaultDialogTitle(dialogCaption);

    editStoryDialog->goModal();
    centerWindowOnCurrentMonitor(MonitorFromWindow(GetHwnd(), 0), reinterpret_cast<CWnd&>(*editStoryDialog));
    editStoryDialog->ShowWindow(SW_SHOW);
    
}

//-----------------------------------------------------------------------------
// finishedEditStoryDialog
//-----------------------------------------------------------------------------

void MainWindowFrame::finishedEditStoryDialog() {

    if(!editStoryDialog) {
        return;
    }

    /*
    if(!wasCanceled) {
        gameWorldController->tryUpdateStoryAndSummary(editStoryDialog->getStory(),
                                                      editStoryDialog->getSummary()); 
    }
    */


    //if(!pressedApply) {
        delete editStoryDialog;
        editStoryDialog = NULL;
        activeWindowHandle = GetHwnd();
    //}

}

//-----------------------------------------------------------------------------
// onEditTileDescription
//-----------------------------------------------------------------------------

void MainWindowFrame::onEditTileDescription() {

    if(editTileDescriptionDialog || activeWindowHandle != GetHwnd()) {
        return;
    }

    LanguageMapper& langMap = LanguageMapper::getInstance();

    editTileDescriptionDialog = new EditTileDescriptionDialog(this, GetHwnd());
    editTileDescriptionDialog->Create(GetHwnd(), WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT, WS_POPUPWINDOW | WS_DLGFRAME);

    if(!editTileDescriptionDialog->IsWindow()) {
        displayErrorMessage(langMap.get("ErrCreatingDialogText"), langMap.get("ErrCreatingDialogTitle"));
        delete editTileDescriptionDialog;
        return;
    }

    editTileDescriptionDialog->SetExStyle(editTileDescriptionDialog->GetExStyle() | WS_EX_DLGMODALFRAME);
    const GameTile gt = gameWorldController->getGameMap()->getTile(selectedTileIndex);
    editTileDescriptionDialog->setTileDescription(gt.getName(), gt.getDescription());

    activeWindowHandle = editTileDescriptionDialog->GetHwnd();
    editTileDescriptionDialog->goModal();

    // TODO: Maybe make the dialog title part of the Constructor?
    // Or even move this to OnCreate since that function already needs languagemapper and
    // that's one less "getInstance".
    const CString dialogCaption = LM_toUTF8("EditTileDescTitle", langMap);
    editTileDescriptionDialog->setDefaultDialogTitle(dialogCaption);

    centerWindowOnCurrentMonitor(MonitorFromWindow(GetHwnd(), 0), reinterpret_cast<CWnd&>(*editTileDescriptionDialog));
    editTileDescriptionDialog->ShowWindow(SW_SHOW);
}

//-----------------------------------------------------------------------------
// finishedEditTileDescriptionDialog
//-----------------------------------------------------------------------------

void MainWindowFrame::finishedEditTileDescriptionDialog() {
    

    if(!editTileDescriptionDialog) {
        return;
    }

    if(editTileDescriptionDialog->hasSavedChanges()) {



        gameWorldController->tryUpdateTileDescription(selectedTileIndex, editTileDescriptionDialog->getTileName(),
                                                      editTileDescriptionDialog->getTileDescription());
    }

    delete editTileDescriptionDialog;
    editTileDescriptionDialog = NULL;
    activeWindowHandle = GetHwnd();

}