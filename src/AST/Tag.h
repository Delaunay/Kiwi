#ifndef KIWI_AST_NODE_TAG_HEADER
#define KIWI_AST_NODE_TAG_HEADER

#include "../Types.h"

namespace kiwi {

#define DEF_TAG 1 << 7
#define TYPE_TAG 1 << 6
#define EXPR_TAG 1 << 5
#define TYPE_DEF_TAG DEF_TAG | TYPE_TAG

// Last 3 Bytes are used to tag node kind
#define KIWI_AST_NODES(X)                                                                          \
    X(borrow, Borrow, i8(1))                                                                       \
    X(definition, Definition, 0 | DEF_TAG)                                                         \
    X(function_def, FunctionDefinition, 1 | DEF_TAG)                                               \
    X(macro_def, MacroDefinition, 2 | DEF_TAG)                                                     \
    X(type, Type, 0 | TYPE_TAG)                                                                    \
    X(struct_def, StructDefinition, 0 | TYPE_DEF_TAG)                                              \
    X(union_def, UnionDefinition, 1 | TYPE_DEF_TAG)                                                \
    X(builtin_type, BuiltinType, 2 | TYPE_TAG | EXPR_TAG)                                          \
    X(function_type, FunctionType, 3 | TYPE_TAG | EXPR_TAG)                                        \
    X(unknown_type, UnknownType, 4 | TYPE_TAG | EXPR_TAG)                                          \
    X(error_type, ErrorType, 5 | TYPE_TAG | EXPR_TAG)                                              \
    X(union_type, UnionType, 6 | TYPE_TAG | EXPR_TAG)                                              \
    X(struct_type, StructType, 7 | TYPE_TAG | EXPR_TAG)                                            \
    X(expression, Expression, 0 | EXPR_TAG)                                                        \
    X(unary_call, UnaryCall, 1 | EXPR_TAG)                                                         \
    X(binary_call, BinaryCall, 2 | EXPR_TAG)                                                       \
    X(function_call, FunctionCall, 3 | EXPR_TAG)                                                   \
    X(match, Match, 4 | EXPR_TAG)                                                                  \
    X(block, Block, 5 | EXPR_TAG)                                                                  \
    X(placeholder, Placeholder, 6 | EXPR_TAG)                                                      \
    X(placeholder_ref, PlaceholderReference, 7 | EXPR_TAG)                                         \
    X(value, Value, 8 | EXPR_TAG)

enum class NodeTag : u8 {
#define X(name, object, val) name = val,
    KIWI_AST_NODES(X)
#undef X
};

const char *to_string(NodeTag tag);

class Node {
  public:
    Node(NodeTag tag) : tag(tag) {}

    ~Node() {}

    NodeTag tag;

    i8 resolve_tag();

    bool is_def() { return resolve_tag() & DEF_TAG; }
    bool is_type() { return resolve_tag() & TYPE_TAG; }
    bool is_expr() { return resolve_tag() & EXPR_TAG; }
};

/* Borrow is a dummy Node which only exists to help
 * memory management. A Borrowed node is a node owned
 * by another tree. i.e we don't need to free it
 *
 * Borrow is mostly during meta programming phase
 * when the user generates code from a programm.
 * Borrow allow us to reuse the original program subtree
 * instead of copying it
 */
class Borrow final : public Node {
  public:
    Borrow(Node *expr) : Node(NodeTag::borrow), expr(expr) {}

    Node *expr{nullptr};
};
}

#endif
