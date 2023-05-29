#ifndef __EDITOBJECT_TABVIEWS_H__
#define __EDITOBJECT_TABVIEWS_H__

#include <wxx_wincore.h>
#include <wxx_stdcontrols.h>

#include "../util/languagemapper.h"
#include "../util/win32pp_extras.h"
#include "../win32/window_metrics.h"

#include "../model/gameobject.h"
#include "../model/gamecharacter.h"
#include "../model/gamemap.h"

#include "editdialog_base.h"

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
        virtual BOOL PreTranslateMessage(MSG& msg);

    private:

        BOOL onBrowseForMedia(const bool forIcon);

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

        // Constuctor
        EditObjectQualitiesTab(const GameMap* inGameMap, EditDialogBase* inParent) : EOTabViewBase(inParent), gameMap(inGameMap) { }

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
        virtual BOOL PreTranslateMessage(MSG& msg);

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
        virtual BOOL PreTranslateMessage(MSG& msg);
        
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


    protected:

        virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
        virtual int OnCreate(CREATESTRUCT& cs);
        virtual void PreRegisterClass(WNDCLASS& wc);
        virtual BOOL PreTranslateMessage(MSG& msg);

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
