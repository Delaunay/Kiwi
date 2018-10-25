#include "Tag.h"

namespace kiwi {

i8 Node::resolve_tag() {
    if(tag == NodeTag::borrow)
        return i8(static_cast<Borrow *>(this)->expr->tag);
    return i8(tag);
}

const char *to_string(NodeTag tag) {
    static std::unordered_map<NodeTag, const char *> tags = {
#define X(name, object, val) {NodeTag::name, #object},
        KIWI_AST_NODES(X)
#undef X
    };
    return tags[tag];
}

} // namespace kiwi
