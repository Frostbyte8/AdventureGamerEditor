#ifndef __EDITCHARACTER_DIALOG_H__
#define __EDITCHARACTER_DIALOG_H__

#include <wxx_wincore.h>
#include <wxx_stdcontrols.h>
#include <wxx_imagelist.h> // Needed for wxx_tab
#include <wxx_tab.h>

#include "../model/gameobject.h"
#include "../model/gamecharacter.h"
#include "../model/gamemap.h"

#include "../win32/window_metrics.h"
#include "../interface/mainwindow_interface.h"
#include "editcharacter_tabviews.h"

class EditCharacterDialog : public CWnd {

    public:

        EditCharacterDialog(MainWindowInterface* inMainWindow, WindowMetrics* inWindowMetrics, const GameMap* inGameMap, bool inEditCharacter);

        const CSize& getContentSize() const { return contentSize; }

        void SetParentWindow(const HWND hWnd) {
            parentWindow = hWnd;
        }

        void DoStuff() {
            if(parentWindow != 0) {
                ::EnableWindow(parentWindow, FALSE);
            }
        }

        void SetCharacterToEdit(const GameCharacter& gameCharacter) {

            if(descriptionsTab) {
                descriptionsTab->populateFields(gameCharacter, *gameMap);
                qualitiesTab->populateFields(gameCharacter, *gameMap);
                attributesTab->populateFields(gameCharacter, *gameMap);
                miscTab->populateFields(gameCharacter, *gameMap);
            }
        }

    protected:

        virtual void OnClose(); 
        virtual int OnCreate(CREATESTRUCT& cs);
        virtual void PreRegisterClass(WNDCLASS& wc);
        virtual LRESULT WndProc(UINT msg, WPARAM wParam, LPARAM lParam);

    private:

        static bool CALLBACK SetProperFont(HWND child, LPARAM font);

        HWND                            parentWindow;
        MainWindowInterface*            mainWindow;
        const GameMap*                  gameMap;
        WindowMetrics*                  windowMetrics;

        EditCharacterDescriptionsTab*   descriptionsTab;
        EditCharacterQualitiesTab*      qualitiesTab;
        EditCharacterAttributesTab*     attributesTab;
        EditCharacterMiscTab*           miscTab;
        CTab                            tabControl;
        CSize                           contentSize;
};

#endif // __EDITCHARACTER_DIALOG_H__
