#ifndef __EDITOR_CONSTANTS_H__
#define __EDITOR_CONSTANTS_H__

#include <string>
#include "compat/stdint_compat.h"

///----------------------------------------------------------------------------
/// EditorConstants - Useful constants that are used by the editor
///----------------------------------------------------------------------------

namespace EditorConstants {
    unsigned int const TilesPerRow          = 16;
    unsigned int const TilesPerCol          = 16;
    unsigned int const DirtroadStartRow     = 8;

	const int DefaultCols			        = 10;
    const int DefaultRows			        = 10;

    const int IGNORE_ROW                    = -1;
    const int IGNORE_COL                    = -1;
}           

///----------------------------------------------------------------------------
/// AlterTypes - Add / Edit / Place / Delete an Object
///----------------------------------------------------------------------------

namespace AlterType {
    const unsigned int Add      = 0;
    const unsigned int Edit     = 1;
    const unsigned int Place    = 2;
    const unsigned int Delete   = 3;
}

#endif // __EDITOR_CONSTANTS_H__
