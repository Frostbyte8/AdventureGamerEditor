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

namespace MenuIDs {

    // File Menu

    const WORD FilePopupMenu            = 200;
    const WORD NewFile                  = 201;
    const WORD OpenFile                 = 202;
    const WORD SaveFile                 = 203;
    const WORD SaveFileAs               = 204;
    const WORD FileMenuDiv1             = 205;
    const WORD ExitItem                 = 206;

    const WORD WorldPopupMenu           = 210;
    const WORD SummaryAndStory          = 211;
    const WORD WorldProperties          = 212;
    const WORD ResizeWorld              = 213;

    const WORD TilePopupMenu            = 220;
    const WORD FeatureSubMenu           = 221;
    const WORD EditDescription          = 222;

    // For these IDs, we'll just obtain their MOD flag count.

    // Straight Aways

    const WORD AddStart                 = 308;      // MOD1
    const WORD AddFinish                = 309;      // MOD2
    const WORD AddGate                  = 310;      // MOD1+MOD2.
    const WORD AddLockedDoor            = 311;      // MOD3.

    const WORD AddBarrierSouth          = 312;      // MOD1+MOD3
    const WORD AddBarrierNorth          = 313;      // MOD2+MOD3

    const WORD AddBarrierEast           = 312;      // MOD1+MOD3
    const WORD AddBarrierWest           = 313;      // MOD2+MOD3

    // Corners

    const WORD AddSwitchOn              = 308;      // MOD1
    const WORD AddSwitchOff             = 309;      // MOD2

    // Dead Ends

    const WORD AddJumpPad               = 310;      // MOD1+MOD2

    // Cross Roads

    const WORD AddHazard                = 311;      // MOD3
    const WORD AddSafeHaven             = 314;      // ALLMODS

    // Additional Menu Items

    const WORD FirstJumpConnection      = 315;
    const WORD SecondJumpConnection     = 316;

    const WORD startSwitchConnection    = 317;
    const WORD endSwitchConnection      = 318;

    const WORD MakeTileDark             = 319;

}

//=============================================================================
// MainWindowFrame - The Primary view of the program where the map editor
// resides, along with it's child dockers.
//=============================================================================

class MainWindowFrame : public CDockFrame, public MainWindowInterface {

	public:

		MainWindowFrame();
		virtual ~MainWindowFrame();

        // Win32++ Definitions
        virtual HWND Create(HWND parent = 0);
		
        // Public Interface Functions

        virtual int askYesNoQuestion(const std::string& question, const std::string& title,
                                     bool allowCancel = true);

        virtual void displayErrorMessage(const std::string& message, const std::string& title);

        virtual void displayMessage(const std::string& message, const std::string& title, const int& type);

        virtual void onEntitiesChanged(const bool& objectsListChanged, const bool& objectsHereChanged,
                                       const bool& charsListChanged, const bool& charsHereChanged);
        
        virtual bool canCreateDialog(const int& whichDialogType) const;
        virtual void onDialogEnd(const int& whichDialogType);

        virtual bool startEditObjectDialog(GameObject::Builder& objectBuilder, const bool editingObject);
        virtual void finishedAlterObjectDialog();      
        
        virtual bool startEditCharacterDialog(GameCharacter::Builder& characterBuilder, const bool isEditingCharacter);
        virtual void finishedAlterCharacterDialog();

        virtual bool startEditTileDescriptionDialog(const std::string& name, const std::string& description);
        virtual void finishedEditTileDescriptionDialog();
        virtual void onTileUpdated(const int& index, const int& tileUpdateFlags);

        virtual void onSelectedTileChanged();
        virtual void onDrawingTileChanged();

        virtual bool startEditStoryAndSummaryDialog(const std::string& story, const std::string& summary);
        virtual void finishedEditStoryAndSummaryDialog();
        virtual void onStoryAndSummaryUpdated();

        virtual bool startEditWorldInfoDialog(const GameInfo& gameInfo);
        virtual void finishedEditWorldInfoDialog();
        virtual void onWorldInfoUpdated();

        virtual bool startResizeWorldDialog(const int& numRows, const int& numCols);
        virtual void finishedResizeWorldDialog();
        virtual void onWorldResized();
        
        // Functions that are being cleaned

        virtual int onSaveFileDialog(std::string& filePath, std::string& fileName);       

	protected:

        // Win32++ Definitions
        virtual void RecalcDockLayout();

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

        inline void appendPopupMenuWithID(CMenu& targetMenu, CMenu& popupMenu, const UINT& id);

        void updateStatusbar(const int& index);
        void updateFeatureMenu(const int& index);
        void CreateMenuBar();
        bool loadTileSet();

        void updateHereLists(const bool objectsHere, const bool charsHere, 
                             const GameMap* inMap = NULL, const int* row = NULL, 
                             const int* col = NULL);

        void updateTitleBar(const bool changeMadeOnly);

        void updateControlCaptions();

        BOOL OnFileNew();
        BOOL OnFileOpen();

        CString originalWindowTitle;

        CMenu mainMenu;
        CMenu fileMenu;
        CMenu worldMenu;

        CMenu tileMenu;


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
