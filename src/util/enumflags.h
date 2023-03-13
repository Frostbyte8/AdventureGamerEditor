#ifndef __ENUMFLAGS_H__
#define __ENUMFLAGS_H__

#include <assert.h>

template<class EnumType, unsigned int numFlags>
class EnumFlags {

    public:

        EnumFlags() : flags(0), flagCount(numFlags) {
            assert(numFlags != 0);
            assert(numFlags <= sizeof(unsigned int) * 8);
        }

        EnumFlags(EnumType flag) : flags(flag) {
        }

        EnumFlags(const EnumFlags& efCopy) : flags(efCopy.flags), flagCount(numFlags) {
        }

        EnumFlags& operator|=(const EnumFlags& rhs) {
            flags |= rhs.flags;
            return *this;
        }

        EnumFlags& operator&=(const EnumFlags& rhs) {
            flags &= rhs.flags;
            return *this;
        }

        EnumFlags operator|(const EnumType flagsToAdd) const {
            EnumFlags newFlags(*this);
            newFlags |= flagsToAdd;
            return newFlags;
        }

        EnumFlags operator&(const EnumType flagsToMask) const {
            EnumFlags newFlags(*this);
            newFlags &= flagsToMask;
            return newFlags;
        }

        EnumFlags operator~() const {
            EnumFlags inverseFlags(*this);
            inverseFlags.flags = ~inverseFlags.flags; 
            return inverseFlags; 
        }

        // Check if individual flags are set

        bool isSet(const EnumType checkFor) const {
            return (flags & checkFor) ? true : false;
        }

        // Check if a group of flags are set, which makes more
        // sense than overriding bool() for EnumFlags

        bool isSet(const EnumFlags checkFor) const {
            return (flags & checkFor.flags) ? true : false;
        }

        // Check if no flags are set

        bool none() const {
            return (!flags) ? true : false;
        }

        // Reset flags to 0

        void reset() {
            flags = 0;
        }

        // Return the flags as an unsigned int

        const unsigned int& to_uint() const { 
            return flags;
        }

        // In some cases, we may need to turn a uint back into
        // flags

        void from_uint(const unsigned int& inFlags) {
            // Mask out impossible flags
            flags = (inFlags & ((1 << flagCount)-1));
        }
    
    protected:
        unsigned int flags;
        unsigned int flagCount;
};

// Usage:
// typedef EnumFlags<enum EnumThatHasFlags, NumFlagsInEnum> YourTypeName_T;

#endif // __ENUMFLAGS_H__