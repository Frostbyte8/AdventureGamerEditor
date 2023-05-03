#include "editobject_tabviews.h"
#include "../model/gameobject.h"
#include "../util/languagemapper.h"
#include <algorithm>

namespace ControlIDs {
    const WORD MasterKey    = 101;
    const WORD Invisible    = 102;
    const WORD Climber      = 103;
    const WORD Protection   = 104;
    const WORD Torch        = 105;
    const WORD Worn         = 106;
    const WORD Fixed        = 107;
    const WORD Money        = 108;
}

///----------------------------------------------------------------------------
/// OnCreate - Creates the controls for the Tab page.
///----------------------------------------------------------------------------

int EditObjectQualitiesTab::OnCreate(CREATESTRUCT& cs) {
    
    const int retVal = CWnd::OnCreate(cs);
    
    CString caption;
    LanguageMapper& langMap = LanguageMapper::getInstance();

    grpFlags.Create(*this, 0, BS_GROUPBOX);
    EOD_SetWindowText(LanguageConstants::FlagsGroupLabel, grpFlags, caption, langMap);

    for(int k = 0; k < GameObjectFlags1::NumFlags; ++k) {
        btnFlags[k].Create(*this, 0, BS_AUTOCHECKBOX);
        btnFlags[k].SetDlgCtrlID(ControlIDs::MasterKey + k);
        EOD_SetWindowText(LanguageConstants::MasterKeyFlag+k, btnFlags[k], caption, langMap);
    }

    for(int i = 0; i < 2; ++i) {
        lblProperties[i].Create(*this, 0, SS_SIMPLE);
        txtProperties[i].Create(*this, 0, ES_AUTOHSCROLL);
        txtProperties[i].SetExStyle(WS_EX_CLIENTEDGE);
        EOD_SetWindowText(LanguageConstants::MonetaryLabel+i, lblProperties[i], caption, langMap);

        // TODO: Why does WS_VISBLE have to be set? Possible bug?
        spnProperties[i].Create(*this, 0, WS_VISIBLE | UDS_AUTOBUDDY |
                                UDS_SETBUDDYINT | UDS_ARROWKEYS | UDS_ALIGNRIGHT);
       
    }

    grpProperties.Create(*this, 0, BS_GROUPBOX);
    grpProperties.SetWindowTextW(L"Properties");

    lblProperties[2].Create(*this, 0, SS_SIMPLE);
    EOD_SetWindowText(LanguageConstants::ObjectHeldLabel, lblProperties[2], caption, langMap);

    cbxUsedWith.Create(*this, 0, CBS_DROPDOWNLIST | CBS_DISABLENOSCROLL | WS_VSCROLL);
    EOD_AddString(LanguageConstants::NoObjectSelected, cbxUsedWith, caption, langMap);
    cbxUsedWith.SetCurSel(0);

    spnProperties[0].SetRange(GameObjectConstants::MinMonetaryValue,
                              GameObjectConstants::MaxMonetaryValue);

    spnProperties[1].SetRange(GameObjectConstants::MinNumUses,
                              GameObjectConstants::MaxNumUses);
    
    return retVal;

}

///----------------------------------------------------------------------------
/// calculatePageWidth - Finds how wide the tab page needs to be display the
/// controls
///----------------------------------------------------------------------------

void EditObjectQualitiesTab::calculatePageWidth() {
    
    pageWidth = 0;

    const WindowMetrics::ControlSpacing CS      = windowMetrics->GetControlSpacing();
    const WindowMetrics::ControlDimensions CD   = windowMetrics->GetControlDimensions();

    const LONG checkboxWidth = CD.XCHECKBOX + CD.XCHECKBOX_GAP;
    
    for(int i = 0; i < GameObjectFlags1::NumFlags; ++i) {
        pageWidth = std::max(windowMetrics->CalculateStringWidth(btnFlags[i].GetWindowTextW().c_str()) + checkboxWidth, pageWidth);
    }

    std::vector<CWnd *> controlList;
    controlList.reserve(2);
        
    for(int k = 0; k < 2; ++k) {
        controlList.push_back(&lblProperties[k]);
    }
    
    const size_t clSize = controlList.size();
    
    for(size_t j = 0; j < clSize; ++j) {
        pageWidth = std::max(windowMetrics->CalculateStringWidth(
                             controlList[j]->GetWindowTextW().c_str()), pageWidth);
    }

}

