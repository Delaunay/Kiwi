#include "Tag.h"

namespace kiwi {

i8 Node::resolve_tag() {
    if(tag == NodeTag::borrow)
        return i8(static_cast<Borrow *>(this)->expr->tag);
    return i8(tag);
}

} // namespace kiwi
