#ifndef KIWI_AST_BUILDER_HEADER
#define KIWI_AST_BUILDER_HEADER

#define PARENT(x)

#include "Definition.h"
#include "Expression.h"
#include "Stack.h"
#include "Type.h"
#include "Value.h"
#include <cmath>

namespace kiwi {

class BuilderContext {
  public:
    BuilderContext() {
        owned = true;
        ctx_stack.push(new SubContext(0));
    }

    ~BuilderContext() {
        if(owned) {
            SubContext *ctx = ctx_stack.pop();
            delete ctx;
        }
    }

    Array<Statement *> get_statements() { return ctx_stack.last()->statements; }

    // Some statements are not addressabable
    void insert(Statement *stmt) {
        ctx_stack.last()->insert(stmt);
        size += 1;
    }

    // Addressable statement
    void insert(String const &str, Statement *stmt) {
        ctx_stack.last()->insert(str, stmt);
        SubContext *ctx = ctx_mappings[str];

        // Variable Shadowing
        if(ctx) {
            // Print Warning
            previous_ctx[ctx_stack.last()] = ctx;
            shadowed_variables[ctx_stack.last()].push_back(str);
        }

        ctx_mappings[str] = ctx_stack.last();
        size += 1;
    }

    void enter_scope() { ctx_stack.push(new SubContext(ctx_stack.last()->depth + 1)); }

    void exit_scope() {
        SubContext *ctx  = ctx_stack.pop();
        SubContext *pctx = previous_ctx[ctx];
        // Shadowed Variables detected need to restore previous context
        if(pctx) {
            Array<String> &vars = shadowed_variables.at(ctx);
            for(String &str : vars) {
                ctx_mappings[str] = pctx;
            }
            shadowed_variables.erase(ctx);
            previous_ctx.erase(ctx);
        }
        delete ctx;
    }

    Statement *get_statement(String const &str) const {
        SubContext *ctx = ctx_mappings.at(str);
        if(ctx)
            return ctx->get_statement(str);
        return nullptr;
    }

    int32 get_ref_index(String const &str) const {
        SubContext *ctx = ctx_mappings.at(str);
        if(ctx)
            return ctx->get_ref_index(str);
        return -1;
    }

    u64 depth() { return ctx_stack.last()->depth; }

  private:
    struct SubContext {
        Array<Statement *> statements;
        Dict<String, u64> mappings;
        u64 depth;

        SubContext(u64 depth) : depth(depth) {}

        // Some statement are not addressabable
        void insert(Statement *stmt) { statements.push_back(stmt); }

        // Addressable statement
        void insert(String const &str, Statement *stmt) {
            mappings[str] = statements.size();
            insert(stmt);
        }

        int32 get_ref_index(String const &str) const {
            if(mappings.count(str) > 0)
                return int32(mappings.at(str));
            else
                return -1;
        }

        Statement *get_statement(String const &str) const {
            int32 index = get_ref_index(str);
            return get_statement(index);
        }

        Statement *get_statement(int32 index) const {
            if(index >= 0)
                return statements.at(std::size_t(index));
            return nullptr;
        }
    };

