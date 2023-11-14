#ifndef __EDITCHARACTER_TABVIEWS_H__
#define __EDITCHARACTER_TABVIEWS_H__

#include <wxx_wincore.h>
#include <wxx_stdcontrols.h>

#include "../../util/languagemapper.h"
#include "../../util/win32pp_extras.h"
#include "../../util/inputvalidator.h"
#include "../../win32/window_metrics.h"

#include "../../model/gameobject.h"
#include "../../model/gamecharacter.h"
#include "../../model/gamemap.h"

#include "../editdialog_base.h"

namespace EditCharacterDialogConstants {

    namespace DescriptionsTab {
        namespace ControlIDs {

            const WORD NameText     = 101;
            const WORD OnSightText  = 102;
            const WORD OnFightText  = 103;
            const WORD OnDeathText  = 104;
            const WORD BrowseIcon   = 105;
            const WORD BrowseSound  = 106;

        }
    }

    namespace QualitiesTab {
        namespace ControlIDs {
            const WORD MoneyOnHand      = 101;
            const WORD CharType         = 102;
            const WORD FlagEnterDark    = 103;
            const WORD FlagEnterHazard  = 104;
            const WORD FlagCanJump      = 105;
            const WORD FlagBypassGates  = 106;
            const WORD FlagBypassDoors  = 107;
            const WORD FlagCanClimb     = 108;
            const WORD FlagWanderer     = 109;
            const WORD FlagStalker      = 110;
        }
    }

    namespace AttributesTab {
        namespace ControlIDs {
            const WORD Energy       = 101;
            const WORD Skill        = 102;
            const WORD Willpower    = 103;
            const WORD Luck         = 104;
            const WORD SightType    = 105;
        }
    }

    namespace MiscTab {
        namespace ControlIDs {
            const WORD XTextCoord   = 101;
            const WORD YTextCoord   = 102;
        }
    }
}

//=============================================================================
// ECTabViewBase - The base class for all tab pages within the Edit Character 
// Dialog Window.
//=============================================================================

class ECTabViewBase : public CWnd {

    public:

        ECTabViewBase(EditDialogBase* inParent) : parentWindow(inParent) {}

        // Accessors
        const LONG& getPageWidth() const { return pageWidth; }
        const LONG& getPageHeight() const { return pageHeight; }

        // Pure Virtual Functions
        virtual void calculatePageWidth(const WindowMetrics& windowMetrics) = 0;
        virtual void insertData(GameCharacter::Builder& builder) = 0;
        virtual void populateFields(const GameCharacter& gameCharacter) = 0;
        virtual void moveControls(const WindowMetrics& windowMetrics) = 0;
        virtual InputValidator* validateFields() = 0;

    protected:

        LONG pageWidth;
        LONG pageHeight;
        EditDialogBase*     parentWindow;
};

//=============================================================================
// EditCharacterDescriptionsTab - Contains descriptions associated with the
// character
//=============================================================================

class EditCharacterDescriptionsTab : public ECTabViewBase {

    public:

        EditCharacterDescriptionsTab(EditDialogBase* inParent) : ECTabViewBase(inParent) {}

        // Pure Virtual Functions
        virtual void calculatePageWidth(const WindowMetrics& windowMetrics);
        virtual void insertData(GameCharacter::Builder& builder);
        virtual void populateFields(const GameCharacter& gameCharacter);
        virtual void moveControls(const WindowMetrics& windowMetrics);
        virtual InputValidator* validateFields();

    protected:

        virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
        virtual int OnCreate(CREATESTRUCT& cs);
        virtual void PreRegisterClass(WNDCLASS& wc);

    private:
        
        StringValidator     iconDescValidator;
        StringValidator     soundDescValidator;

        CButton             grpDescriptions;
        CStatic             lblDescriptions[6];
        CAnsiEdit           txtDescriptions[6];
        CButton             btnBrowse[2];

};

//=============================================================================
// EditCharacterQualitesTab - Contains information about the qualities the
// character has
//=============================================================================

class EditCharacterQualitiesTab : public ECTabViewBase {

    public:
        
        EditCharacterQualitiesTab(const GameMap* inGameMap, EditDialogBase* inParent) : ECTabViewBase(inParent), gameMap(inGameMap) {}
        
        // Pure Virtual Functions
        virtual void calculatePageWidth(const WindowMetrics& windowMetrics);
        virtual void insertData(GameCharacter::Builder& builder);
        virtual void populateFields(const GameCharacter& gameCharacter);
        virtual void moveControls(const WindowMetrics& windowMetrics);
        virtual InputValidator* validateFields();

    protected:

        virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
        virtual int OnCreate(CREATESTRUCT& cs);
        virtual void PreRegisterClass(WNDCLASS& wc);

    private:

        const GameMap*      gameMap;
        CButton             grpFlags;
        CButton             btnFlags[8];

        CButton             grpProperties;
        CStatic             lblMoney;
        CEdit               txtMoney;
        CSpinButton         spnMoney;
        CStatic             lblType;
        CComboBox           cbxType;

        IntegerValidator    moneyValidator;


};

//=============================================================================
// EditCharacterAttributesTab - Contains information about the Attributes a
// Character has
//=============================================================================

class EditCharacterAttributesTab : public ECTabViewBase {

    public:

        EditCharacterAttributesTab(EditDialogBase* inParent) : ECTabViewBase(inParent) {}

        // Pure Virtual Functions

        virtual void calculatePageWidth(const WindowMetrics& windowMetrics);
        virtual void insertData(GameCharacter::Builder& builder);
        virtual void populateFields(const GameCharacter& gameCharacter);
        virtual void moveControls(const WindowMetrics& windowMetrics);
        virtual InputValidator* validateFields();

    protected:

        virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
        virtual int OnCreate(CREATESTRUCT& cs);
        virtual void PreRegisterClass(WNDCLASS& wc);

    private:

        CButton             grpAttrib;

        CStatic             lblAttribType[4];
        CEdit               txtAttribType[4];
        CSpinButton         spnAttribType[4];

        CStatic             lblSight;
        CComboBox           cbxSight;

        IntegerValidator    attributeValidator[4];

};

//=============================================================================
// EditCharacterMiscTab - Contains the remaining misc information about a
// Character
//=============================================================================

class EditCharacterMiscTab : public ECTabViewBase {

   public:

        EditCharacterMiscTab(const GameMap* inGameMap, EditDialogBase* inParent) : ECTabViewBase(inParent), gameMap(inGameMap) {}

        // Pure Virtual Functions
        virtual void calculatePageWidth(const WindowMetrics& windowMetrics);
        virtual void insertData(GameCharacter::Builder& builder);
        virtual void populateFields(const GameCharacter& gameCharacter);
        virtual void moveControls(const WindowMetrics& windowMetrics);
        virtual InputValidator* validateFields();

    protected:

        virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
        virtual int OnCreate(CREATESTRUCT& cs);
        virtual void PreRegisterClass(WNDCLASS& wc);

    private:

        const GameMap*      gameMap;

        CButton             grpLocations;
        CStatic             lblCoords[2];
        CEdit               txtCoords[2];
        CButton             grpInventory;
        CListBox            lsbInventory;

        IntegerValidator    coordValidator[2];

};

#endif // __EDITCHARACTER_TABVIEWS_H__
