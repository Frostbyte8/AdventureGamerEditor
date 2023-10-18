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
#include "edit_tiledescription_dialog.h"

class MainWindowFrame : public CDockFrame, public MainWindowInterface {

	public:

		MainWindowFrame();
		virtual ~MainWindowFrame();
		virtual HWND Create(HWND parent = 0);

        // Public Interface Functions
        virtual int askYesNoQuestion(const std::string& question, const std::string& title,
                                     bool allowCancel = true);

        virtual void displayErrorMessage(const std::string& message, const std::string& title);

        virtual void finishedEditCharacterDialog();
        virtual void finishedEditObjectDialog();
        virtual void finishedEditStoryDialog();
        virtual void finishedEditTileDescriptionDialog();
        virtual void finishedEditWorldInfoDialog();
                
        virtual bool onSelectedTileChanged(const int& row, const int& col);
        virtual void onAlterCharacter(const int& alterType, const size_t& index);
        virtual void onAlterObject(const int& alterType, const size_t& index);
        virtual void onEditStory();
        virtual void onEditTileDescription();
        virtual void onEditWorldInfo();
        virtual int onSaveFileDialog(std::string& filePath, std::string& fileName);

	protected:

        virtual BOOL    OnCommand(WPARAM wparam, LPARAM lparam);
		virtual int     OnCreate(CREATESTRUCT& cs);
		virtual void    OnInitialUpdate();
		virtual LRESULT WndProc(UINT msg, WPARAM wparam, LPARAM lparam);

        static bool CALLBACK SetProperFont(HWND child, LPARAM font) {
            ::SendMessage(child, WM_SETFONT, font, true);
            return true;
        }

	private:

        void CreateMenuBar();
        bool loadTileSet();

        BOOL OnFileNew();
        BOOL OnFileOpen();

        CMenu mainMenu;
        CMenu fileMenu;
        CMenu editMenu;
        CMenu featureMenu;

        CBitmap tilesetBMP; 

        int     tileWidth;
        int     tileHeight;

		GameEntitiesView*               entityView;
		GameMapDocker*                  gameMapDocker;
		RoadSelectorDocker*             roadSelectorDocker;
		EntitiesHereDocker*             entitiesHereDocker;

        GameWorldController*            gameWorldController;
		WindowMetrics                   windowMetrics;

        EditObjectDialog*               editObjectDialog;
        EditCharacterDialog*            editCharacterDialog;
        EditWorldInfoDialog*            editWorldInfoDialog;
        EditStoryDialog*                editStoryDialog;
        EditTileDescriptionDialog*      editTileDescriptionDialog;

        HWND                            activeWindowHandle;

		// Disable copy construction and assignment operator
		MainWindowFrame(const MainWindowFrame&);
		MainWindowFrame& operator= (const MainWindowFrame&);

};


#endif //  __EDITOR_MAINWINDOW_H__
