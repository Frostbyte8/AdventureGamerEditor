#ifndef __EDITOBJECT_TABVIEWS_H__
#define __EDITOBJECT_TABVIEWS_H__

#include <wxx_wincore.h>
#include <wxx_stdcontrols.h>
#include "../util/languagemapper.h"
#include "../util/win32pp_extras.h"
#include "../win32/window_metrics.h"
#include "../model/gameobject.h"

inline void EOD_SetWindowText(const unsigned int& ID, CWnd& widget, CString& caption, const LanguageMapper& langMap) {
    caption = AtoW(langMap.get(ID).c_str(), CP_UTF8);
    widget.SetWindowTextW(caption);
}

inline void EOD_SetWindowText(const std::string& str, CWnd& widget, CString& caption) {
    caption = AtoW(str.c_str(), CP_UTF8);
    widget.SetWindowTextW(caption);
}

inline void EOD_AddString(const unsigned int& ID, CComboBox& widget, CString& caption, const LanguageMapper& langMap) {
    caption = AtoW(langMap.get(ID).c_str(), CP_UTF8);
    widget.AddString(caption);
}

/*
struct EditObjectDialogInfo {
    const GameMap* gameMap;
    std::vector<GameObject> gameObjects;
    std::vector<GameCharacter> gameCharacters;
}
*/

class EOTabViewBase : public CWnd {

    public:

        virtual void insertData(GameObject::Builder& builder) = 0;
        virtual WORD validateFields() = 0;
        virtual void populateFields(const GameObject& gameObject) = 0; // Populate Fields
        virtual void moveControls() = 0;
        virtual void calculatePageWidth() = 0;
        LONG& getPageWidth() { return pageWidth; }
        LONG& getPageHeight() { return pageHeight; }

        LONG pageWidth;
        LONG pageHeight;
};

class EditObjectDescriptionsTab : public EOTabViewBase {

    public:
        
        EditObjectDescriptionsTab(WindowMetrics* inWindowMetrics) : windowMetrics(inWindowMetrics) { }
        virtual int OnCreate(CREATESTRUCT& cs);
        virtual void PreRegisterClass(WNDCLASS& wc);
        virtual void moveControls();
        virtual void calculatePageWidth();
        virtual void populateFields(const GameObject& gameObject);
        virtual WORD validateFields() { return 0; }
        virtual void insertData(GameObject::Builder& builder);

    protected:
        
        virtual BOOL PreTranslateMessage(MSG& msg);

    private:

        CButton                 grpDescriptions;
        CStatic                 lblDescriptions[6];
        CAnsiEdit               txtDescriptions[6];
        CButton                 btnBrowse[2];
        CStatic                 picIcon;
        
        WindowMetrics*          windowMetrics;
        
};

class EditObjectQualitiesTab : public EOTabViewBase {

    public:

        EditObjectQualitiesTab(WindowMetrics* inWindowMetrics) : windowMetrics(inWindowMetrics) { }
        virtual int OnCreate(CREATESTRUCT& cs);
        virtual void PreRegisterClass(WNDCLASS& wc);
        virtual void moveControls();
        virtual void calculatePageWidth();
        virtual void populateFields(const GameObject& gameObject); // Populate Fields
        virtual WORD validateFields();
        virtual void insertData(GameObject::Builder& builder);

    protected:
                
        virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
        virtual BOOL PreTranslateMessage(MSG& msg);

    private:

        void flagsChanged(const WORD& ctrlID, const WORD& ctrlAction);
        void updatePropertyValue(const WORD& ctrlID);

        WindowMetrics*          windowMetrics;
        CButton                 grpFlags;
        CButton                 grpProperties;
        CButton                 btnFlags[8];
        CStatic                 lblProperties[3];
        CEdit                   txtProperties[2];
        CSpinButton             spnProperties[2];
        CComboBox               cbxUsedWith;
};

class EditObjectEffectsTab : public EOTabViewBase {

    public:

        EditObjectEffectsTab(WindowMetrics* inWindowMetrics) : windowMetrics(inWindowMetrics) {}
        virtual int OnCreate(CREATESTRUCT& cs);
        virtual void PreRegisterClass(WNDCLASS& wc);
        virtual void moveControls();
        virtual void calculatePageWidth();
        virtual void populateFields(const GameObject& gameObject); // Populate Fields
        virtual WORD validateFields() { return 0; }
        virtual void insertData(GameObject::Builder& builder);

    protected:

        virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
        virtual BOOL PreTranslateMessage(MSG& msg);
        
    private:

        void updateAttributeValue(const WORD& ctrlID);

        WindowMetrics*          windowMetrics;

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

class EditObjectLocationsTab : public EOTabViewBase {

    public:

        EditObjectLocationsTab(WindowMetrics* inWindowMetrics) : windowMetrics(inWindowMetrics) {}
        virtual int OnCreate(CREATESTRUCT& cs);
        virtual void PreRegisterClass(WNDCLASS& wc);
        virtual void moveControls();
        virtual void calculatePageWidth();
        virtual void populateFields(const GameObject& gameObject); // Populate Fields
        virtual WORD validateFields();
        virtual void insertData(GameObject::Builder& builder) {}

    protected:

        virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
        virtual BOOL PreTranslateMessage(MSG& msg);

    private:

        inline void locatedAtChanged(const WORD& ctrlID, const WORD& ctrlAction);
        inline void toggleUnlocksDoor(const BOOL& doesUnlock);

        WindowMetrics*          windowMetrics;

        CButton                 grpLocations;

        CButton                 btnLocatedAt[3];

        CStatic                 lblGroundCoord[2];
        CEdit                   txtGroundCoord[2];

        CComboBox               cbxWhichCharacter;

        CButton                 btnUnlocksDoor;
        CStatic                 lblDoorCoord[2];
        CEdit                   txtDoorCoord[2];

};

#endif // __EDITOBJECT_TABVIEWS_H__
