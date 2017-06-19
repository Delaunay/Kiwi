#include "../AST/Visitor.h"
#include <SFML/Graphics.hpp>

#include "../AST/Builder.h"
#include "Fonts.h"

namespace kiwi{

class RenderNode{
public:
    typedef sf::Text                 StringType;
    typedef std::vector<RenderNode*> Args;
    typedef std::vector<StringType>  ArgNames;
    typedef std::size_t              IndexType;

    RenderNode(NodeTag tag):
        tag(tag)
    {}

    static StringType make_string(const std::string& str){
        return StringType(str, default_font(), default_font_size());
    }

    NodeTag tag;
    PARENT(RenderNode* parent = nullptr;)
};


typedef AST<RenderNode> RenderTree;

class RenderVisitor: public generic::StaticVisitor<RenderVisitor, RenderTree, sf::Vector2f, sf::Vector2f, int>{
public:
    typedef generic::Root<RenderTree::Node> Root;
    typedef Builder<Root, RenderTree> NBuilder;
    typedef typename RenderTree::Call Call;
    #define X(name, object) typedef typename RenderTree::object object;
        KIWI_AST_NODES
    #undef X

    RenderVisitor(sf::RenderWindow& rw, sf::Vector2f pos):
        rw(rw), original_pos(pos)
    {}

    sf::Font font = default_font();
    std::size_t font_size = default_font_size();
    sf::Text fun_def = sf::Text("def ", font, font_size);
    sf::Text fun_op  = sf::Text("(", font, font_size);
    sf::Text fun_cp  = sf::Text("):", font, font_size);
    sf::Text fun_com = sf::Text(", ", font, font_size);
    sf::Text space = sf::Text(" ", font, font_size);

    int char_width = 10;
    sf::Vector2f original_pos;
    std::ostream& out = std::cout;
    sf::RenderWindow& rw;

    // helpers
    // ------------------------------------------------------------------------
    sf::Vector2f render(sf::Text item, sf::Vector2f pos){
        item.setPosition(pos.x, pos.y);
        rw.draw(item);
        pos.x += item.getGlobalBounds().width;
        return pos;
    }

    sf::Vector2f new_line(sf::Vector2f pos, int indent){
        pos.y += font.getLineSpacing(10);
        pos.x = original_pos.x + char_width * indent;
        return pos;
    }

    static Root make_sqr(){
        Root x    = NBuilder::placeholder("x");
        Root body = NBuilder::mult(x, NBuilder::borrow(x));
        Root sqr  = NBuilder::function("sqr", body);
        RenderNode* node = sqr.get();

        // generic::Function<RenderNode>*
        Function* f = static_cast<Function*>(node);
        f->args.push_back(sf::Text("x", default_font(), 10));

        return sqr;
    }

    // ------------------------------------------------------------------------
    static sf::Vector2f run(sf::RenderWindow& rw, RenderNode* expr, sf::Vector2f pos){
        RenderVisitor eval(rw, pos);
        return eval.traverse(expr, pos, 0);
    }

    sf::Vector2f function(Function* x, sf::Vector2f pos, int idt){
        pos = render(fun_def, pos);
        pos = render(x->name, pos);
        pos = render(fun_op, pos);

        int n = int(x->args_size()) - 1;
        //out << "def " << x->name << "(";

        if (n >= 0){
            for(int i = 0; i < n; ++i){
                pos = render(x->arg(i), pos);
                pos = render(fun_com, pos);
            }
            pos = render(x->arg(n), pos);
        }

        pos = render(fun_cp, pos);
        pos = new_line(pos);
        pos = indent(pos, idt + 1);
        return traverse(x->body, pos, idt + 1);
    }

    sf::Vector2f new_line(sf::Vector2f pos){
        return pos + sf::Vector2f(0, font.getLineSpacing(font_size));
    }

    sf::Vector2f indent(sf::Vector2f pos, int idt){
        return sf::Vector2f(space.getGlobalBounds().width * 4 * idt , pos.y);
    }

    sf::Vector2f function_call(FunctionCall* x, sf::Vector2f pos, int indent){/*
        std::size_t n = x->args_size() - 1;

        out << x->name << "(";
        for(int i = 0; i < n; ++i){
            traverse(x->arg(i), indentation);
            out << ", ";
        }

        traverse(x->arg(n), indentation);
        out << ")";*/
        return pos;
    }

    sf::Vector2f unary_call(UnaryCall* x, sf::Vector2f pos, int indent){/*
        if (!x->right)
            out << x->name;

        traverse(x->arg(0), indentation);

        if (x->right)
            out << x->name;*/
        return pos;
    }

    sf::Vector2f binary_call(BinaryCall* x, sf::Vector2f pos, int indent){/*
        out << "(";
        traverse(x->arg(0), indentation);
        out << " " << x->name << " ";
        traverse(x->arg(1), indentation);
        out << ")";*/
        pos = traverse(x->lhs, pos, indent);
        pos = render(x->name, pos);
        pos = traverse(x->rhs, pos, indent);
        return pos;
    }

    sf::Vector2f borrow(Borrow* b, sf::Vector2f pos, int indent){
        return traverse(b->expr, pos, indent);
    }

    sf::Vector2f arrow(Arrow* x, sf::Vector2f pos, int indent){/*
        out << "(";

        int n = int(x->args.size()) - 1;
        for(int i = 0; i < n; ++i){
            traverse(x->args[i], indentation);
            out << ", ";
        }
        traverse(x->args[n], indentation);

        out << ") -> ";
        traverse(x->return_type, indentation);*/
        return pos;
    }

    sf::Vector2f type(Type* x, sf::Vector2f pos, int indent){
        //out << x->name.getString();
        return pos;
    }

    sf::Vector2f builtin(Builtin* e, sf::Vector2f pos, int indent){
        //out << "(Builtin " << e->name.getString() << ")";
        return pos;
    }

    sf::Vector2f error(ErrorNode* e, sf::Vector2f pos, int indent){
        //out << "(error, m = " << e->message.getString() << ")";
        return pos;
    }

    sf::Vector2f value(Value* x, sf::Vector2f pos, int indent){
        sf::Text txt(std::to_string(x->as<f64>()), font, 10);
        return render(txt, pos);
    }

    sf::Vector2f placeholder(Placeholder* x, sf::Vector2f pos, int indent){
        return render(x->name, pos);
    }
};

}
