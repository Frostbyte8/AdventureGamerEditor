#include "editor_mainwindow.h"
#include "../editor_constants.h"
#include "../model/gametile.h"
#include <vector>

#include <wxx_commondlg.h>

//=============================================================================
// Constructors / Destructor
//=============================================================================

///----------------------------------------------------------------------------
/// MainWindowFrame - Creates a new window frame with entity view as the
/// main view.
///----------------------------------------------------------------------------

MainWindowFrame::MainWindowFrame() : entityView(0), gameMapDocker(0), entitiesHereDocker(0), 
                                     roadSelectorDocker(0), gameWorldController(0),
                                     languageController(0) {

	entityView = new GameEntitiesView();
    gameWorldController = new GameWorldController(this);
    languageController  = new LanguageController();

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

    if(languageController) {
        delete languageController;
        languageController = NULL;
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
    languageController->LoadLanguageFile("TODO", "TODO");
    SetView(*entityView);
	return CDockFrame::Create(parent);
}

//=============================================================================
// Protected / Private Functions
//=============================================================================

///----------------------------------------------------------------------------
/// CreateMenuBar - Creates the menu bar.
///----------------------------------------------------------------------------

#define ADV_ADDMENUITEM(ID, MENUOBJ) caption = AtoW(languageMapper.getLangString(ID).c_str(), CP_UTF8); \
    MENUOBJ.AppendMenu(MF_STRING, ID, caption);

void MainWindowFrame::CreateMenuBar() {

    mainMenu.CreateMenu();
    fileMenu.CreatePopupMenu();

    LanguageMapper& languageMapper = languageController->getLanguageMapper();

    CStringW caption;

    ADV_ADDMENUITEM(LanguageConstants::NewMenuItem, fileMenu);
    ADV_ADDMENUITEM(LanguageConstants::OpenMenuItem, fileMenu);
    ADV_ADDMENUITEM(LanguageConstants::SaveMenuItem, fileMenu);
    ADV_ADDMENUITEM(LanguageConstants::SaveAsMenuItem, fileMenu);
    ADV_ADDMENUITEM(LanguageConstants::ExitMenuItem, fileMenu);

    caption = AtoW(languageMapper.getLangString(LanguageConstants::FileMenuItem).c_str(), CP_UTF8);
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

	const int retVal = CDockFrame::OnCreate(cs);

    CreateMenuBar();

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

    return retVal;
}

///----------------------------------------------------------------------------
/// OnInitialUpdate - 
/// Refer to the Win32++ documentation for more information.
///----------------------------------------------------------------------------

void MainWindowFrame::OnInitialUpdate() {
    
    
    /*
    const std::vector<GameTile> tiles = gameWorldController->getTiles();
    // Just for a test.
    DisplayErrorMessage(tiles.at(0).getDescription(), tiles.at(0).getName());
    */
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

	return WndProcDefault(msg, wParam, lParam);
}

BOOL MainWindowFrame::OnCommand(WPARAM wParam, LPARAM) {
    
    switch (LOWORD(wParam)) {
        case LanguageConstants::NewMenuItem: return OnFileNew();
        case LanguageConstants::OpenMenuItem: return OnFileOpen();
    }

    return FALSE;

}

BOOL MainWindowFrame::OnFileNew() {
    gameWorldController->newWorld();
    reinterpret_cast<GameMapView&>(gameMapDocker->GetView()).UpdateBackBuffer();
    return TRUE;
}

BOOL MainWindowFrame::OnFileOpen() {

	CFileDialog fileDialog(TRUE, L"SG0", NULL, OFN_NOLONGNAMES | OFN_FILEMUSTEXIST, L"Adventure Gamer World Files (*.SG0)\0*.SG0\0\0");
	fileDialog.SetTitle(L"Open World File");

	if(fileDialog.DoModal() == IDOK) {

		std::string filePath(WtoA(fileDialog.GetFolderPath().c_str()));
		std::string fileName(WtoA(fileDialog.GetFileName().c_str()));

		gameWorldController->loadWorld(filePath, fileName);
        reinterpret_cast<GameMapView&>(gameMapDocker->GetView()).UpdateBackBuffer();

	}

    return TRUE;
}

//=============================================================================
// Public Interface Functions
//=============================================================================

int MainWindowFrame::AskYesNoQuestion(const std::string& inQuestion, const std::string& inTitle, bool allowCancel) {

    const CString question      = AtoW(inQuestion.c_str(), CP_UTF8);
    const CString title         = AtoW(inTitle.c_str(), CP_UTF8);
    const UINT messageBoxFlags  = MB_ICONQUESTION | (allowCancel ? MB_YESNOCANCEL : MB_YESNO);

    const int retVal = MessageBox(question, title, messageBoxFlags);

    return retVal;
}

void MainWindowFrame::DisplayErrorMessage(const std::string& inMessage, const std::string& inTitle) {

    const CString message       = AtoW(inMessage.c_str(), CP_UTF8);
    const CString title         = AtoW(inTitle.c_str(), CP_UTF8);
    const UINT messageBoxFlags  = MB_ICONERROR;

    MessageBox(message, title, messageBoxFlags);

}
