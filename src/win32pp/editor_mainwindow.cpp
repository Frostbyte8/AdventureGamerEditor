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
                                     roadSelectorDocker(0), gameWorldController(0),
                                     activeWindowHandle(0), editObjectDialog(0),
                                     editCharacterDialog(0), editWorldInfoDialog(0) {

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

#define ADV_ADDMENUITEM(ID, MENUOBJ) caption = AtoW(languageMapper.get(ID).c_str(), CP_UTF8); \
    MENUOBJ.AppendMenu(MF_STRING, ID, caption);

void MainWindowFrame::CreateMenuBar() {

    mainMenu.CreateMenu();
    fileMenu.CreatePopupMenu();

    LanguageMapper& languageMapper = LanguageMapper::getInstance();

    CStringW caption;

    ADV_ADDMENUITEM(LanguageConstants::NewMenuItem, fileMenu);
    ADV_ADDMENUITEM(LanguageConstants::OpenMenuItem, fileMenu);
    ADV_ADDMENUITEM(LanguageConstants::SaveMenuItem, fileMenu);
    ADV_ADDMENUITEM(LanguageConstants::SaveAsMenuItem, fileMenu);
    ADV_ADDMENUITEM(LanguageConstants::ExitMenuItem, fileMenu);

    caption = AtoW(languageMapper.get(LanguageConstants::FileMenuItem).c_str(), CP_UTF8);
    mainMenu.AppendMenu(MF_STRING | MF_POPUP,
                        reinterpret_cast<UINT_PTR>(fileMenu.GetHandle()), caption);

    SetFrameMenu(mainMenu);

}

#undef ADV_ADDMENUITEM

///----------------------------------------------------------------------------
/// OnCreate - Set some defaults for the frame, and create remaining child
/// controls.
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

int MainWindowFrame::OnCreate(CREATESTRUCT& cs) {
	
	UseThemes(FALSE);				// Don't use themes
    //m_bUseCustomDraw = FALSE;     // Don't use custom draw for menu items (?)

    SetStyle(GetStyle() | WS_CLIPCHILDREN);

	const int retVal = CDockFrame::OnCreate(cs);

    LanguageMapper& langMap = LanguageMapper::getInstance();
    CString caption = AtoW(langMap.get(LanguageConstants::MainWindowCaption).c_str(), CP_UTF8);
    SetWindowText(caption);

    CreateMenuBar();

    CRect windowDims = GetClientRect();

	// Create our Dockers

	DWORD styleFlags = DS_NO_UNDOCK | DS_NO_CAPTION | DS_DEFAULT_CURSORS | DS_CLIENTEDGE;
	SetDockStyle(styleFlags);

    gameMapDocker = static_cast<GameMapDocker*>(AddDockedChild(new GameMapDocker(gameWorldController), 
                                                styleFlags | DS_DOCKED_LEFT, 128));

    // TODO: The tileset image should be created here and pass to the view on creation.

    if(!(reinterpret_cast<GameMapView&>(gameMapDocker->GetView()).isBMPLoaded())) {
        MessageBox(L"Could not find tileset.bmp. Please ensure this file is in the same directory as advedit.exe. The program will now close.", L"Missing file tileset.bmp", MB_OK | MB_ICONERROR);
        Close();
        return 1;
    }

	roadSelectorDocker = static_cast<RoadSelectorDocker*>(gameMapDocker->AddDockedChild(
                                                          new RoadSelectorDocker(),
                                                          styleFlags | DS_DOCKED_LEFT | DS_NO_RESIZE, 0));

	entitiesHereDocker = static_cast<EntitiesHereDocker*>(gameMapDocker->AddDockedChild(
                                                          new EntitiesHereDocker(), styleFlags | DS_DOCKED_BOTTOM, 128));


    // The Road Selector is the Width of one tile plus the scroll bar
    // TODO: Zoom Factor

	CRect rc;

    const int tileWidth = reinterpret_cast<GameMapView&>(gameMapDocker->GetView()).getTileWidth();
	rc.right = tileWidth + windowMetrics.GetControlDimensions().X_SCROLLBAR;
	AdjustWindowRectEx(&rc, 0, FALSE, roadSelectorDocker->GetDockClient().GetExStyle());
	const int newWidth = abs(rc.right - rc.left);
	roadSelectorDocker->SetDockSize(newWidth);   

    windowDims.right -= newWidth;

    const int mapSize = static_cast<int>(windowDims.right * 0.5);
    gameMapDocker->SetDockSize(mapSize);

    windowDims.right -= mapSize;

    return retVal;
}

///----------------------------------------------------------------------------
/// OnInitialUpdate - 
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

void MainWindowFrame::OnInitialUpdate() {
    
    activeWindowHandle = *this; 
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


	return WndProcDefault(msg, wParam, lParam);
}

///----------------------------------------------------------------------------
/// OnCommand - Processes WM_COMMAND events
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

BOOL MainWindowFrame::OnCommand(WPARAM wParam, LPARAM) {
    
    static int i = 0;
    switch (LOWORD(wParam)) {
        case LanguageConstants::NewMenuItem: return OnFileNew();
        case LanguageConstants::OpenMenuItem: return OnFileOpen();
    }

    return FALSE;

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

	if(fileDialog.DoModal() == IDOK) {

		std::string filePath(WtoA(fileDialog.GetFolderPath().c_str()));
		std::string fileName(WtoA(fileDialog.GetFileName().c_str()));

		gameWorldController->loadWorld(filePath, fileName);
        reinterpret_cast<GameMapView&>(gameMapDocker->GetView()).UpdateBackBuffer();
        entityView->updateLists(gameWorldController->getGameMap()->getGameObjects(), gameWorldController->getGameMap()->getGameCharacters());
	}

    return TRUE;
}

//=============================================================================
// Public Interface Functions
// Refer to the interface header file for more information.
//=============================================================================

int MainWindowFrame::askYesNoQuestion(const std::string& inQuestion, const std::string& inTitle,
                                      bool allowCancel) {

    const CString question      = AtoW(inQuestion.c_str(), CP_UTF8);
    const CString title         = AtoW(inTitle.c_str(), CP_UTF8);
    const UINT messageBoxFlags  = MB_ICONQUESTION | (allowCancel ? MB_YESNOCANCEL : MB_YESNO);

    const int retVal = MessageBox(question, title, messageBoxFlags);

    return retVal;
}

void MainWindowFrame::displayErrorMessage(const std::string& inMessage,
                                          const std::string& inTitle) {

    const CString message       = AtoW(inMessage.c_str(), CP_UTF8);
    const CString title         = AtoW(inTitle.c_str(), CP_UTF8);
    const UINT messageBoxFlags  = MB_ICONERROR;

    MessageBox(message, title, messageBoxFlags);

}

void MainWindowFrame::onAlterObject(const int& alterType, const size_t& index) {

    if(!editObjectDialog && activeWindowHandle == *this) {

        const std::vector<GameObject>& gameObjects = gameWorldController->getGameMap()->getGameObjects();
        const bool editingObject = (alterType == AlterType::Edit) ? true : false;

        if(editingObject) {
            
            if(gameObjects.empty() || index > gameObjects.size() - 1) {
                displayErrorMessage("Invalid object index", "error");
                return;
            }
        }

        editObjectDialog = new EditObjectDialog(this, gameWorldController->getGameMap(), *this, editingObject);
        editObjectDialog->Create(*this, WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT, WS_POPUPWINDOW | WS_DLGFRAME);
        editObjectDialog->SetExStyle(editObjectDialog->GetExStyle() | WS_EX_DLGMODALFRAME);
        

        if(!editObjectDialog->IsWindow()) {
            // TODO: Handle error.
        }

        activeWindowHandle = editObjectDialog->GetHwnd();
        editObjectDialog->GoModal();

        CString caption;
        LanguageMapper& langMap = LanguageMapper::getInstance();

        if(alterType == AlterType::Add) {
            GameObject::Builder bd;
            editObjectDialog->SetObjectToEdit(bd.build());
            EOD_SetWindowText(LanguageConstants::AddObjectDialogCaption, *editObjectDialog, caption, langMap);
        }
        else if (alterType == AlterType::Edit) {
            // TODO: make sure the character exists before doing this.
            const GameObject& gameObject = gameWorldController->getGameMap()->getGameObjects().at(index);
            GameObject::Builder objectToEdit(gameObject);
            editObjectDialog->SetObjectToEdit(objectToEdit.build());
            caption = LM_toUTF8(LanguageConstants::EditObjectDialogCaption, langMap);
            caption += gameObject.getName().c_str();
            editObjectDialog->SetWindowText(caption); 
        }

        centerWindowOnCurrentMonitor(MonitorFromWindow(*this, 0), reinterpret_cast<CWnd&>(*editObjectDialog));
        editObjectDialog->ShowWindow(SW_SHOW);
        
   } 
}

void MainWindowFrame::onAlterCharacter(const int& alterType, const size_t& index) {

    // Make sure that the dialog isn't already running, or that
    // another modal window isn't already running.

    if(!editCharacterDialog && activeWindowHandle == *this) {

        const std::vector<GameCharacter>& gameCharacters = gameWorldController->getGameMap()->getGameCharacters();
        const bool editingChar = (alterType == AlterType::Edit) ? true : false;

        if(editingChar) {
            
            if(gameCharacters.empty() || index > gameCharacters.size() - 1) {
                displayErrorMessage("Invalid character index", "error");
                return;
            }
        }

        editCharacterDialog = new EditCharacterDialog(this, gameWorldController->getGameMap(), *this, editingChar);
        editCharacterDialog->Create(*this, WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT, WS_POPUPWINDOW | WS_DLGFRAME);
        editCharacterDialog->SetExStyle(editCharacterDialog->GetExStyle() | WS_EX_DLGMODALFRAME);

        if(!editCharacterDialog->IsWindow()) {
            // TODO: Handle error.
        }

        activeWindowHandle = editCharacterDialog->GetHwnd();
        editCharacterDialog->GoModal();

        // TODO: Set Caption

        LanguageMapper& langMap = LanguageMapper::getInstance();
        CString caption;

        if(alterType == AlterType::Add) {
            GameCharacter::Builder bd;
            editCharacterDialog->SetCharacterToEdit(bd.build());
            EOD_SetWindowText(LanguageConstants::AddCharacterDialogCaption, *editCharacterDialog, caption, langMap);
        }
        else if(alterType == AlterType::Edit) {

            const GameCharacter& gameCharacter = gameWorldController->getGameMap()->getGameCharacters().at(0);
            caption = LM_toUTF8(LanguageConstants::EditCharacterDialogCaption, langMap);
            caption += gameCharacter.getName().c_str();
            GameCharacter::Builder charToEdit(gameCharacter);
            editCharacterDialog->SetCharacterToEdit(charToEdit.build());
            editCharacterDialog->SetWindowText(caption); 
        }
        
        centerWindowOnCurrentMonitor(MonitorFromWindow(*this, 0), reinterpret_cast<CWnd&>(*editCharacterDialog));
        editCharacterDialog->ShowWindow(SW_SHOW);
    }

}

void MainWindowFrame::finishedEditCharacterDialog(const int& alterType, const bool& wasCanceled) {

    if(!editCharacterDialog || (alterType != AlterType::Add && alterType != AlterType::Edit)) {
        return;
    }
   
    if(!wasCanceled) {

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
    activeWindowHandle = *this;

}

void MainWindowFrame::finishedEditObjectDialog(const int& alterType, const bool& wasCanceled) {

    if(!editObjectDialog)  {
        return;
    }

    if(alterType != AlterType::Add && alterType != AlterType::Edit) {
        return;
    }

    if(!wasCanceled) {

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
    activeWindowHandle = *this;

}

void MainWindowFrame::onEditWorldInfo() {

    // Make sure that the dialog isn't already running, or that
    // another modal window isn't already running.

    if(!editWorldInfoDialog && activeWindowHandle == *this) {
        return;
    }

    editWorldInfoDialog = new EditWorldInfoDialog(this, gameWorldController->getGameMap(), *this);
    editWorldInfoDialog->Create(*this, WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT, WS_POPUPWINDOW | WS_DLGFRAME);
    editWorldInfoDialog->SetExStyle(editWorldInfoDialog->GetExStyle() | WS_EX_DLGMODALFRAME);

    if(!editWorldInfoDialog->IsWindow()) {
        // TODO: Handle error.
    }

    activeWindowHandle = editWorldInfoDialog->GetHwnd();
    editWorldInfoDialog->GoModal();

    centerWindowOnCurrentMonitor(MonitorFromWindow(*this, 0), reinterpret_cast<CWnd&>(*editWorldInfoDialog));
    editWorldInfoDialog->ShowWindow(SW_SHOW);
}

void MainWindowFrame::finishedEditWorldInfoDialog(const bool& wasCanceled) {

    if(!editWorldInfoDialog) {
        return;
    }

    if(!wasCanceled) {
        // Update Information
    }

    delete editWorldInfoDialog;
    editWorldInfoDialog = NULL;
    activeWindowHandle = *this;

}
