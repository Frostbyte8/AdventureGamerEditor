#ifndef __SIMPLE_POINT_H__
#define __SIMPLE_POINT_H__

class SimplePoint {

    public:

        SimplePoint() {
            x = 0;
            y = 0;
        }
        
        SimplePoint(const int& x, const int y) {
            this->x = x;
            this->y = y;
        }

        const int& getX() const {
            return x;
        }

        const int& getColumn() const {
            return x;
        }

        const int& getY() const {
            return y;
        }

        const int& getRow() const {
            return y;
        }

        bool operator==(const SimplePoint& rhs) const {
            return (this->x == rhs.x && this->y == rhs.y);
        }

    private:
        int x;
        int y;
};

#endif // __SIMPLE_POINT_H__