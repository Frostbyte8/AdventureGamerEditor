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
                                     activeWindowHandle(0), editObjectDialog(0) {

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

    switch(msg) {

        case WM_ACTIVATEAPP:
        case WM_ACTIVATE:

            // Make sure the correct window goes to the top
            if(!IsWindowEnabled() && activeWindowHandle != 0) {
                if((BOOL)wParam == TRUE) {
                    ::SetWindowPos(activeWindowHandle, HWND_TOP, 0, 0, 0, 0, 
                                   SWP_NOMOVE | SWP_NOSIZE);
                }
                return 0;
            }

            break;
    }


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
        entityView->updateLists(gameWorldController->getGameMap()->getGameObjects());
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

void MainWindowFrame::finishedEditObjectDialog() {

    if(editObjectDialog) {
        delete editObjectDialog;
        editObjectDialog = NULL;
        activeWindowHandle = GetHwnd();
    }

}

void MainWindowFrame::onEditObject(const int& alterType) {

    LanguageMapper& langMap = LanguageMapper::getInstance();

    if(!editObjectDialog && activeWindowHandle == GetHwnd()) {
        
        editObjectDialog = new EditObjectDialog(this, &windowMetrics, gameWorldController->getGameMap());
        editObjectDialog->Create(0, WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE, WS_POPUPWINDOW | WS_CAPTION);
        editObjectDialog->SetParentWindow(GetHwnd());

        // We need to hold on to this as we will need it to make sure the Dialog is the front most window
        activeWindowHandle = editObjectDialog->GetHwnd();

        CString caption;

        if(alterType == 0) {
            EOD_SetWindowText(LanguageConstants::AddObjectDialogCaption, *editObjectDialog, caption, langMap);
        }
        else {
            //EOD_SetWindowText(303, editObjectDialog, caption, langMap);
        }

        const CSize contentSize = editObjectDialog->getContentSize();
        RECT rc = {0, 0, contentSize.cx, contentSize.cy};

        // TODO: DPI
        const HMONITOR currentMonitor = MonitorFromWindow(GetHwnd(), 0);
        MONITORINFOEX monitorInfo;
        monitorInfo.cbSize = sizeof(MONITORINFOEX);
        GetMonitorInfo(currentMonitor, &monitorInfo);

        
        AdjustWindowRectEx(&rc, editObjectDialog->GetStyle(), FALSE, editObjectDialog->GetExStyle());
        CPoint windowPos;
        
        // Calculate where on the monitor the window is position

        windowPos.x = (abs(monitorInfo.rcWork.right - monitorInfo.rcWork.left) / 2) - ((rc.right + abs(rc.left)) / 2);
        windowPos.y = (abs(monitorInfo.rcWork.bottom - monitorInfo.rcWork.top) / 2) - ((rc.bottom + abs(rc.top)) / 2);

        // Offset this to where the monitor is
        windowPos.Offset(monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.top);
        
        editObjectDialog->DoStuff();

        // Move window to the top and show it.
        //editObjectDialog->MoveWindow(0, 0, contentSize.cx, contentSize.cy, TRUE);
        editObjectDialog->SetWindowPos(HWND_TOP, windowPos.x, windowPos.y, rc.right + abs(rc.left), rc.bottom + abs(rc.top), 0);
        editObjectDialog->ShowWindow(SW_SHOW);

   } 
}
