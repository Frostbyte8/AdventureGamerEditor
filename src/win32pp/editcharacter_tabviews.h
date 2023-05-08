#ifndef __EDITCHARACTER_TABVIEWS_H__
#define __EDITCHARACTER_TABVIEWS_H__

#include <wxx_wincore.h>
#include <wxx_stdcontrols.h>

#include "../util/languagemapper.h"
#include "../util/win32pp_extras.h"
#include "../win32/window_metrics.h"

#include "../model/gameobject.h"
#include "../model/gamecharacter.h"
#include "../model/gamemap.h"


//=============================================================================
// ECTabViewBase - The base class for all tab pages within the Edit Character 
// Dialog Window.
//=============================================================================

class ECTabViewBase : public CWnd {

    public:

        // Accessors
        const LONG& getPageWidth() const { return pageWidth; }
        const LONG& getPageHeight() const { return pageHeight; }

        // Pure Virtual Functions
        // TODO: Give moveControls a WindowMetrics object so it's consistent with
        // the DPI settings of the monitor it is on.
        virtual void calculatePageWidth(const WindowMetrics& windowMetrics) = 0;
        virtual void insertData(GameObject::Builder& builder) = 0;
        virtual void populateFields(const GameObject& gameObject, const GameMap& gameMap) = 0;
        virtual void moveControls(const WindowMetrics& windowMetrics) = 0;
        virtual WORD validateFields() = 0;

    protected:

        LONG pageWidth;
        LONG pageHeight;
};

//=============================================================================
// EditCharacterDescriptionsTab - Contains descriptions associated with the
// character
//=============================================================================

class EditCharacterDescriptionsTab : public ECTabViewBase {

    public:
        // Pure Virtual Functions (implemented) 
        virtual void calculatePageWidth(const WindowMetrics& windowMetrics) {}
        virtual void insertData(GameObject::Builder& builder) {}
        virtual void populateFields(const GameObject& gameObject, const GameMap& gameMap) {}
        virtual void moveControls(const WindowMetrics& windowMetrics) {}
        virtual WORD validateFields() {}

    protected:
        //virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
        virtual int OnCreate(CREATESTRUCT& cs);
        virtual void PreRegisterClass(WNDCLASS& wc);
        //virtual BOOL PreTranslateMessage(MSG& msg);
};

#endif // __EDITCHARACTER_TABVIEWS_H__