#ifndef __EDITOR_CONSTANTS_H__
#define __EDITOR_CONSTANTS_H__

#include <string>
#include "compat/stdint_compat.h"

///----------------------------------------------------------------------------
/// EditorConstants - Useful constats that are used by the editor
///----------------------------------------------------------------------------

namespace EditorConstants {
    unsigned int const TilesPerRow          = 16;
    unsigned int const TilesPerCol          = 16;
    unsigned int const DirtroadStartRow     = 8;

	const unsigned int DefaultCols			= 10;
	const unsigned int DefaultRows			= 10;
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
