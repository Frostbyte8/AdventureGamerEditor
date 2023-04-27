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

class EOTabViewBase : public CWnd {

    public:
        //virtual void MoveControls(CSize);     // Move/Resize controls
        //virtual CSize getContentMaxSizes();   // Calculate the Max Width and Height of the page
        //virtual int getLongestControl();

        //UpdateWindowMetrics(); // Window Metrics has changed
        //virtual int ValidateFields();        // Validate Fields
        virtual void populateFields(const GameObject& gameObject) = 0; // Populate Fields
        virtual void moveControls() = 0;
        virtual void calculatePageWidth() = 0;
        virtual void calculatePageHeight() = 0;

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
        virtual void calculatePageHeight();
        virtual void populateFields(const GameObject& gameObject);

    protected:
        
        
        virtual BOOL PreTranslateMessage(MSG &msg);

    private:

        CButton                 grpDescriptions;
        CStatic                 lblDescriptions[6];
        CAnsiEdit               txtDescriptions[6];
        CButton                 btnBrowse[2];
        CStatic                 picIcon;
        
        WindowMetrics*          windowMetrics;
        //CSize                   contentSize;
};

class EditObjectQualitiesTab : public EOTabViewBase {

    public:
        EditObjectQualitiesTab(WindowMetrics* inWindowMetrics) : windowMetrics(inWindowMetrics) { }
        virtual int OnCreate(CREATESTRUCT& cs);
        virtual void PreRegisterClass(WNDCLASS& wc);
        virtual void moveControls();
        virtual void calculatePageWidth();
        virtual void calculatePageHeight();
        virtual void populateFields(const GameObject& gameObject); // Populate Fields
    
    protected:
                
        virtual BOOL PreTranslateMessage(MSG &msg);

    private:
        WindowMetrics*          windowMetrics;
        CButton                 grpFlags;
        CButton                 grpProperties;
        CButton                 btnFlags[8];
        CStatic                 lblProperties[3];
        CEdit                   txtProperties[2];
        CSpinButton             spnProperties[2];
        CComboBox               cbxUsedWith;
};

#endif // __EDITOBJECT_TABVIEWS_H__