    Stack<SubContext *> ctx_stack;
    Dict<String, SubContext *> ctx_mappings;
    Dict<SubContext *, SubContext *> previous_ctx;
    Dict<SubContext *, Array<String>> shadowed_variables;
    bool owned = true;
    u64 size;
};

/*
class Builder {
  public:
    using ExpressionPtr = Expression *; // UniquePtr<Expression>;
    using TypePtr       = Type *;       // UniquePtr<Type>;

  private:
    //;
    BuilderContext *current_ctx;

  public:
    Builder(BuilderContext *ctx) : current_ctx(ctx) {
        // stack_ctx.push(ctx);
    }

    virtual ~Builder() {}

    BuilderContext *context() { return current_ctx; }

    FunctionDefinition *start_function(String const &op) {
        FunctionDefinition *root = new FunctionDefinition(op);
        current_ctx->insert(op, root);

        return root;
    }

    static ExpressionPtr call(ExpressionPtr const op, Array<Expression *> args) {
        return ExpressionPtr(new FunctionCall(op, args));
    }

    static ExpressionPtr unary_call(ExpressionPtr const op, const ExpressionPtr &expr) {
        return new UnaryCall(op, expr);
    }

    static ExpressionPtr binary_call(ExpressionPtr const op, const ExpressionPtr &lhs,
                                     const ExpressionPtr &rhs) {
        return new BinaryCall(op, lhs, rhs);
    }

    static ExpressionPtr error(String const &message) { return new ErrorNode(message); }

    static ExpressionPtr value(double value) { return new PrimitiveValue(value); }

    static ExpressionPtr placeholder(String const &name) { return new Placeholder(name); }

    // static ExpressionPtr borrow(Node *expr) { return new Borrow(expr); }

    static TypePtr function_type(Array<Type *> args, const TypePtr &return_type) {
        return new FunctionType(args, return_type);
    }

    static TypePtr arrow(Array<Type *> args, const TypePtr &return_type) {
        return function_type(args, return_type);
    }

    static ExpressionPtr block() { return new Block(); }

    static TypePtr builtin(String const &name) { return new BuiltinType(name); }

    bool is_zero(f64 value) { return std::abs(value) <= 1e-12; }

    bool is_one(f64 value) { return is_zero(value - 1); }

    static ExpressionPtr add(const ExpressionPtr &lhs, const ExpressionPtr &rhs) {
        ExpressionPtr root = binary_call(placeholder("+"), lhs, rhs);
        PARENT(lhs->parent = root);
        PARENT(rhs->parent = root);
        return root;
    }

    static ExpressionPtr mult(const ExpressionPtr &lhs, const ExpressionPtr &rhs) {
        ExpressionPtr root = binary_call(placeholder("*"), lhs, rhs);
        PARENT(lhs->parent = root);
        PARENT(rhs->parent = root);
        return root;
    }

    static ExpressionPtr div(const ExpressionPtr &lhs, const ExpressionPtr &rhs) {
        ExpressionPtr root = binary_call(placeholder("/"), lhs, rhs);
        PARENT(lhs->parent = root);
        PARENT(rhs->parent = root);
        return root;
    }

    static ExpressionPtr sub(const ExpressionPtr &lhs, const ExpressionPtr &rhs) {
        ExpressionPtr root = binary_call(placeholder("-"), lhs, rhs);
        PARENT(lhs->parent = root);
        PARENT(rhs->parent = root);
        return root;
    }

    static Definition *struct_def(String const &name, Array<Tuple<String, Statement *>> const &meta,
                                  Array<Tuple<String, Statement *>> const &attr) {
        return new StructDefinition(name, meta, attr);
    }

    static Definition *union_def(String const &name, Array<Tuple<String, Statement *>> const &meta,
                                 Array<Tuple<String, Statement *>> const &attr) {
        return new UnionDefinition(name, meta, attr);
    }

    static FunctionDefinition *function(String const &op, ExpressionPtr body) {
        FunctionDefinition *root = new FunctionDefinition(op);
        root->body               = body;

        return root;
    }
};*/

class UnionBuilder {
  public:
    UnionBuilder(BuilderContext *ctx) : ctx(ctx) {}

    // Union with Bind
    UnionBuilder(BuilderContext *ctx, String const &name) : ctx(ctx) { ctx->insert(name, un); }

  public:
    UnionBuilder *add_meta_type(String const &name, Statement *stmt) {
        ctx->insert(name, stmt);
        un->add_meta_type(name, stmt);
        return this;
    }

    UnionBuilder *add_attribute(String const &name, Statement *stmt) {
        ctx->insert(name, stmt);
        un->add_attribute(name, stmt);
        return this;
    }

    Union *build() {
        ctx->exit_scope();
        return un;
    }

  private:
    BuilderContext *ctx;
    Union *un = new Union();
};

class StructBuilder {
  public:
    StructBuilder(BuilderContext *ctx) : ctx(ctx) {}

    // Struct with Bind
    StructBuilder(BuilderContext *ctx, String const &name) : ctx(ctx) { ctx->insert(name, stru); }

  public:
    StructBuilder *add_meta_type(String const &name, Statement *stmt) {
        ctx->insert(name, stmt);
        stru->add_meta_type(name, stmt);
        return this;
    }

    StructBuilder *add_attribute(String const &name, Statement *stmt) {
        ctx->insert(name, stmt);
        stru->add_attribute(name, stmt);
        return this;
    }

    Struct *build() {
        ctx->exit_scope();
        return stru;
    }

  private:
    BuilderContext *ctx;
    Struct *stru = new Struct();
};

class FunctionBuilder {
  public:
    FunctionBuilder(BuilderContext *ctx) : ctx(ctx) {}

    // Function with Bind
    FunctionBuilder(BuilderContext *ctx, String const &name) : ctx(ctx) { ctx->insert(name, fun); }

  public:
    FunctionBuilder *add_arg(String const &name, Statement *stmt) {
        ctx->insert(name, stmt);
        fun->add_arg(name, stmt);
        return this;
    }

