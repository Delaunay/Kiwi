#pragma once

#include "../Parsing/Optional.h"
#include "../Types.h"
#include "Point.h"

namespace kiwi {

template <typename T> class Collision {
  public:
    // return the object T collinding with the given point
    Option<T> collide(uint32 x, uint32 y) {
        for(auto &item : _objects) {
            Point4i &rectangle = std::get<0>(item);
            if(x > rectangle.x && x < rectangle.x + rectangle.width && y > rectangle.y &&
               y < rectangle.y + rectangle.height) {
                return some(std::get<1>(item));
            } else {
                return none();
            }
        }
    }

    void remove_object(T *object) {
        _objects.erase(std::remove(_objects.begin(), _objects.end(), object), _objects.end());
    }

    void insert_object(Point4i bounding_box, T &object) {
        _objects.push_back(std::make_tuple(bounding_box, object));
    }

  private:
    Array<Tuple<Point4i, T *>> _objects;
};
} // namespace kiwi
