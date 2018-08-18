#ifndef KIWI_AST_STATEMENT_HEADER
#define KIWI_AST_STATEMENT_HEADER

#include "Tag.h"

namespace kiwi {

/*
 * Everything is Kiwi is a Statement
 */
class Statement {
  public:
    Statement(NodeTag tag) : tag(tag) {}

    ~Statement() {}

    bool is_def() { return resolve_tag() & DEF_TAG; }
    bool is_type() { return resolve_tag() & TYPE_TAG; }
    bool is_expr() { return resolve_tag() & EXPR_TAG; }

    i8 resolve_tag();
    NodeTag tag;
};

} // namespace kiwi

#endif
