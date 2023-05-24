#ifndef __EDITOR_MAINWINDOW_H__
#define __EDITOR_MAINWINDOW_H__

#include <wxx_wincore.h>
#include <wxx_dockframe.h>
#include <wxx_stdcontrols.h>
#include "editor_mainwindow_dockers.h"
#include "../interface/mainwindow_interface.h"
#include "../controller/gameworld_controller.h"
#include "../win32/window_metrics.h"
#include "editobject_dialog.h"
#include "editcharacter_dialog.h"
#include "edit_worldinfo.h"
#include "editstory_dialog.h"

class MainWindowFrame : public CDockFrame, public MainWindowInterface {

	public:

		MainWindowFrame();
		virtual ~MainWindowFrame();
		virtual HWND Create(HWND parent = 0);

        // Public Interface Functions
        virtual int askYesNoQuestion(const std::string& question, const std::string& title,
                                     bool allowCancel = true);

        virtual void displayErrorMessage(const std::string& message, const std::string& title);

        virtual void finishedEditCharacterDialog(const int& alterType, const bool& wasCanceled);
        virtual void finishedEditObjectDialog(const int& alterType, const bool& wasCanceled, const bool& pressedApply);
        virtual void finishedEditStoryDialog(const bool& wasCanceled, const bool& pressedApply);
        virtual void finishedEditWorldInfoDialog(const bool& wasCanceled);
                
        virtual void onAlterCharacter(const int& alterType, const size_t& index);
        virtual void onAlterObject(const int& alterType, const size_t& index);
        virtual void onEditStory();
        virtual void onEditWorldInfo();

	protected:

        virtual BOOL    OnCommand(WPARAM wparam, LPARAM lparam);
		virtual int     OnCreate(CREATESTRUCT& cs);
		virtual void    OnInitialUpdate();
		virtual LRESULT WndProc(UINT msg, WPARAM wparam, LPARAM lparam);

	private:

        void CreateMenuBar();

        BOOL OnFileNew();
        BOOL OnFileOpen();

        CMenu mainMenu;
        CMenu fileMenu;
        CMenu editMenu;
        CBitmap tilesetBMP; 

		GameEntitiesView*	    entityView;
		GameMapDocker*          gameMapDocker;
		RoadSelectorDocker*     roadSelectorDocker;
		EntitiesHereDocker*     entitiesHereDocker;

        GameWorldController*    gameWorldController;
		WindowMetrics		    windowMetrics;

        EditObjectDialog*       editObjectDialog;
        EditCharacterDialog*    editCharacterDialog;
        EditWorldInfoDialog*    editWorldInfoDialog;
        EditStoryDialog*        editStoryDialog;

        HWND                    activeWindowHandle;

		// Disable copy construction and assignment operator
		MainWindowFrame(const MainWindowFrame&);
		MainWindowFrame& operator= (const MainWindowFrame&);

};


#endif //  __EDITOR_MAINWINDOW_H__