///----------------------------------------------------------------------------
/// moveControls - Move the controls to their desired positions
///----------------------------------------------------------------------------

void EditObjectQualitiesTab::moveControls() {
    
    const WindowMetrics::ControlSpacing cs      = windowMetrics->GetControlSpacing();
    const WindowMetrics::ControlDimensions cd   = windowMetrics->GetControlDimensions();
    
    // The max width of a row is the size of the tab page, less the margins of the
    // Group Box and the window.

    const int maxGroupBoxWidth  = GetClientRect().right - (cs.XWINDOW_MARGIN * 2);
    const int maxRowWidth       = maxGroupBoxWidth - (cs.XGROUPBOX_MARGIN * 2);

    const CSize defaultCheckboxSize(maxRowWidth, cd.YCHECKBOX);
    const CSize defaultLabelSize(maxRowWidth, cd.YLABEL);
    const CSize defaultTextSize(maxRowWidth, cd.YTEXTBOX_ONE_LINE_ALONE);

    CPoint cPos(cs.XGROUPBOX_MARGIN + cs.XWINDOW_MARGIN,
        cs.YFIRST_GROUPBOX_MARGIN + cs.YRELATED_MARGIN + cs.YWINDOW_MARGIN);

    // First deal with all the Flags

    for(int i = 0; i < GameObjectFlags1::NumFlags; ++i) {
        btnFlags[i].MoveWindow(cPos.x, cPos.y, 
                               defaultCheckboxSize.cx, defaultCheckboxSize.cy);

        if(i == GameObjectFlags1::NumFlags - 1) {
            cPos.Offset(0, defaultCheckboxSize.cy);
        }
        else {
            cPos.Offset(0, defaultCheckboxSize.cy + cs.YRELATED_MARGIN);
        }
    }

    grpFlags.MoveWindow(cs.XWINDOW_MARGIN, cs.YWINDOW_MARGIN,
                        maxGroupBoxWidth, cPos.y);

    // Now deal with things like money and uses

    cPos.Offset(0, cs.YUNRELATED_MARGIN + cs.YFIRST_GROUPBOX_MARGIN);
    
    for(int i = 0; i < 2; i++) {
        lblProperties[i].MoveWindow(cPos.x, cPos.y, 
                                    defaultLabelSize.cx, defaultLabelSize.cy);

        cPos.Offset(0, defaultLabelSize.cy + cs.YLABELASSOC_MARGIN);

        txtProperties[i].MoveWindow(cPos.x, cPos.y,
                                    defaultTextSize.cx, defaultTextSize.cy);

        spnProperties[i].SetBuddy(txtProperties[i]);
        cPos.Offset(0, defaultTextSize.cy + cs.YRELATED_MARGIN);
    }

    lblProperties[2].MoveWindow(cPos.x, cPos.y,
                                defaultLabelSize.cx, defaultLabelSize.cy);

    cPos.Offset(0, defaultLabelSize.cy + cs.YLABELASSOC_MARGIN);

    cbxUsedWith.MoveWindow(cPos.x, cPos.y, defaultTextSize.cx,
                           cd.YDROPDOWN + (cd.YTEXTBOX_ONE_LINE_ALONE * 3));

    cPos.Offset(0, cd.YDROPDOWN);

    grpProperties.MoveWindow(cs.XWINDOW_MARGIN, grpFlags.GetClientRect().Height() + cs.YUNRELATED_MARGIN,
                             maxGroupBoxWidth, cPos.y - grpFlags.GetClientRect().Height());


    pageHeight = cPos.y + cs.YUNRELATED_MARGIN;
}