    FunctionBuilder *add_body(Expression *stmt) {
        fun->body = stmt;
        return this;
    }

    FunctionBuilder *set_return_type(Statement *ret) {
        fun->return_type = ret;
        return this;
    }

    Function *build() {
        ctx->exit_scope();
        return fun;
    }

  private:
    BuilderContext *ctx;
    Function *fun = new Function();
};

class UnionValueBuilder {
  public:
    UnionValueBuilder(Union *type) { un_val->set_type(type); }

    // Union Value with Bind
    UnionValueBuilder(BuilderContext *ctx, String const &name, Union *type) {
        ctx->insert(name, un_val);
        un_val->set_type(type);
    }

  public:
    UnionValueBuilder *set_value(String const &name, Value *stmt) {
        un_val->index = un_val->definition()->get_attribute_index(name);
        un_val->value = stmt;
        return this;
    }

    UnionValue *build() { return un_val; }

  private:
    UnionValue *un_val = new UnionValue();
};

class StructValueBuilder {
  public:
    StructValueBuilder(Struct *type) { struct_val->set_type(type); }

    // Struct Value with Bind
    StructValueBuilder(BuilderContext *ctx, String const &name, Struct *type) {
        ctx->insert(name, struct_val);
        struct_val->set_type(type);
    }

  public:
    StructValueBuilder *set_value(String const &name, Value *stmt) {
        auto index = struct_val->definition()->get_attribute_index(name);
        if(index >= 0)
            struct_val->values[std::size_t(index)] = stmt;
        return this;
    }

    StructValue *build() { return struct_val; }

  private:
    StructValue *struct_val = new StructValue();
};

// Need to spawn a builder and ctx per thread
struct Builder {
    Builder(BuilderContext *ctx) : ctx(ctx) { assert(ctx != nullptr); }

    FunctionBuilder make_function() { return FunctionBuilder(ctx); }

    UnionBuilder make_union() { return UnionBuilder(ctx); }

    StructBuilder make_struct() { return StructBuilder(ctx); }

    FunctionBuilder make_function(String const &name) { return FunctionBuilder(ctx, name); }

    UnionBuilder make_union(String const &name) { return UnionBuilder(ctx, name); }

    StructBuilder make_struct(String const &name) { return StructBuilder(ctx, name); }

    Union *make_union(String const &name, Array<Tuple<String, Statement *>> const &meta,
                      Array<Tuple<String, Statement *>> const &attr) {
        auto s = new Union(meta, attr);
        ctx->insert(name, s);
        return s;
    }
    Struct *make_struct(String const &name, Array<Tuple<String, Statement *>> const &meta,
                        Array<Tuple<String, Statement *>> const &attr) {
        auto s = new Struct(meta, attr);
        ctx->insert(name, s);
        return s;
    }

    // Leaves
    PlaceholderReference *get_ctx_ref(String const &name) {
        return new PlaceholderReference(name, ctx->get_ref_index(name));
    }

    template <typename T> Value *make_value(T val) { return new PrimitiveValue(val); }

    template <typename T> Value *make_value(String const &name, T val) {
        auto v = new PrimitiveValue(val);
        context()->insert(name, v);
        return v;
    }

    BuiltinType *make_builtin(String const &name) { return new BuiltinType(name); }

    UnionValueBuilder *make_union_value(String const &type_name) {
        Statement *type = ctx->get_statement(type_name);
        if(type->tag == NodeTag::struct_def) {
            return new UnionValueBuilder(static_cast<Union *>(type));
        }
        printf("Could not find the union type %s\n", type_name.c_str());
        return nullptr;
    }

    StructValueBuilder *make_struct_value(String const &type_name) {
        Statement *type = ctx->get_statement(type_name);
        if(type->tag == NodeTag::struct_def) {
            return new StructValueBuilder(static_cast<Struct *>(type));
        }
        printf("Could not find the struct type %s\n", type_name.c_str());
        return nullptr;
    }

    Expression *make_unary_call(String const &op_name, Expression *expr) {
        return new UnaryCall(get_ctx_ref(op_name), expr);
    }

    Expression *make_binary_call(String const &op_name, Expression *lhs, Expression *rhs) {
        return new BinaryCall(get_ctx_ref(op_name), lhs, rhs);
    }

    Expression *make_placeholder(String const name) {
        auto place = new Placeholder(name);
        ctx->insert(name, place);
        return place;
    }

  private:
    BuilderContext *ctx;
};

} // namespace kiwi

#endif
