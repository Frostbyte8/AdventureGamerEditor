#ifndef __EDITOBJECT_TABVIEWS_H__
#define __EDITOBJECT_TABVIEWS_H__

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

namespace EditObjectDialogConstants {

    namespace DescriptionsTab {
        namespace ControlIDs {

            const WORD BrowseIcon       = 101;
            const WORD BrowseSound      = 102;
            const WORD Name             = 103;
            const WORD Sight            = 104;
            const WORD OnUse            = 105;
            const WORD OnLastUse        = 106;

        }
    }

    namespace QualitiesTab {
        namespace ControlIDs {

            const WORD MasterKey    = 101;
            const WORD Invisible    = 102;
            const WORD Climber      = 103;
            const WORD Protection   = 104;
            const WORD Torch        = 105;
            const WORD Worn         = 106;
            const WORD Fixed        = 107;
            const WORD Money        = 108;
            const WORD UsesBox      = 109;
            const WORD MoneyBox     = 110;
            const WORD UsedWith     = 111;

        }
    }

    namespace EffectsTab {
        namespace ControlIDs {

            const WORD EnergyBase            = 101;
            const WORD EnergyRandom          = 102;
            const WORD SkillBase             = 103;
            const WORD SkillRandom           = 104;
            const WORD WillpowerBase         = 105;
            const WORD WillpowerRandom       = 106;
            const WORD LuckBase              = 107;
            const WORD LuckRandom            = 108;
            const WORD TorchLifeBase         = 109;
            const WORD TorchLifeRandom       = 110;
            const WORD MakesSight            = 111;
            const WORD MakesHearing          = 112;
            const WORD EffectsConsume        = 113;
            const WORD EffectsTemp           = 114;
            const WORD EnergyPosBTN          = 115;
            const WORD EnergyNegBTN          = 116;
            const WORD SkillPosBTN           = 117;
            const WORD SkillNegBTN           = 118;
            const WORD WillpowerPosBTN       = 119;
            const WORD WillpowerNegBTN       = 120;
            const WORD LuckPosBTN            = 121;
            const WORD LuckNegBTN            = 122;
            const WORD TorchPosBTN           = 123;
            const WORD TorchNegBTN           = 124;

        }
    }

    namespace LocationsTab {
        namespace ControlIDs {

            const WORD OnGround         = 101;
            const WORD OnPlayer         = 102;
            const WORD OnCharacter      = 103;
            const WORD UnlocksDoor      = 104;
            const WORD XGroundText      = 105;
            const WORD YGroundText      = 106;
            const WORD XDoorText        = 107;
            const WORD YDoorText        = 108;
            const WORD WhichCharacter   = 109;

        }
    }
}

//=============================================================================
// EOTabViewBase - The base class for all tab pages within the Edit Object 
// Dialog Window.
//=============================================================================

class EOTabViewBase : public CWnd {

    public:

        EOTabViewBase(EditDialogBase* inParent) : parentWindow(inParent) {}

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
        virtual InputValidator* validateFields() = 0;

        virtual void PreCreate(CREATESTRUCT& cs) {
            cs.dwExStyle |= WS_EX_CONTROLPARENT;
        }

    protected:
        
        LONG pageWidth;
        LONG pageHeight;
        EditDialogBase*     parentWindow;

};

//=============================================================================
// EditObjectDescriptionsTab - Contains the descriptions associated with the
// object, as well what icon to use, and what (unused) sound to play.
//=============================================================================

class EditObjectDescriptionsTab : public EOTabViewBase {

    public:
        
        // Constructor
        EditObjectDescriptionsTab(EditDialogBase* inParent) : EOTabViewBase(inParent) {} 

        // Pure Virtual Functions (implemented) 
        virtual void calculatePageWidth(const WindowMetrics& windowMetrics);
        virtual void insertData(GameObject::Builder& builder);
        virtual void populateFields(const GameObject& gameObject, const GameMap& gameMap);
        virtual void moveControls(const WindowMetrics& windowMetrics);
        virtual InputValidator* validateFields();

    protected:
        
        virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
        virtual int OnCreate(CREATESTRUCT& cs);
        virtual void PreRegisterClass(WNDCLASS& wc);

    private:

        CButton                 grpDescriptions;
        CStatic                 lblDescriptions[6];
        CAnsiEdit               txtDescriptions[6];
        CButton                 btnBrowse[2];
        CStatic                 picIcon;

        StringValidator         iconDescValidator;
        StringValidator         soundDescValidator;
                
};

//=============================================================================
// EditObjectQualitiesTab - Contains Flags/Properties that make up the object. 
//=============================================================================

