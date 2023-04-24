#ifndef __EDITOBJECT_TABVIEWS_H__
#define __EDITOBJECT_TABVIEWS_H__

#include <wxx_wincore.h>
#include <wxx_stdcontrols.h>
#include "../util/win32pp_extras.h"
#include "../win32/window_metrics.h"

class EOTabViewBase : public CWnd {

    public:
        //virtual void MoveControls(CSize);     // Move/Resize controls
        //virtual CSize getContentMaxSizes();   // Calculate the Max Width and Height of the page
        //virtual int getLongestControl();

        //virtual void CalculatePageHeight() = 0;       
        //virtual bool ValidateFields();        // Validate Fields
        //virtual void PopulateFields();        // Populate Fields
        
        //virtual void MoveControls(const int& maxWidth);
        virtual void calculatePageWidth() = 0;
        LONG pageWidth;
};

class EditObjectDescriptionsTab : public EOTabViewBase {

    public:
        EditObjectDescriptionsTab(WindowMetrics* inWindowMetrics) : windowMetrics(inWindowMetrics) { }
        virtual int OnCreate(CREATESTRUCT& cs);

    private:

        virtual void calculatePageWidth();

        CButton                 grpDescriptions;
        CStatic                 lblDescriptions[6];
        CAnsiEdit               txtDescriptions[6];
        CButton                 btnBrowse[2];
        CStatic                 picIcon;
        
        WindowMetrics*          windowMetrics;
        CSize                   contentSize;
};

#endif // __EDITOBJECT_TABVIEWS_H__