///----------------------------------------------------------------------------
/// populateFields - Take the data from the GameObject and fill in the fields
/// on the tab page.
/// @param a constant reference to GameObject object
///----------------------------------------------------------------------------

void EditObjectQualitiesTab::populateFields(const GameObject &gameObject) {

    const uint8_t flags = gameObject.getFlags1();
    
    for(int i = 0; i < GameObjectFlags1::NumFlags; ++i) {
        if(flags & (1<<i)) {
            btnFlags[i].SetCheck(BST_CHECKED);
            if((flags & (1<<i)) == GameObjectFlags1::Money ||
               (flags & (1<<i)) == GameObjectFlags1::FixedLocation) {
                flagsChanged(ControlIDs::MasterKey+i, BN_CLICKED);
            }
        }
    }

    CString windowText = AtoW(std::to_string(gameObject.getMonetaryWorth()).c_str());
    txtProperties[0].SetWindowText(windowText);

    windowText = AtoW(std::to_string(gameObject.getUses()).c_str());
    txtProperties[1].SetWindowText(windowText);
}

///----------------------------------------------------------------------------
/// PreRegisterClass - Override defaults for tab page
///----------------------------------------------------------------------------

void EditObjectQualitiesTab::PreRegisterClass(WNDCLASS& wc) {
    wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"EditObjectQualitiesTabPage";
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
}

///----------------------------------------------------------------------------
/// PreTranslateMessage - Intercept messages, and check if the tab needs
/// to process them.
///----------------------------------------------------------------------------

BOOL EditObjectQualitiesTab::PreTranslateMessage(MSG &msg) {

    if(IsDialogMessage(msg)) {
        return TRUE;
    }
    
    return CWnd::PreTranslateMessage(msg);

}

//=============================================================================
// Events
//=============================================================================

///----------------------------------------------------------------------------
/// OnCommand - Process the WM_COMMAND message. Refer to to the Win32++
/// documentation for more details.
///----------------------------------------------------------------------------

BOOL EditObjectQualitiesTab::OnCommand(WPARAM wParam, LPARAM lParam) {

    if(!lParam) {
        return FALSE;
    }

    const WORD ctrlID = LOWORD(wParam);
    const WORD ctrlAction = HIWORD(wParam);

    if(ctrlID >= ControlIDs::MasterKey && ctrlID <= ControlIDs::Money) {
        flagsChanged(ctrlID, ctrlAction); 
    }


    return TRUE;
}

///----------------------------------------------------------------------------
/// flagsChanged - Updates the check boxes when one of the flags is selected,
/// as well as toggling the two textboxes if necessary.
/// @param an integer containing the control ID of the control being processed
/// @param an integer indicating the notification code the control sent
///----------------------------------------------------------------------------

void EditObjectQualitiesTab::flagsChanged(const WORD& ctrlID, const WORD& ctrlAction) {

    const int which = ctrlID - ControlIDs::MasterKey;
    const bool isChecked = btnFlags[which].GetCheck() == BST_CHECKED ? true : false; 

    // If an object is at a fixed location, it can't have any other flag
    // set. You also can't sell it.
    // TODO: It also cannot be in a character's inventory.

    // If an object is Money, the only other thing it can be is invisible, it
    // also cannot be used, but does have monetary value, and it cannot be used
    // with another object.

    if(ctrlID == ControlIDs::Fixed) {

        for(int i = 0; i < 8; ++i) {
            if(i != which) {
                btnFlags[i].EnableWindow(!isChecked);
            }
        }

        txtProperties[0].EnableWindow(!isChecked);
        
    }
    else if(ctrlID == ControlIDs::Money) {

        for(int i = 0; i < 8; ++i) {
            if(i != which && i != ControlIDs::Invisible - ControlIDs::MasterKey) {
                btnFlags[i].EnableWindow(!isChecked);
            }
        }

        txtProperties[1].EnableWindow(!isChecked);
        cbxUsedWith.EnableWindow(!isChecked);

    }

}
