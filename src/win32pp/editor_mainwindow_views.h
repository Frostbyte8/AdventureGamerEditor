#ifndef __EDITOR_MAINWINDOW_VIEWS_H__
#define __EDITOR_MAINWINDOW_VIEWS_H__

#include <wxx_wincore.h>
#include <wxx_docking.h>
#include <wxx_scrollview.h>
#include "../controller/gameworld_controller.h"
#include "../interface/mainwindow_interface.h"
#include "editor_mainwindow_win32pp_interface.h"
#include "../win32/window_metrics.h"
#include <vector>

//=============================================================================
//
// Abstract Base Classes - Meant to avoid repeating code
//
//=============================================================================


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

#ifdef __WIN9X_COMPAT__
        const virtual std::string getClassName() const = 0;
#else 
        const virtual std::wstring getClassName() const = 0;
#endif

        virtual void PreRegisterClass(WNDCLASS& wc) {
            wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
            className = getClassName();
            wc.lpszClassName = className.c_str();
            wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
        }

    private:
#ifdef __WIN9X_COMPAT__
        std::string className;
#else 
        std::wstring className;
#endif

};

class ScrollPanelBaseClass : public CScrollView {

    protected:

#ifdef __WIN9X_COMPAT__
        const virtual std::string getClassName() const = 0;
#else 
        const virtual std::wstring getClassName() const = 0;
#endif
        
        const virtual LONG getClassStyles() const { 
            return 0; 
        }

        virtual void PreRegisterClass(WNDCLASS& wc) {
            wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
            className = getClassName();
            wc.lpszClassName = className.c_str();
            wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
            wc.style = getClassStyles();

        }

        virtual void PreCreate(CREATESTRUCT& cs) {
            // This has to be done as ScrollView is it's own custom control
            CScrollView::PreCreate(cs);
            className = getClassName();
            cs.lpszClass = className.c_str();
        }

    private:

#ifdef __WIN9X_COMPAT__
        std::string className;
#else 
        std::wstring className;
#endif
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

        GameEntitiesPanel(GameWorldController* gwc, WindowMetrics* inWindowMetrics);
        virtual ~GameEntitiesPanel();

        void updateCharacterList(const std::vector<GameCharacter>& characterList);
        void updateObjectList(const std::vector<GameObject>& objectList);
        void worldStateChanged();

    protected:

        virtual int OnCreate(CREATESTRUCT& cs);
        virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
        virtual void PreCreate(CREATESTRUCT& cs);

        virtual LRESULT WndProc(UINT msg, WPARAM wParam, LPARAM lParam);

#ifdef __WIN9X_COMPAT__
        const virtual std::string getClassName() const { return "GameEntitiesPanel"; }
#else 
        const virtual std::wstring getClassName() const { return L"GameEntitiesPanel"; }
#endif

	private:

        int OnSize(const WPARAM& wParam, const LPARAM& lParam);

        void sizeGroupBox(HDWP& hDWP, const bool doCharacters, const CRect& dimensions, 
                          const WindowMetrics::ControlSpacing& cs,
                          const WindowMetrics::ControlDimensions& cd);

		// Disable copy construction and assignment operator
        GameEntitiesPanel(const GameEntitiesPanel&);
        GameEntitiesPanel& operator = (const GameEntitiesPanel&);

        // Member Variables

        GameWorldController*    mainWindowController;
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

        void clearLists();
        void updateCharacterList(const std::vector<GameCharacter>& characterList, const bool& playerHere);
        void updateObjectList(const std::vector<GameObject>& objectList);

    protected:
        
        virtual int OnCreate(CREATESTRUCT& cs);
        virtual void PreCreate(CREATESTRUCT& cs);
        
#ifdef __WIN9X_COMPAT__
        const virtual std::string getClassName() const { return "EntitiesHerePanel"; }
#else 
        const virtual std::wstring getClassName() const { return L"EntitiesHerePanel"; }
#endif

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

        RoadPalettePanel(MainWindowInterface* inMainWindow, GameWorldController* gwc);
        virtual ~RoadPalettePanel();
        
        void onDrawingTileChanged();
        void setTileset(CBitmap& inTileSet);
        void setZoomFactor(const int& newFactor);

        const int& getScaledTileWidth() const { return scaledTileWidth; }

    protected:

#ifdef __WIN9X_COMPAT__
        const virtual std::string getClassName() const { return "RoadPalettePanel"; }
#else 
        const virtual std::wstring getClassName() const { return L"RoadPalettePanel"; }
#endif
        
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
        
        int zoomFactor;
        int tileWidth;
        int tileHeight;
        int scaledTileWidth;
        int scaledTileHeight;

		// Disable copy construction and assignment operator
        RoadPalettePanel(const RoadPalettePanel&);
        RoadPalettePanel& operator = (const RoadPalettePanel&);
};

///----------------------------------------------------------------------------
/// GameMapPanel - Displays the game world's map.
///----------------------------------------------------------------------------

class GameMapPanel : public ScrollPanelBaseClass {
	
	public:

        GameMapPanel(Win32ppMainWindowInterface* inMainWindow, GameWorldController* gwc);
        virtual ~GameMapPanel();
        
        void onMapSizeChanged();
        void onNewTileSelected();
        void onTileUpdated();
        
        void setTileset(CBitmap& inTileSet);
        void updateBackBuffer();

        void setZoomFactor(const int& newFactor);

    protected:

#ifdef __WIN9X_COMPAT__
        const virtual std::string getClassName() const { return "GameMapPanel"; }
#else 
        const virtual std::wstring getClassName() const { return L"GameMapPanel"; }
#endif

        const LONG getClassStyles() const { return CS_DBLCLKS; }

        virtual int     OnCreate(CREATESTRUCT& cs);
        virtual void    OnDraw(CDC& dc);
        virtual LRESULT WndProc(UINT msg, WPARAM wParam, LPARAM lParam);

	private:

        LRESULT onLButtonDown(const WORD& xPos, const WORD& yPos);
        LRESULT onLButtonDBLClick(const WORD& xPos, const WORD& yPos);
        LRESULT onRButtonDown(const WORD& xPos, const WORD& yPos);
        LRESULT onKeyDown(const WORD& vKey, const WORD& keyData);

        void updateScrollSize();

		// Disable copy construction and assignment operator
        GameMapPanel(const GameMapPanel&);
        GameMapPanel& operator = (const GameMapPanel&);

        CMemDC			           tilesetDC;
        CMemDC      backBufferDC;
        CBitmap     backBufferBMP;
        
        Win32ppMainWindowInterface* const mainWindow;
        GameWorldController* const  gameWorldController;

        int zoomFactor;
        int tileWidth;
        int tileHeight;
        int scaledTileWidth;
        int scaledTileHeight;

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
        GameMapDocker(Win32ppMainWindowInterface* mainWindow, GameWorldController* gwc,
                      const CBitmap* inTileSet) : view(mainWindow, gwc) {

            SetView(view);
            SetBarWidth(6);

        }
        virtual ~GameMapDocker() {}

    private:

        GameMapPanel view;

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
