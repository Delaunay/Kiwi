#pragma once

#include <cassert>
#include <cstdio>
#include <memory>
#include <ostream>

#include "Expression.h"

#include "Definition.h"
#include "Type.h"

#include "../Logging/Log.h"
//

namespace kiwi {

// -------------------------------------------------------------------------------------------
enum class ValueTag { vprimitive, vstruct, vunion, vfunction };

class Value : public Expression {
  public:
    Value(ValueTag tag) : Expression(NodeTag::value), value_tag(tag) {}

    virtual ~Value() {}

    virtual std::ostream &dump(std::ostream &out) const = 0;

    virtual Type *type() const = 0;

    template <typename T> T as(std::size_t index = 0) const;

    ValueTag value_tag;
};

// -------------------------------------------------------------------------------------------
//  Primitive Value
// -------------------------------------------------------------------------------------------

// TBD: half precision (IEEE 754-2008)
/*
struct f16 {
    f16() = default;
    template <typename T> f16(T) {}
    template <typename T> f16 operator=(T) { return f16(); }
    template <typename T> operator T() { return T(); }
};
inline std::ostream &operator<<(std::ostream &out, f16) { return out; }
*/

//     X(f16)

#define KIWI_PRIMITIVE(X)                                                                          \
    X(i8)                                                                                          \
    X(i16)                                                                                         \
    X(i32)                                                                                         \
    X(i64)                                                                                         \
    X(u8)                                                                                          \
    X(u16)                                                                                         \
    X(u32)                                                                                         \
    X(u64)                                                                                         \
    X(f32)                                                                                         \
    X(f64)

// clang-format off
enum class PrimitiveTag {
    #define X(n) n,
        KIWI_PRIMITIVE(X)
    #undef X
    none,
};

template <typename T> PrimitiveTag get_primitive_tag() { return PrimitiveTag::none; }

// Map C++ type to Primitive Tag
#define X(n)                                                                                       \
    template <> inline PrimitiveTag get_primitive_tag<n>() { return PrimitiveTag::n; }
    KIWI_PRIMITIVE(X)
#undef X

// PrettyPrint a Primitive Tag name
inline const std::string &get_primitive_name(PrimitiveTag id) {
    static std::unordered_map<PrimitiveTag, std::string> builtin_types = {
    #define X(n) {PrimitiveTag::n, #n},
        KIWI_PRIMITIVE(X)
    #undef X
        {PrimitiveTag::none, "NotAPrimitive"}
    };
    return builtin_types[id];
}

// Build a Kiwi Builtin Type for primitives
template <typename T> BuiltinType *get_primitive_type() {
    static BuiltinType t(get_primitive_name(get_primitive_tag<T>()));
    return &t;
}
//clang-format on

class PrimitiveValue : public Value {
  public:
    template <typename T>
    PrimitiveValue(T v) : Value(ValueTag::vprimitive),
        primitive_tag(get_primitive_tag<T>()), _type(get_primitive_type<T>())
    {
        set_value(v);
    }

    Type* type() const override { return _type;}

    template <typename T> void set_value(T v) {
        // clang-format off
        switch(primitive_tag) {
        #define X(n)                                                                                   \
        case PrimitiveTag::n: {                                                                        \
            primitive_tag = get_primitive_tag<T>();                                                    \
            _type = get_primitive_type<T>();                                                           \
            value.n##_value = n(v);                                                                    \
            break;                                                                                     \
        }
        KIWI_PRIMITIVE(X)
        #undef X
        case PrimitiveTag::none:
            return;
        //clang-format on
        }
    }

    template<typename T> T as() const{
        if (get_primitive_tag<T>() == primitive_tag){
            // clang-format off
            switch(primitive_tag) {
            #define X(n)                                                                                \
            case PrimitiveTag::n: {                                                                     \
                return T(value.n##_value);                                                              \
            }
            KIWI_PRIMITIVE(X)
            #undef X
            case PrimitiveTag::none:
                return T();
            }
            //clang-format on
        }
        return T();
    }

    std::ostream &dump(std::ostream &out) const override {
        // clang-format off
        switch(primitive_tag) {
        #define X(n)                                                                                \
        case PrimitiveTag::n: {                                                                     \
            out << value.n##_value;                                                                 \
            break;                                                                                  \
        }
        KIWI_PRIMITIVE(X)
        #undef X
        case PrimitiveTag::none:
            break;
        }
        //clang-format on
        out << ": " << get_primitive_name(primitive_tag);
        return out;
    }

  private:
    union PrimitiveValues {
    // clang-format off
    #define X(n) n n##_value;
        KIWI_PRIMITIVE(X)
    #undef X
    //clang-format on
    } value;

    PrimitiveTag primitive_tag;
    Type* _type;
};

class UnionValue : public Value {
  public:
    UnionValue() : Value(ValueTag::vunion){}

    UnionValue* set_type(Union* ptr){
        _type = ptr;
        return this;
    }

    //Type* type() const override { return _type;}
    Type* type() const override { return nullptr;}

    Union* definition() { return _type; }

    std::ostream &dump(std::ostream &out) const override {
        out << "(";
        if (value && index >= 0){
            out << std::get<0>(_type->attributes[std::size_t(index)]) << " = ";
            value->dump(out);
        } else {
            out << "none";
        }
        return out << ")";
    }

    Value *value;
    int32 index;

private:
    Union* _type;
};

class StructValue : public Value {
  public:
    StructValue() :
        Value(ValueTag::vstruct) {}

    StructValue* set_type(Struct* ptr){
        _type = ptr;
        return this;
    }

    //Type* type() const override { return _type;}
    Type* type() const override { return nullptr;}

    Struct* definition() { return _type; }

    std::ostream &dump(std::ostream &out) const override {
        out << "(";
        for(u64 i = 0; i < values.size() - 1; ++i) {
            auto &item = values[i];
            item->dump(out) << ", ";
        }
        values[values.size() - 1]->dump(out);
        return out << ")";
    }

    Array<Value *> values;
private:
    Struct* _type;
};

struct ExecutionContext{};

class FunctionValue : public Value {
public:
    FunctionValue(Function* def, ExecutionContext& ctx):
        Value(ValueTag::vfunction), def(def), ctx(ctx)
    {}

    Function* def;
    ExecutionContext& ctx;
};

template<typename T> T Value::as(std::size_t index) const{
    switch(value_tag){
    case ValueTag::vprimitive:{
        PrimitiveValue const* val = static_cast<PrimitiveValue const*>(this);
        return val->as<T>();
    }
    case ValueTag::vstruct:{
        UnionValue const* val = static_cast<UnionValue const*>(this);
        return val->as<T>();
    }
    case ValueTag::vunion:{
        StructValue const* val = static_cast<StructValue const*>(this);
        return val->values[index]->as<T>();
    }
    case ValueTag::vfunction:{
        FunctionValue const* val = static_cast<FunctionValue const*>(this);
        return val->as<T>();
    }
    }
}

} // namespace kiwi
