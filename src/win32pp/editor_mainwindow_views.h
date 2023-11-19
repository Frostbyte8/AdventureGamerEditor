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
            className = getClassName();
            wc.lpszClassName = className.c_str();
            wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
        }

    private:
        std::wstring className;

};

class ScrollPanelBaseClass : public CScrollView {

    protected:

        const virtual std::wstring getClassName() const = 0;

        virtual void PreRegisterClass(WNDCLASS& wc) {
            wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
            className = getClassName();
            wc.lpszClassName = className.c_str();
            wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
        }

        virtual void PreCreate(CREATESTRUCT& cs) {
            // This has to be done as ScrollView is it's own custom control
            CScrollView::PreCreate(cs);
            className = getClassName();
            cs.lpszClass = className.c_str();
        }

    private:
        std::wstring className;
};

//=============================================================================
//
// Panels - The Windows that belong to each Docker
//
//=============================================================================

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
/// RoadPalettePanel - Show all the road tiles available
///----------------------------------------------------------------------------

class RoadPalettePanel : public ScrollPanelBaseClass {

	public:
        RoadPalettePanel(MainWindowInterface* inMainWindow, GameWorldController* gwc);// : gameWorldController(gwc), backBufferDC(0), tilesetDC(0), mainWindow(inMainWindow) {}
        virtual ~RoadPalettePanel();
        
        void setTileset(CBitmap& inTileSet);

    protected:

        const virtual std::wstring getClassName() const { return L"RoadPalettePanel"; }

        virtual int     OnCreate(CREATESTRUCT& cs);
        virtual void    OnDraw(CDC& dc);
        virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

	private:

        LRESULT onLButtonDown(const WORD& x, const WORD& y);
        void    updateScrollSize();
        void    updateBackBuffer();

        MainWindowInterface*    mainWindow;
        GameWorldController*    gameWorldController;
        CMemDC			        tilesetDC;
        CMemDC                  backBufferDC;
        CBitmap                 backBufferBMP;


        int tileWidth;
        int tileHeight;

		// Disable copy construction and assignment operator
        RoadPalettePanel(const RoadPalettePanel&);
        RoadPalettePanel& operator = (const RoadPalettePanel&);
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

};

//=============================================================================
//
// Dockers
//
//=============================================================================

///----------------------------------------------------------------------------
/// EntitiesHereDocker - Docker that contains the EntitiesHereView
///----------------------------------------------------------------------------

class EntitiesHereDocker : public CDocker {

    public:
        EntitiesHereDocker(WindowMetrics* inWindowMetrics) : view(inWindowMetrics) {
            SetView(view);
            SetBarWidth(6);
        }
        virtual ~EntitiesHereDocker() {}

    private:

        EntitiesHerePanel view;

        // Disable copy construction and assignment operator
        EntitiesHereDocker(const EntitiesHereDocker&);
        EntitiesHereDocker& operator=(const EntitiesHereDocker&);

};

///----------------------------------------------------------------------------
/// GameMapDocker - Docker that contains the GameMapView
///----------------------------------------------------------------------------

class GameMapDocker : public CDocker {

    public:
        GameMapDocker(MainWindowInterface* inMainWindow, GameWorldController* gwc,
                      const CBitmap* inTileSet) : view(inMainWindow, gwc) {

            SetView(view);
            SetBarWidth(6);

        }

        virtual ~GameMapDocker() {}

    private:

        GameMapView view;

        // Disable copy construction and assignment operator
        GameMapDocker(const GameMapDocker&);
        GameMapDocker& operator=(const GameMapDocker&);

};


///----------------------------------------------------------------------------
/// RoadSelectorDocker - Docker that contains the RoadSelectorView
///----------------------------------------------------------------------------

class RoadSelectorDocker : public CDocker {

    public:

        RoadSelectorDocker(MainWindowInterface* inMainWindow, GameWorldController* gwc) : 
                           view(inMainWindow, gwc) {

            SetView(view);
            SetBarWidth(0);
        }
        virtual ~RoadSelectorDocker() {}

    private:

        RoadPalettePanel view;

        // Disable copy construction and assignment operator
        RoadSelectorDocker(const RoadSelectorDocker&);
        RoadSelectorDocker& operator=(const RoadSelectorDocker&);
};

#endif // __EDITOR_MAINWINDOW_VIEWS_H__
