#ifndef __EDITOBJECT_INTERFACE_H__
#define  __EDITOBJECT_INTERFACE_H__

class EditObjectInterface {
    public:
        virtual void fixedLocationToggled(const bool& enabled) = 0;
};

#endif // __EDITOBJECT_INTERFACE_H__