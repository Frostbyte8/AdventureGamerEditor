#ifndef __EDITOR_CONSTANTS_H__
#define __EDITOR_CONSTANTS_H__

#include "compat/stdint_compat.h"

namespace EditorConstants {

    const unsigned int TileWidth            = 37;
    const unsigned int TileHeight           = 37;
    const unsigned int TilesPerRow          = 16;
    const unsigned int TilesPerCol          = 16;
    const unsigned int DirtroadStartRow     = 8;
}

namespace AttributeTypes {
    enum types {
        Energy,
        Skill,
        Willpower,
        Luck,
        Torchlife,
        NumTypes
    };
}

namespace AdventureGamerConstants {
    const unsigned int NumAttributes    = AttributeTypes::NumTypes;
}

namespace AdventureGamerHeadings {
    const std::string Row           = "{row ";      // Space is intentional.
    const std::string Jumps         = "{jumps";
    const std::string Switches      = "{swtchs";
    const std::string Attributes    = "{attrb";
    const std::string Objects       = "{objct";
    const std::string Character     = "{cretr";
}

namespace AdventureGamerSubHeadings {
    const unsigned int NumAttributeSubHeadings = AttributeTypes::NumTypes;
    const std::string Attributes[NumAttributeSubHeadings] = { "Stamina", "Skill", "Willpower", "Luck", "Torch Life" };
}

namespace HearingTypes {
    enum types {
        NoChange,
        Deaf,
        Normal,
        Ultrasonic
    };
}

namespace SightTypes {
    enum types {
        NoChange,
        Blind,
        Normal,
        Infrared
    };
}

namespace LanguageConstants {
    const unsigned int FileMenuItem         = 100;
    const unsigned int ExitMenuItem         = 101;
}

#endif // __EDITOR_CONSTANTS_H__
