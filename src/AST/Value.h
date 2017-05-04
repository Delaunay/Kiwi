#pragma once

#include <cstdio>
#include <memory>
#include <cassert>

#include "Expression.h"

//#define KIWI_DEBUG
#include "../src/Debug.h"

namespace kiwi{

struct typetype{};
struct object{};

struct f8{};
struct f16{};
typedef float f32;
typedef double f64;

typedef std::int8_t  i8;
typedef std::int16_t i16;
typedef std::int32_t i32;
typedef std::int64_t i64;

typedef std::uint8_t u8;
typedef std::uint16_t u16;
typedef std::uint32_t u32;
typedef std::uint64_t u64;

#define KIWI_TYPE_TYPE \
    X(i8)  \
    X(i16) \
    X(i32) \
    X(i64) \
    X(u8)  \
    X(u16) \
    X(u32) \
    X(u64) \
    X(f32) \
    X(f64)

enum class BuiltinType : std::size_t{
#define X(n) n,
    KIWI_TYPE_TYPE
#undef X
    object
};


inline
const std::string& name(BuiltinType id){
    static std::unordered_map<BuiltinType, std::string> builtin_types = {
    #define X(n) {BuiltinType::n, #n},
        KIWI_TYPE_TYPE
    #undef X
    };
    return builtin_types[id];
}


template<typename T>
BuiltinType type_id(){ return BuiltinType::object; }

#define X(n) template<> inline BuiltinType type_id<n>(){ return BuiltinType::n; }
    KIWI_TYPE_TYPE
#undef X

template<typename T>
Expression* type(){
    static Type t(name(type_id<T>()));
    return &t;
}


//
class Value: public Expression{
public:
    template<typename T>
    Value(T x):
        Expression(NodeTag::value),
        _self(std::make_shared<Model<T>>(std::move(x))),
        type(kiwi::type_id<T>())
    {}

    VTABLEV(void visit(class DynamicVisitor* v) override;)

    template<typename T>
    T as(){
        debug_if(type != type_id<T>() , this->dump(std::cout));

        assert(type == type_id<T>() && "wrong Value cast");
        return reinterpret_cast<const Model<T>*>(_self.get())->_data;
    }

    std::ostream& dump(std::ostream& out){
        switch(type){
        #define X(n)\
            case BuiltinType::n:{\
                out << "(" << as<n>() << ": " << name(BuiltinType::n) << ")";\
                out.flush();\
                break;}
            KIWI_TYPE_TYPE
        #undef X
        }
        return out;
    }


    const BuiltinType type;

private:

    struct Concept{
        virtual ~Concept() = default;
    };

    template<typename T>
    struct Model final : Concept{
        Model(T x):
            _data(std::move(x))
        {}

        T                 _data;
    };

    std::shared_ptr<const Concept> _self;
};

}
