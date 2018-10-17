#pragma once
#include "AST/Builder.h"
#include <gtest/gtest.h>

using namespace kiwi;

void print(std::string const &str, BuilderContext const &ctx) {
    std::cout << str << ": (";
    Statement *stmt = ctx.get_statement("a");
    if(stmt->tag == NodeTag::value) {
        Value *v = static_cast<Value *>(stmt);
        v->dump(std::cout) << ")" << std::endl;
    }
}

TEST(BuilderContext, NestedScope) {
    BuilderContext ctx;

    // clang-format off
    ctx.insert("a", new PrimitiveValue(1.0));
    ctx.insert("c", new PrimitiveValue(5.0));
    ctx.insert(new PrimitiveValue(10));
    ctx.enter_scope();
        ctx.insert("a", new PrimitiveValue(2.0));
        ctx.insert("c", new PrimitiveValue(4.0));
        ctx.enter_scope();
            ctx.insert("a", new PrimitiveValue(3.0));
            ctx.insert("c", new PrimitiveValue(3.0));
            ctx.enter_scope();
                ctx.insert("a", new PrimitiveValue(4.0));
                ctx.insert("c", new PrimitiveValue(2.0));
                ctx.enter_scope();
                    ctx.insert("a", new PrimitiveValue(5.0));
                    ctx.insert("c", new PrimitiveValue(1.0));
                    ctx.enter_scope();
                    print("a", ctx);
                    ctx.exit_scope();
                print("a", ctx);
                ctx.exit_scope();
            print("a", ctx);
            ctx.exit_scope();
        print("a", ctx);
        ctx.exit_scope();
    print("a", ctx);
    ctx.exit_scope();
    // clang-format on
}
