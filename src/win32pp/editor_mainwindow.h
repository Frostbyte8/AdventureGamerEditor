#ifndef __EDITOR_MAINWINDOW_H__
#define __EDITOR_MAINWINDOW_H__

#include <wxx_wincore.h>
#include <wxx_dockframe.h>
#include <wxx_stdcontrols.h>
#include "editor_mainwindow_win32pp_interface.h"
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

    // Tile Menu

    const WORD FIRST_OF_MAP_ALTER_IDS   = 210;

    const WORD TilePopupMenu            = 210;
    const WORD FeatureSubMenu           = 211;
    const WORD EditDescription          = 212;

    // World Menu

    const WORD WorldPopupMenu           = 220;
    const WORD SummaryAndStory          = 221;
    const WORD WorldProperties          = 222;
    const WORD ResizeWorld              = 223;

    // Straight Away Menu

    const WORD AddStart                 = 231;
    const WORD AddFinish                = 232;
    const WORD AddGate                  = 233;
    const WORD AddLockedDoor            = 234;
    const WORD AddBarrierNorth          = 235;
    const WORD AddBarrierSouth          = 236;
    const WORD AddBarrierEast           = 237;
    const WORD AddBarrierWest           = 238;

    // Corners

    const WORD AddOnSwitch              = 241;
    const WORD AddOffSwitch             = 242;

    // Dead-Ends

    const WORD AddJumpPad               = 251;

    // Crossroads Menu

    const WORD AddHazard                = 261;
    const WORD AddSafeHaven             = 262;

    // Feature Sub Menu

    const WORD StraightAwayMenu         = 270;
    const WORD CornerMenu               = 271;
    const WORD DeadEndMenu              = 272;
    const WORD CrossroadsMenu           = 273;
    const WORD FeatureMenuDiv1          = 274;
    const WORD StartJumpConnection      = 275;
    const WORD EndJumpConnection        = 276;
    const WORD StartSwitchConnection    = 277;
    const WORD EndSwitchConnection      = 278;
    const WORD FeatureMenuDiv2          = 279;
    const WORD ToggleTileDarkness       = 280;

    const WORD LAST_OF_MAP_ALTER_IDS    = 281;

}

//=============================================================================
// MainWindowFrame - The Primary view of the program where the map editor
// resides, along with it's child dockers.
//=============================================================================

class MainWindowFrame : public CDockFrame, public MainWindowInterface, public Win32ppMainWindowInterface {

	public:

		MainWindowFrame();
		virtual ~MainWindowFrame();

        // Win32++ Definitions
        virtual HWND Create(HWND parent = 0);
		
        // Public Generic Interface Functions

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

        virtual void onWorldStateChanged();

        virtual bool startSaveDialog();
        virtual void finishedSaveDialog() {};
        virtual void onChangesSaved();

        virtual bool startLoadDialog();
        virtual void finishedLoadDialog() {};

        // Interface Functions For this View Type

        virtual void onGameMapRightClick(const WORD& xPos, const WORD& yPos);

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

        // Interface Helpers
        inline void makeDialogModal(EditDialogBase& dialog, const CString& caption);
        inline bool doSaveOrOpenDialog(const BOOL& isOpen);
        
        // Window Creation Helpers
        inline void appendPopupMenuWithID(CMenu& targetMenu, CMenu& popupMenu, const UINT& id, const bool& enabled);      

        void addKeyboardAccelerators();

        void updateStatusbar(const int& index);
        void updateFeatureMenu(const int& index);
        void CreateMenuBar();
        bool loadTileSet();

        void updateMenuState();
        void updateHereLists(const bool objectsHere, const bool charsHere, 
                             const GameMap* inMap = NULL, const int* row = NULL, 
                             const int* col = NULL);

        void updateTitleBar(const bool changeMadeOnly);

        void updateControlCaptions();

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

        HACCEL accelHandle;
        std::vector<ACCEL> keyboardAccelerators;

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
