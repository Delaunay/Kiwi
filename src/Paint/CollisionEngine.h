#pragma once

namespace kiwi{

class CollisionEngine{
private:
    CollisionEngine() = default;
public:
    static CollisionEngine& instance(){
        static CollisionEngine assm;
        return assm;
    }







    // Extract Bounding box from a Render Tree




};

}
