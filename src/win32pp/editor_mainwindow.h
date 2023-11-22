#ifndef __EDITOR_MAINWINDOW_H__
#define __EDITOR_MAINWINDOW_H__

#include <wxx_wincore.h>
#include <wxx_dockframe.h>
#include <wxx_stdcontrols.h>
#include "../interface/mainwindow_interface.h"
#include "../controller/gameworld_controller.h"
#include "../win32/window_metrics.h"

// Dockers which contain each panel of related controls
#include "editor_mainwindow_views.h"

// Dialog windows
#include "objectdlg/editobject_dialog.h"
#include "chardlg/editcharacter_dialog.h"
#include "worldinfodlg/edit_worldinfo.h"
#include "storydlg/editstory_dialog.h"
#include "tiledescdlg/edit_tiledescription_dialog.h"
#include "resizedlg/resizeworld_dialog.h"

//=============================================================================
// MainWindowFrame - The Primary view of the program where the map editor
// resides, along with it's child dockers.
//=============================================================================

class MainWindowFrame : public CDockFrame, public MainWindowInterface {

	public:

		MainWindowFrame();
		virtual ~MainWindowFrame();
		virtual HWND Create(HWND parent = 0);

        // Public Interface Functions
        virtual int askYesNoQuestion(const std::string& question, const std::string& title,
                                     bool allowCancel = true);

        virtual void displayErrorMessage(const std::string& message, const std::string& title);

        virtual void onGameObjectsChanged();
        virtual void onGameCharactersChanged();
        
        virtual bool canCreateDialog(const int& whichDialogType) const;
        virtual void onDialogEnd(const int& whichDialogType);

        virtual bool startEditObjectDialog(GameObject::Builder& objectBuilder, const bool editingObject);
        virtual void finishedAlterObjectDialog();      
        
        virtual bool startEditCharacterDialog(GameCharacter::Builder& characterBuilder, const bool isEditingCharacter);
        virtual void finishedAlterCharacterDialog();


        virtual bool startEditTileDescriptionDialog(const std::string& name, const std::string& description);
        virtual void finishedEditTileDescriptionDialog();
        virtual void onTileUpdated(const int& index, const int& tileUpdateFlags);

        virtual bool startEditStoryAndSummaryDialog(const std::string& story, const std::string& summary);
        virtual void finishedEditStoryAndSummaryDialog();

        virtual void onStoryAndSummaryUpdated();

        //


        
        virtual void finishedEditWorldInfoDialog();
        virtual void finishedResizeWorldDialog();
        
        

        virtual bool onSelectedTileChanged(const int& row, const int& col);

        virtual void onEditWorldInfo();
        virtual void onResizeWorld();
        virtual int onSaveFileDialog(std::string& filePath, std::string& fileName);

        virtual void RecalcDockLayout();

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

        inline void makeDialogModal(EditDialogBase& dialog, const CString& caption);

        void addJumpConnection(const int& whichPoint);
        void addSwitchConnection(const int& whichPoint);
        void updateStatusbar(const int& index);
        void updateFeatureMenu(const int& index);
        void CreateMenuBar();
        bool loadTileSet();

        BOOL OnFileNew();
        BOOL OnFileOpen();

        CMenu mainMenu;
        CMenu fileMenu;
        CMenu editMenu;
        CMenu featureMenu;
        CMenu straightAwayMenu;
        CMenu cornerMenu;
        CMenu deadendMenu;
        CMenu crossroadMenu;

        bool isSizing;

        CBitmap tilesetBMP; 

        int     tileWidth;
        int     tileHeight;

		GameEntitiesPanel*              entityView;
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
        ResizeWorldDialog*              resizeWorldDialog;

        HWND                            activeWindowHandle;

		// Disable copy construction and assignment operator
		MainWindowFrame(const MainWindowFrame&);
		MainWindowFrame& operator= (const MainWindowFrame&);

};


#endif //  __EDITOR_MAINWINDOW_H__