class EditObjectQualitiesTab : public EOTabViewBase {

    public:

        // Constructor
        EditObjectQualitiesTab(const GameMap* inGameMap, EditDialogBase* inParent) : EOTabViewBase(inParent), gameMap(inGameMap) { }

        const BOOL isFixedFlagChecked() const {
            return btnFlags[6].GetCheck() == BST_CHECKED;
        }

        // Pure Virtual Functions (implemented)
        virtual void calculatePageWidth(const WindowMetrics& windowMetrics);
        virtual void insertData(GameObject::Builder& builder);
        virtual void populateFields(const GameObject& gameObject, const GameMap& gameMap); 
        virtual void moveControls(const WindowMetrics& windowMetrics);
        virtual InputValidator* validateFields();

    protected:
        
        virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
        virtual int OnCreate(CREATESTRUCT& cs);
        virtual void PreRegisterClass(WNDCLASS& wc);

    private:

        void flagsChanged(const WORD& ctrlID, const WORD& ctrlAction);
        void updatePropertyValue(const WORD& ctrlID);

        const GameMap*          gameMap;

        CButton                 grpFlags;
        CButton                 grpProperties;
        CButton                 btnFlags[8];

        CStatic                 lblProperties[3];
        CEdit                   txtProperties[2];
        CSpinButton             spnProperties[2];

        CComboBox               cbxUsedWith;

        IntegerValidator        moneyValidator;
        IntegerValidator        usesValidator;
};

//=============================================================================
// EditObjectEffectsTab - Contains options for how the object effects the
// player when the object is used, as well as how the object is used.
//=============================================================================

class EditObjectEffectsTab : public EOTabViewBase {

    public:
        EditObjectEffectsTab(EditDialogBase* inParent) : EOTabViewBase(inParent) {} 
        // Pure Virtual Functions (implemented)
        virtual void calculatePageWidth(const WindowMetrics& windowMetrics);
        virtual void insertData(GameObject::Builder& builder);
        virtual void populateFields(const GameObject& gameObject, const GameMap& gameMap); 
        virtual void moveControls(const WindowMetrics& windowMetrics);
        virtual InputValidator* validateFields() { return NULL; }

    protected:

        virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
        virtual int OnCreate(CREATESTRUCT& cs);
        virtual void PreRegisterClass(WNDCLASS& wc);
        
    private:

        void updateAttributeValue(const WORD& ctrlID);

        CButton                 grpEffects;
        CButton                 btnEffect[2];

        CButton                 grpAttrib;

        CStatic                 lblAttribType[5];
        CStatic                 lblAttribHeading[2];
        CButton                 btnAttribPolarity[10];
        CEdit                   txtAttribAmount[10];
        CSpinButton             spnAttribAmount[10];

        CStatic                 lblSenses[2];
        CComboBox               cbxSenses[2];

};

//=============================================================================
// EditObjectEffectsTab - Contains options for where the object is positioned,
// if it unlocks a door, and where that door might be.
//=============================================================================

class EditObjectLocationsTab : public EOTabViewBase {

    public:

        // Constructor
        EditObjectLocationsTab(const GameMap* inGameMap, EditDialogBase* inParent) : EOTabViewBase(inParent), gameMap(inGameMap) {}

        // Pure Virtual Functions (implemented)
        virtual void calculatePageWidth(const WindowMetrics& windowMetrics);
        virtual void insertData(GameObject::Builder& builder);
        virtual void populateFields(const GameObject& gameObject, const GameMap& gameMap); 
        virtual void moveControls(const WindowMetrics& windowMetrics);
        virtual InputValidator* validateFields();

        void canBeHeldByEntities(const BOOL canHold);

    protected:

        virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
        virtual int OnCreate(CREATESTRUCT& cs);
        virtual void PreRegisterClass(WNDCLASS& wc);

    private:

        inline void locatedAtChanged(const WORD& ctrlID, const WORD& ctrlAction);
        inline void toggleUnlocksDoor(const BOOL& doesUnlock);

        const GameMap*          gameMap;

        CButton                 grpLocations;

        CButton                 btnLocatedAt[3];

        CStatic                 lblGroundCoord[2];
        CEdit                   txtGroundCoord[2];
        IntegerValidator        groundCoordValidator[2];

        CComboBox               cbxWhichCharacter;

        CButton                 btnUnlocksDoor;
        CStatic                 lblDoorCoord[2];
        CEdit                   txtDoorCoord[2];
        IntegerValidator        doorCoordVaildator[2];



};

#endif // __EDITOBJECT_TABVIEWS_H__
