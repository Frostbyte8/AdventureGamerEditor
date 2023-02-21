#ifndef __GAMEWORLD_CONTROLLER_H_
#define __GAMEWORLD_CONTROLLER_H_

#include <deque>
#include "../util/languagemapper.h"

// Perhaps this should be renamed to MainWindow_Controller?

class GameWorldController {

    public:
        //bool LoadWorld(const std::string& filePath, const std::string& fileName);
        bool LoadLanguageFile(const std::string& filePath, const std::string& fileName);

        // TODO: return a copy of this
        LanguageMapper& getLanguageMapper() { return languageMapper; }

    private:

        //MainWindowInterface*            mainWindow;

        // Game World is composed of several other objects which I have
        // split up here to make doing undo/redo operations easier.

        /*
        GameMap*                        gameMap;
        GameInfo*                       gameInfo;
        std::string                     gameStory;
        std::deque<GameCharacters*>     gameCharacters;
        std::deque<GameObjects*>        gameObjects;
        */
        LanguageMapper		languageMapper;

};

#endif // __GAMEWORLD_CONTROLLER_H_
