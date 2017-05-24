#include "../AST/Visitor.h"
#include <SFML/Graphics.hpp>

namespace kiwi{

class RenderNode{
public:
    RenderNode(NodeTag tag):
        tag(tag)
    {}

    NodeTag tag;
    PARENT(Expression* parent = nullptr;)
};

namespace generic{
    template<>
    class NodeTrait<RenderNode>{
    public:
        typedef sf::Text                 StringType;
        typedef std::vector<RenderNode*> Args;
        typedef std::vector<StringType>  ArgNames;
        typedef std::size_t              IndexType;
    };
}

#define X(name, object) typedef generic::object<RenderNode> R ## object;
    KIWI_AST_NODES
#undef X

template<typename Visitor, typename RetType, typename ...Args>
class RenderVisitor: public generic::StaticVisitor<Visitor, RenderNode, RetType, Args...>{
};

}
