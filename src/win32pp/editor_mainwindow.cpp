#include "editor_mainwindow.h"
#include "../editor_constants.h"
#include <fstream>
#include "../thirdparty/simpleson/json.h"
#include "../compat/std_extras_compat.h"

//=============================================================================
// Constructors / Destructor
//=============================================================================

///----------------------------------------------------------------------------
/// MainWindowFrame - Creates a new window frame with entity view as the
/// main view.
///----------------------------------------------------------------------------

MainWindowFrame::MainWindowFrame() : entityView(0), gameMapDocker(0), entitiesHereDocker(0), 
                                     roadSelectorDocker(0) {
	entityView = new GameEntitiesView();
}

///----------------------------------------------------------------------------
/// ~MainWindowFrame - Cleans up resources used by the main window frame.
///----------------------------------------------------------------------------

MainWindowFrame::~MainWindowFrame() {
	if(entityView) {
		delete entityView;
		entityView = NULL;
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

    // Until we do some parsing, we'll load the Language Map here.

	std::wstring fileName = L"lang_en.json";

	std::ifstream ifs;
	ifs.open(fileName.c_str(), std::ifstream::in | std::ios::binary);

	if(ifs) {
		std::string data;
		ifs.seekg(0, std::ios::end);
		data.resize(ifs.tellg());
		ifs.seekg(0, std::ios::beg);
		ifs.read(&data[0], data.size());
		ifs.close();


		json::jobject result = json::jobject::parse(data);
		json::key_list_t keyList = result.list_keys();
		json::key_list_t::iterator it;

		for(it = keyList.begin(); it != keyList.end(); ++it) {
			int key = std::stoi(*it);
			languageMapper.addLangString(key, result.get(*it));
		}
	}

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

	CStringW caption = AtoW(languageMapper.getLangString(101).c_str(), CP_UTF8);
    fileMenu.AppendMenu(MF_STRING, 0, caption);


	caption = AtoW(languageMapper.getLangString(100).c_str(), CP_UTF8);
    mainMenu.AppendMenu(MF_STRING | MF_POPUP,
                        reinterpret_cast<UINT_PTR>(fileMenu.GetHandle()), caption);

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

	const int retVal = CDockFrame::OnCreate(cs);

    CreateMenuBar();

	// Create our Dockers

	DWORD styleFlags = DS_NO_UNDOCK | DS_NO_CAPTION | DS_DEFAULT_CURSORS | DS_CLIENTEDGE;
	SetDockStyle(styleFlags);

	gameMapDocker = static_cast<GameMapDocker*>(AddDockedChild(new GameMapDocker(), 
                                                styleFlags | DS_DOCKED_LEFT, 128));

	roadSelectorDocker = static_cast<RoadSelectorDocker*>(gameMapDocker->AddDockedChild(
                                                          new RoadSelectorDocker(),
                                                          styleFlags | DS_DOCKED_LEFT | DS_NO_RESIZE, 0));

	entitiesHereDocker = static_cast<EntitiesHereDocker*>(gameMapDocker->AddDockedChild(
                                                          new EntitiesHereDocker(), styleFlags | DS_DOCKED_BOTTOM, 128)); 


    // The Road Selector is the Width of one tile plus the scroll bar
    // TODO: Zoom Factor

	CRect rc;
	rc.right = EditorConstants::TileWidth + windowMetrics.GetControlDimensions().X_SCROLLBAR;
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

