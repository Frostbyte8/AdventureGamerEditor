#ifndef __EDITOR_MAINWINDOW_VIEWS_H__
#define __EDITOR_MAINWINDOW_VIEWS_H__

#include <wxx_wincore.h>
#include <wxx_docking.h>
#include <wxx_scrollview.h>
#include "../controller/gameworld_controller.h"
#include <vector>

///----------------------------------------------------------------------------
/// GameEntitiesView - Shows Objects and Characters defined by the game world.
/// This is the right-most panel.
///----------------------------------------------------------------------------

class GameEntitiesView : public CWnd {

	public:
		GameEntitiesView() {}
		virtual ~GameEntitiesView() {}

	private:

		// Disable copy construction and assignment operator
	    GameEntitiesView(const GameEntitiesView&);
		GameEntitiesView& operator = (const GameEntitiesView&);
};

///----------------------------------------------------------------------------
/// RoadSelectorView - Show all the road tiles avaliable
///----------------------------------------------------------------------------

class RoadSelectorView : public CWnd {

	public:
		RoadSelectorView() {}
		virtual ~RoadSelectorView() {}
        virtual void PreRegisterClass(WNDCLASS& wc) {
            wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
            wc.lpszClassName = L"RoadSelectorView";
            wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
        }

	private:

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

        const bool isBMPLoaded() const;
        const int& getTileWidth() const { return tileWidth; }
        const int& gettileHeight() const { return tileHeight; }
        //void onZoomChange();
        void UpdateBackBuffer();

    protected:
        virtual int OnCreate(CREATESTRUCT& cs);
        virtual void OnDraw(CDC& dc);
        virtual LRESULT WndProc(UINT msg, WPARAM wParam, LPARAM lParam);

	private:

		// Disable copy construction and assignment operator
	    GameMapView(const GameMapView&);
		GameMapView& operator = (const GameMapView&);

        CMemDC      tilesetDC;
        CBitmap     tilesetBMP;
        
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
		EntitiesHereView() {}
		virtual ~EntitiesHereView() {}

	private:

		// Disable copy construction and assignment operator
	    EntitiesHereView(const EntitiesHereView&);
		EntitiesHereView& operator = (const EntitiesHereView&);
};


#endif // __EDITOR_MAINWINDOW_VIEWS_H__
