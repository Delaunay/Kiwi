﻿#pragma once

#include <cstdio>
#include <memory>
#include <cassert>

#include "Expression.h"

#include "../Logging/Log.h"

namespace kiwi{

struct typetype{};
struct object{};

struct f16{};
typedef float f32;
typedef double f64;

typedef std::int8_t  i8;
typedef std::int16_t i16;
typedef std::int32_t i32;
typedef std::int64_t i64;

typedef std::uint8_t  u8;
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

enum class BuiltinType{
#define X(n) n,
    KIWI_TYPE_TYPE
#undef X
    object,
    type
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
template<> inline BuiltinType type_id<typetype>(){ return BuiltinType::type; }

#define X(n) template<> inline BuiltinType type_id<n>(){ return BuiltinType::n; }
    KIWI_TYPE_TYPE
#undef X

template<typename T, typename NodeTrait>
Expression<NodeTrait>* type(){
    static generic::Type<Expression<NodeTrait>> t(name(type_id<T>()));
    return &t;
}

template<typename NodeTrait>
class Value: public Expression<NodeTrait>{
public:
    NODE_TYPES;

    template<typename T>
    Value(T x):
        Expression<NodeTrait>(NodeTag::value),
        _self(std::make_shared<Model<T>>(std::move(x))),
        type(kiwi::type_id<T>())
    {}

    VTABLEV(void visit(class DynamicVisitor* v) override;);

    template<typename T>
    const T& as() const{
        debug_if(type != type_id<T>(), this);
        assert(type == type_id<T>() && "wrong Value cast");
        return reinterpret_cast<const Model<T>*>(_self.get())->_data;
    }

    template<typename T>
    const T& as(const T& dummy) const{
        debug_if(type != type_id<T>(), this);
        assert(type == type_id<T>() && "wrong Value cast");
        return reinterpret_cast<const Model<T>*>(_self.get())->_data;
    }

    std::ostream& dump(std::ostream& out) const{
        return print(out);
    }

    std::ostream& print(std::ostream& out, bool ptype = false) const{
        switch(type){
        #define X(n)\
            case BuiltinType::n:{\
                if (ptype){ out << "(";}\
                out << as<n>();\
                if (ptype){ out << ": " << name(BuiltinType::n) << ")";}\
                break;\
            }
            KIWI_TYPE_TYPE
        #undef X
        // this is a dummy type
        case BuiltinType::type:{
            out << "Type";
            break;
        }
        // this a user defined object i.e a Kiwi object
        case BuiltinType::object:{
            out << "Object";
            break;
        }
        }
        out.flush();
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

        T _data;
    };

    std::shared_ptr<const Concept> _self;
};

template<typename NodeTrait>
std::ostream& operator<< (std::ostream& out, const Value<NodeTrait>* v){
    return v->print(out);
}

template<typename T, typename NodeTrait>
const T& as(Value<NodeTrait>* expr){
    return expr->as(T());
}

}
