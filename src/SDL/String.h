#pragma once

namespace kiwi{
class String{
public:
    String(const char* data):
        _allocated(false), _data(data)
    {}

    ~String(){
        if (_allocated)
            delete[] _data;
    }

    operator const char*() const{
        return _data;
    }

private:
    bool _allocated = false;
    const char* _data;
};
}
