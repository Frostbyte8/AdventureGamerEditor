#ifndef __EDITOR_MAINWINDOW_VIEWS_H__
#define __EDITOR_MAINWINDOW_VIEWS_H__

#include <wxx_wincore.h>
#include <wxx_docking.h>
#include <wxx_scrollview.h>
#include "../controller/gameworld_controller.h"
#include "../interface/mainwindow_interface.h"
#include "../win32/window_metrics.h"
#include <vector>

///----------------------------------------------------------------------------
/// GameEntitiesView - Shows Objects and Characters defined by the game world.
/// This is the right-most panel.
///----------------------------------------------------------------------------

class GameEntitiesView : public CWnd {

	public:
		GameEntitiesView(MainWindowInterface* inMainWindow, WindowMetrics* inWindowMetrics) : mainWindow(inMainWindow), windowMetrics(inWindowMetrics) {}
		virtual ~GameEntitiesView() {}
        void updateLists(const std::vector<GameObject>& gameObject, const std::vector<GameCharacter>& gameCharacters);

    protected:
        virtual void PreRegisterClass(WNDCLASS& wc);

        virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
        virtual int OnCreate(CREATESTRUCT& cs);
        virtual LRESULT WndProc(UINT msg, WPARAM wParam, LPARAM lParam);

	private:

        int OnSize(const WPARAM& wParam, const LPARAM& lParam);
        void sizeGroupBox(HDWP& hDWP, const bool doCharacters, const CRect& dimensions, const WindowMetrics::ControlSpacing& cs, const WindowMetrics::ControlDimensions& cd);

		// Disable copy construction and assignment operator
	    GameEntitiesView(const GameEntitiesView&);
		GameEntitiesView& operator = (const GameEntitiesView&);

        MainWindowInterface*    mainWindow;
        WindowMetrics*          windowMetrics;

        CButton                 objectsGroup;
        CButton                 charactersGroup;
        CListBox                objectsListBox;
        CListBox                charactersListBox;
        CButton                 alterObjectButton[4];
        CButton                 alterCharacterButton[4];
};

///----------------------------------------------------------------------------
/// RoadSelectorView - Show all the road tiles available
///----------------------------------------------------------------------------

class RoadSelectorView : public CScrollView {

	public:
		RoadSelectorView(MainWindowInterface* inMainWindow) : backBufferDC(0), tilesetDC(0), mainWindow(inMainWindow), selectedTile(0) {}
		virtual ~RoadSelectorView() {}
        virtual void PreRegisterClass(WNDCLASS& wc) {
            wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
            wc.lpszClassName = L"RoadSelectorView";
            wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
        }
        
        void SetTileset(CBitmap& inTileSet);

    protected:
        virtual int OnCreate(CREATESTRUCT& cs);
        virtual void OnDraw(CDC& dc);
        
        virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam) {
            
            if(uMsg == WM_LBUTTONDOWN) {
                return OnLButtonDown(LOWORD(lParam), HIWORD(lParam));
            }
            
            return WndProcDefault(uMsg, wParam, lParam);
        } 

	private:

        LRESULT OnLButtonDown(const WORD& x, const WORD& y);
        void UpdateScrollSize();
        void UpdateBackBuffer();

        MainWindowInterface*     mainWindow;

        CMemDC			        tilesetDC;
        CMemDC                  backBufferDC;
        CBitmap                 backBufferBMP;

        int tileWidth;
        int tileHeight;

        int selectedTile;

		// Disable copy construction and assignment operator
	    RoadSelectorView(const RoadSelectorView&);
		RoadSelectorView& operator = (const RoadSelectorView&);
};

///----------------------------------------------------------------------------
/// GameMapView - Displays the game world's map.
///----------------------------------------------------------------------------

class GameMapView : public CScrollView {
	
	public:

        GameMapView(GameWorldController* gwc);
        virtual ~GameMapView() {}
        //void onZoomChange();
        void setTileset(CBitmap& inTileSet);
        void UpdateBackBuffer();

    protected:
        virtual int OnCreate(CREATESTRUCT& cs);
        virtual void OnDraw(CDC& dc);
        virtual LRESULT WndProc(UINT msg, WPARAM wParam, LPARAM lParam);

	private:

		// Disable copy construction and assignment operator
	    GameMapView(const GameMapView&);
		GameMapView& operator = (const GameMapView&);

        CMemDC			    tilesetDC;
        //CBitmap           tilesetBMP;
        
        CMemDC      backBufferDC;
        CBitmap     backBufferBMP;
        
        GameWorldController*    gameWorldController;

        int fakeZoomLevel;
        int tileWidth;
        int tileHeight;
};

///----------------------------------------------------------------------------
/// EntitiesHereView - Displays what Objects / Characters are at the space that
/// is currently selected.
///----------------------------------------------------------------------------

class EntitiesHereView : public CWnd {
	
	public:
		EntitiesHereView(WindowMetrics* inWindowMetrics) : windowMetrics(inWindowMetrics) {}
		virtual ~EntitiesHereView() {}
        virtual LRESULT WndProc(UINT msg, WPARAM wParam, LPARAM lParam);

        virtual void PreRegisterClass(WNDCLASS& wc) {
            wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
            wc.lpszClassName = L"EntitiesHereView";
            wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
        }

        void updateLists(const std::vector<GameObject>& objectVec);

    protected:
        virtual int OnCreate(CREATESTRUCT& cs);

	private:

        int OnSize(const WPARAM& wParam, const LPARAM& lParam);

        WindowMetrics*  windowMetrics;

        CButton     objectsHereGroup;
        CButton     charactersHereGroup;
        CListBox    objectsHereListBox;
        CListBox    charactersHereListBox;

		// Disable copy construction and assignment operator
	    EntitiesHereView(const EntitiesHereView&);
		EntitiesHereView& operator = (const EntitiesHereView&);
};


#endif // __EDITOR_MAINWINDOW_VIEWS_H__
