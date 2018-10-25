#pragma once

namespace kiwi{

class AssetManager{
private:
    AssetManager() = default;
public:
    static AssetManager& instance(){
        static AssetManager assm;
        return assm;
    }
};

}
