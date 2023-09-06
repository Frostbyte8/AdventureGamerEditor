#ifndef __CONNECTIONPOINT_H__
#define __CONNECTIONPOINT_H__

#include "simple_point.h"

namespace ConnnectionPointConstants {
    const int POINT_NOWHERE = -1;
}

class ConnectionPoint {

    public:
        ConnectionPoint() : connectPoint1(-1, -1), connectPoint2(-1, -1) {
        }

        ConnectionPoint(SimplePoint point1, SimplePoint point2) : connectPoint1(point1), connectPoint2(point2) {
        }

        const SimplePoint& getConnectPoint1() const { return connectPoint1; }
        const SimplePoint& getConnectPoint2() const { return connectPoint2; }

        bool operator==(const ConnectionPoint& rhs) const {

            if((connectPoint1 == rhs.getConnectPoint1() && connectPoint2 == rhs.getConnectPoint2()) || 
               (connectPoint2 == rhs.getConnectPoint1() && connectPoint1 == rhs.getConnectPoint2())) {
                return true;
            }
            return false;
        }

    private:
        SimplePoint connectPoint1;
        SimplePoint connectPoint2;
};

#endif // __CONNECTIONPOINT_H__