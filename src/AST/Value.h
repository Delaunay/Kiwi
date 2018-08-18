#pragma once

#include <cassert>
#include <cstdio>
#include <memory>

#include "Expression.h"
#include "Type.h"

#include "../Logging/Log.h"

namespace kiwi {

struct typetype {};
struct object {};

#define KIWI_TYPE_TYPE(X)                                                                          \
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

enum class BuiltinTypeTag {
#define X(n) n,
    KIWI_TYPE_TYPE(X)
#undef X
        object,
    type
};

inline const std::string &name(BuiltinTypeTag id) {
    static std::unordered_map<BuiltinTypeTag, std::string> builtin_types = {
#define X(n) {BuiltinTypeTag::n, #n},
        KIWI_TYPE_TYPE(X)
#undef X
    };
    return builtin_types[id];
}

template <typename T> BuiltinTypeTag type_id() { return BuiltinTypeTag::object; }
template <> inline BuiltinTypeTag type_id<typetype>() { return BuiltinTypeTag::type; }

#define X(n)                                                                                       \
    template <> inline BuiltinTypeTag type_id<n>() { return BuiltinTypeTag::n; }
KIWI_TYPE_TYPE(X)
#undef X

template <typename T> Type *type() {
    static BuiltinType t(name(type_id<T>()));
    return &t;
}

class Value : public Expression {
  public:
    template <typename T>
    Value(T x) :
        Expression(NodeTag::value), _self(std::make_shared<Model<T>>(std::move(x))),
        type(kiwi::type_id<T>()) {}

    template <typename T> const T &as() const {
        debug_if(type != type_id<T>(), this);
        assert(type == type_id<T>() && "wrong Value cast");
        return reinterpret_cast<const Model<T> *>(_self.get())->_data;
    }

    template <typename T> const T &as(const T &) const {
        debug_if(type != type_id<T>(), this);
        assert(type == type_id<T>() && "wrong Value cast");
        return reinterpret_cast<const Model<T> *>(_self.get())->_data;
    }

    std::ostream &dump(std::ostream &out) const { return print(out); }

    std::ostream &print(std::ostream &out, bool ptype = false) const {
        switch(type) {
#define X(n)                                                                                       \
    case BuiltinTypeTag::n: {                                                                      \
        if(ptype) {                                                                                \
            out << "(";                                                                            \
        }                                                                                          \
        out << as<n>();                                                                            \
        if(ptype) {                                                                                \
            out << ": " << name(BuiltinTypeTag::n) << ")";                                         \
        }                                                                                          \
        break;                                                                                     \
    }
            KIWI_TYPE_TYPE(X)
#undef X
        // this is a dummy type
        case BuiltinTypeTag::type: {
            out << "Type";
            break;
        }
        // this a user defined object i.e a Kiwi object
        case BuiltinTypeTag::object: {
            out << "Object";
            break;
        }
        }
        out.flush();
        return out;
    }

    const BuiltinTypeTag type;

  private:
    struct Concept {
        virtual ~Concept() = default;
    };

    template <typename T> struct Model final : Concept {
        Model(T x) : _data(std::move(x)) {}

        T _data;
    };

    std::shared_ptr<const Concept> _self;
};

template <typename NodeTrait> std::ostream &operator<<(std::ostream &out, const Value *v) {
    return v->print(out);
}

template <typename T, typename NodeTrait> const T &as(Value *expr) { return expr->as(T()); }

} // namespace kiwi
