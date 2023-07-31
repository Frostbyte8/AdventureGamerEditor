#ifndef __ADVENTUREGAMER_CONSTANTS_H__
#define  __ADVENTUREGAMER_CONSTANTS_H__

///----------------------------------------------------------------------------
/// AdventureGamerConstants - Hard limits of the game itself
///----------------------------------------------------------------------------

namespace AdventureGamerConstants {
	const std::string FileNameExtension     = "SG0";
    const unsigned int MaxFileNameLength    = 12;

    const int MinAttributeValue             = 0;
    const int MaxAttributeValue             = 12;
    const int MinAmountOfMoney              = 0;
    const int MaxAmountOfMoney              = 10000;
    
    const int MaxNumObjects                 = 100;
    const int MaxNumCharacters              = 25;
}

#endif //  __ADVENTUREGAMER_CONSTANTS_H__