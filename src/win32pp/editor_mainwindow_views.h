#ifndef __EDITOR_MAINWINDOW_VIEWS_H__
#define __EDITOR_MAINWINDOW_VIEWS_H__

#include <wxx_wincore.h>
#include <wxx_docking.h>
#include <wxx_scrollview.h>
#include "../controller/gameworld_controller.h"
#include "../interface/mainwindow_interface.h"
#include "../win32/window_metrics.h"
#include <vector>

/*
class PanelBaseClass : public CWnd {

    public:
        
        void setWindowMetrics(WindowMetrics* inWindowMetrics);
        virtual void moveControls() = 0;
        virtual void updateCaptions() = 0;
        
        const virtual std::wstring getClassName() const = 0;
};
*/

class PanelBaseClass : public CWnd {

    protected:

        const virtual std::wstring getClassName() const = 0;

        virtual void PreRegisterClass(WNDCLASS& wc) {
            wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
            className = getClassName(); //
            wc.lpszClassName = className.c_str();
            wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
        }

    private:
        std::wstring className;

};

class PanelBaseTest : public CWnd {
    
    public:
        const virtual std::wstring getClassName() const = 0;

        virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam) {
            
            if (uMsg == WM_MOUSEMOVE) {
                MessageBox(getClassName().c_str(), L"", MB_OK);
            }

            return WndProcDefault(uMsg, wParam, lParam);
        }

};

///----------------------------------------------------------------------------
/// GameEntitiesPanel - Shows Objects and Characters defined by the game world.
/// This is the right-most panel.
///----------------------------------------------------------------------------

class GameEntitiesPanel : public PanelBaseClass {

	public:

        GameEntitiesPanel(MainWindowInterface* inMainWindow, WindowMetrics* inWindowMetrics);
        virtual ~GameEntitiesPanel();

        void updateCharacterList(const std::vector<GameCharacter>& characterList);
        void updateObjectList(const std::vector<GameObject>& objectList);

    protected:

        virtual int OnCreate(CREATESTRUCT& cs);
        virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
        virtual void PreCreate(CREATESTRUCT& cs);

        virtual LRESULT WndProc(UINT msg, WPARAM wParam, LPARAM lParam);

        const virtual std::wstring getClassName() const { return L"GameEntitiesPanel"; }

	private:

        int OnSize(const WPARAM& wParam, const LPARAM& lParam);

        void sizeGroupBox(HDWP& hDWP, const bool doCharacters, const CRect& dimensions, 
                          const WindowMetrics::ControlSpacing& cs,
                          const WindowMetrics::ControlDimensions& cd);

		// Disable copy construction and assignment operator
        GameEntitiesPanel(const GameEntitiesPanel&);
        GameEntitiesPanel& operator = (const GameEntitiesPanel&);

        // Member Variables

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
/// EntitiesHerePanel - Displays what Objects / Characters are at the space that
/// is currently selected.
///----------------------------------------------------------------------------

class EntitiesHerePanel : public PanelBaseClass {

    public:

        EntitiesHerePanel(WindowMetrics* inWindowMetrics);
        virtual ~EntitiesHerePanel();
        virtual LRESULT WndProc(UINT msg, WPARAM wParam, LPARAM lParam);

        void updateCharacterList(const std::vector<GameCharacter>& characterList);
        void updateObjectList(const std::vector<GameObject>& objectList);

    protected:
        
        virtual int OnCreate(CREATESTRUCT& cs);
        virtual void PreCreate(CREATESTRUCT& cs);
        
        const virtual std::wstring getClassName() const { return L"EntitiesHerePanel"; }

    private:

        int OnSize(const WPARAM& wParam, const LPARAM& lParam);

        WindowMetrics*  windowMetrics;

        CButton     objectsHereGroup;
        CButton     charactersHereGroup;
        CListBox    objectsHereListBox;
        CListBox    charactersHereListBox;

        // Disable copy construction and assignment operator
        EntitiesHerePanel(const EntitiesHerePanel&);
        EntitiesHerePanel& operator = (const EntitiesHerePanel&);
};

///----------------------------------------------------------------------------
/// RoadSelectorView - Show all the road tiles available
///----------------------------------------------------------------------------

class RoadSelectorView : public CScrollView {

	public:
		RoadSelectorView(MainWindowInterface* inMainWindow, GameWorldController* gwc) : gameWorldController(gwc), backBufferDC(0), tilesetDC(0), mainWindow(inMainWindow) {}
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

        MainWindowInterface*    mainWindow;
        GameWorldController*    gameWorldController;
        CMemDC			        tilesetDC;
        CMemDC                  backBufferDC;
        CBitmap                 backBufferBMP;


        int tileWidth;
        int tileHeight;

		// Disable copy construction and assignment operator
	    RoadSelectorView(const RoadSelectorView&);
		RoadSelectorView& operator = (const RoadSelectorView&);
};

///----------------------------------------------------------------------------
/// GameMapView - Displays the game world's map.
///----------------------------------------------------------------------------

class GameMapView : public CScrollView {
	
	public:

        GameMapView(MainWindowInterface* inMainWindow, GameWorldController* gwc);
        virtual ~GameMapView() {}
        //void onZoomChange();
        void setTileset(CBitmap& inTileSet);
        void UpdateBackBuffer();

    protected:
        virtual int OnCreate(CREATESTRUCT& cs);
        virtual void OnDraw(CDC& dc);
        LRESULT onLButtonDown(const WORD& xPos, const WORD& yPos);
        LRESULT onLButtonDBLClick(const WORD& xPos, const WORD& yPos);
        virtual LRESULT WndProc(UINT msg, WPARAM wParam, LPARAM lParam);

        virtual void PreCreate(CREATESTRUCT& cs) {
            // This has to be done as ScrollView is it's own custom control
            CScrollView::PreCreate(cs);
            cs.lpszClass = L"GameMapView";
        }

        virtual void PreRegisterClass(WNDCLASS& wc) {
            wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
            wc.lpszClassName = L"GameMapView";
            wc.style = CS_DBLCLKS;
        }

	private:

		// Disable copy construction and assignment operator
	    GameMapView(const GameMapView&);
		GameMapView& operator = (const GameMapView&);

        CMemDC			    tilesetDC;
        
        CMemDC      backBufferDC;
        CBitmap     backBufferBMP;
        MainWindowInterface*    mainWindow;
        GameWorldController*    gameWorldController;

        int fakeZoomLevel;
        int tileWidth;
        int tileHeight;
        //int selectedRow;
        //int selectedCol;
};

#endif // __EDITOR_MAINWINDOW_VIEWS_H__
