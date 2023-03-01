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
        GameMapView(GameWorldController* gwc) : backBufferDC(NULL), tilesetDC(NULL), gameWorldController(gwc) {}
        virtual ~GameMapView() {}

        const bool isBMPLoaded() const {
            return tilesetBMP.GetHandle();
        }

        virtual int OnCreate(CREATESTRUCT& cs) {
            
            tilesetBMP.LoadImage(L"tileset.bmp", LR_LOADFROMFILE | LR_CREATEDIBSECTION);
            CSize abc(512, 512);
            SetScrollSizes(abc); // Otherwise it won't work.
            CBrush outofbounds(RGB(255,0,0));
            SetScrollBkgnd(outofbounds);

            CClientDC dc(*this);
            CBitmap oldBMP;

            tilesetDC = CMemDC(dc);
            tilesetDC.SelectObject(tilesetBMP);

            backBufferBMP = CreateCompatibleBitmap(dc, 512, 512);
            backBufferDC = CMemDC(dc);

            oldBMP = backBufferDC.SelectObject(backBufferBMP);
            backBufferDC.BitBlt(0,0,128,128,tilesetDC,0,0,SRCCOPY);
            backBufferDC.SelectObject(oldBMP);

            return CScrollView::OnCreate(cs);
        }

        virtual void OnInitialUpdate() {
        }

        // void onZoomChange()

        void doSomething() {
            CBitmap oldBMP;
            oldBMP = backBufferDC.SelectObject(backBufferBMP);
            backBufferDC.BitBlt(0,0,128,128,tilesetDC,128,0,SRCCOPY);
            backBufferDC.SelectObject(oldBMP);
            InvalidateRect(NULL);
        }

        virtual void OnDraw(CDC& dc) {
            
            if(backBufferBMP.GetHandle()) {
                dc.SelectObject(backBufferBMP);
            }
        }

        virtual LRESULT WndProc(UINT msg, WPARAM wparam, LPARAM lparam) {
            if(msg == WM_LBUTTONDOWN) {
                doSomething();
                return 0;
            }
            return WndProcDefault(msg, wparam, lparam);
        }

	private:

		// Disable copy construction and assignment operator
	    GameMapView(const GameMapView&);
		GameMapView& operator = (const GameMapView&);

        CBitmap     tilesetBMP;
        CMemDC      tilesetDC;
        CMemDC      backBufferDC;
        CBitmap     backBufferBMP;
        
        GameWorldController*    gameWorldController;
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
