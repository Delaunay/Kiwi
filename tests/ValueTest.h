#pragma once

#include "AST/LightAST.h"
#include <gtest/gtest.h>

#include "AST/Expression.h"
#include "AST/KExpression.h"

//#define KIWI_DEBUG
#include "../src/Debug.h"

using namespace kiwi;

TEST(Value, BuiltinTypes) {
    PrimitiveValue(2).dump(std::cout) << std::endl;
    PrimitiveValue(2.0).dump(std::cout) << std::endl;
    // PrimitiveValue("hello").dump(std::cout) << std::endl;
}

TEST(Value, StructType) {
    // Value(Array<Value *> x, StructType *type)
    Struct *def = new Struct();
    def->add_attribute("x", get_primitive_type<i64>());
    def->add_attribute("y", get_primitive_type<f64>());

    // StructType *stype  = new StructType(def);
    Array<Value *> arr = {new PrimitiveValue(2), new PrimitiveValue(2.0)};

    auto v = new StructValue();
    v->set_type(def);
    v->dump(std::cout) << std::endl;
}

TEST(Value, UnionType) {
    // Value(Array<Value *> x, StructType *type)
    Union *def = new Union();
    def->add_attribute("x", get_primitive_type<i64>());
    def->add_attribute("y", get_primitive_type<f64>());

    // UnionType *stype = new UnionType(def);
    auto v = new UnionValue();
    v->set_type(def);
    v->index = 0;
    v->value = new PrimitiveValue(2);
    v->dump(std::cout) << std::endl;
}

TEST(Value, UnionTypeNone) {
    // Value(Array<Value *> x, StructType *type)
    Union *def = new Union();
    def->add_attribute("x", get_primitive_type<i64>());
    def->add_attribute("y", get_primitive_type<f64>());

    // UnionType *stype = new UnionType(def);
    auto v = new UnionValue();
    v->set_type(def);
    v->index = 0;
    v->value = new PrimitiveValue(2);
    v->dump(std::cout) << std::endl;
}
