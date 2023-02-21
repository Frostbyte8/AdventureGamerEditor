#ifndef __EDITOR_MAINWINDOW_H__
#define __EDITOR_MAINWINDOW_H__

#include <wxx_wincore.h>
#include <wxx_dockframe.h>
#include <wxx_stdcontrols.h>
#include "editor_mainwindow_dockers.h"
#include "../interface/mainwindow_interface.h"
#include "../win32/window_metrics.h"
#include "../util/languagemapper.h"

class MainWindowFrame : public CDockFrame, public MainWindowInterface {

	public:
		MainWindowFrame();
		virtual ~MainWindowFrame();
		virtual HWND Create(HWND parent = 0);

        // Public Interface Functions
        virtual int AskYesNoQuestion(const std::string& question, const std::string& title,
                                     bool allowCancel = true);

	protected:
		virtual int     OnCreate(CREATESTRUCT& cs);
		virtual void    OnInitialUpdate();
		virtual LRESULT WndProc(UINT msg, WPARAM wparam, LPARAM lparam);

	private:

        void CreateMenuBar();

        CMenu mainMenu;
        CMenu fileMenu;

		GameEntitiesView*	entityView;
		GameMapDocker*		gameMapDocker;
		RoadSelectorDocker* roadSelectorDocker;
		EntitiesHereDocker* entitiesHereDocker;

		LanguageMapper		languageMapper;
		WindowMetrics		windowMetrics;

		// Disable copy construction and assignment operator
		MainWindowFrame(const MainWindowFrame&);
		MainWindowFrame& operator = (const MainWindowFrame&);

};


#endif //  __EDITOR_MAINWINDOW_H__
