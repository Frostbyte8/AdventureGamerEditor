#ifndef __EDITOR_MAINWINDOW_DOCKERS_H__
#define __EDITOR_MAINWINDOW_DOCKERS_H__

#include "editor_mainwindow_views.h"
#include "../controller/gameworld_controller.h"

///----------------------------------------------------------------------------
/// RoadSelectorDocker - Docker that cotains the RoadSelectorView
///----------------------------------------------------------------------------

class RoadSelectorDocker : public CDocker {

	public:
		RoadSelectorDocker() {
			SetView(view);
			SetBarWidth(0);
		}
		virtual ~RoadSelectorDocker() {}

	private:

		RoadSelectorView view;

		// Disable copy construction and assignment operator
		RoadSelectorDocker(const RoadSelectorDocker&);
		RoadSelectorDocker& operator=(const RoadSelectorDocker&);
};

///----------------------------------------------------------------------------
/// GameMapDocker - Docker that contains the GameMapView
///----------------------------------------------------------------------------

class GameMapDocker : public CDocker {

	public:
        GameMapDocker(GameWorldController* gwc, const CBitmap* inTileSet) : view(gwc, inTileSet) {
			SetView(view);
			SetBarWidth(6);
		}
		virtual ~GameMapDocker() {}

	private:

		GameMapView view;

		// Disable copy construction and assignment operator
		GameMapDocker(const GameMapDocker&);
		GameMapDocker& operator=(const GameMapDocker&);

};

///----------------------------------------------------------------------------
/// EntitiesHereDocker - Docker that contains the EntitiesHereView
///----------------------------------------------------------------------------

class EntitiesHereDocker : public CDocker {

	public:
		EntitiesHereDocker() {
			SetView(view);
			SetBarWidth(6);
		}
		virtual ~EntitiesHereDocker() {}

	private:

		EntitiesHereView view;

		// Disable copy construction and assignment operator
		EntitiesHereDocker(const EntitiesHereDocker&);
		EntitiesHereDocker& operator=(const EntitiesHereDocker&);

};

#endif // __EDITOR_MAINWINDOW_DOCKERS_H__
