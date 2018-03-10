#include <unordered_map>

//#define LINK_TO_PARENT
//#define VTABLE_VISITOR

// Shall we enable children to have a reference to
// their parents
#ifdef LINK_TO_PARENT
#   define PARENT(X) X
#else
#   define PARENT(X)
#endif

// Shall we use a vtable
#ifdef VTABLE_VISITOR
#   define VTABLEV(X) X
#   define STATICV(X)
#else
#   define VTABLEV(X)
#   define STATICV(X) X
#endif

/*  Expression Representing Types
*
*  Because we want a dependent types we need something to bridge between expression and types
*
*/
namespace kiwi {
    #define KIWI_TYPE_NODES\
        X(builtin, Builtin)\
        X(arrow, Arrow)\
        X(record, Record)\
        X(unknown, Unknown)\
        X(error, ErrorType)

    enum class NodeTypeTag {
    #define X(name, object) name,
        KIWI_TYPE_NODES
    #undef X
    };

    #define NODE_TYPES\
        typedef typename NodeTrait::StringType StringType;\
        typedef typename NodeTrait::Args       Args;\
        typedef typename NodeTrait::ArgNames   ArgNames;\
        typedef typename NodeTrait::IndexType  IndexType;

    template <typename T>
    class Expression;

    template<typename NodeTrait>
    class Type: public NodeTrait{
    public:
        Type(NodeTypeTag tag) :
            tag(tag)
        {}

        const NodeTypeTag tag;

        VTABLEV(virtual void visit(class DynamicTypeVisitor* v) = 0;);
        PARENT(Expression<NodeTrait>* parent = nullptr;);
    };

    // Supporting Currying does not interest us
    // (int, double) -> double
    template<typename NodeTrait>
    class Arrow : public Type<NodeTrait> {
    public:
        NODE_TYPES;
            
        Arrow() :
            Type<NodeTrait>(NodeTypeTag::arrow)
        {}

        Arrow(const Args& args, Type<NodeTrait>* return_type) :
            Type<NodeTrait>(NodeTypeTag::arrow), args(args), return_type(return_type)
        {}

        int args_size() { return args.size(); }
        Type<NodeTrait>* arg(int i) { return args[i]; }

        Args  args;
        Type<NodeTrait>* return_type{ nullptr };
    };

    // User defined Type
    // ----------------------------------
    // Tuple like syntax:
    //
    //      MyType = (my_x: Int, my_y: Int)
    //
    // ----------------------------------
    //  OOP like syntax:
    //
    //      class MyType:
    //          my_x: Int
    //          my_y: Int
    //
    // We allow the users to defined methods. 
    // Methods are functions that always have at leat one argument
    // The first agurment is always a reference of type `object`
    // Since methods are namespaced functions we do not support 
    // virtual calls and as such we do not support OOP (:D)
    //

    template<typename NodeTrait>
    class Record : public Type<NodeTrait> {
    public:
        NODE_TYPES;

        Record() :
            Type<NodeTrait>(NodeTypeTag::record)
        {}

        const StringType name;

        std::unordered_map<StringType, Type<NodeTrait>> types;
        std::unordered_map<StringType, Expression<NodeTrait>> attributes;
    };

    //  Builtin Types
    //  Mainly used to represent int/float/char/...
    template<typename NodeTrait>
    class Builtin : public Type<NodeTrait> {
    public:
        NODE_TYPES;

        Builtin(const std::string& name) :
            Type<NodeTrait>(NodeTypeTag::builtin), 
            name(NodeTrait::make_builtin_name(name))
        {}

        const StringType name;
    };

    //
    //  A type that should be resolved later some type X
    template<typename NodeTrait>
    class Unknown : public Type<NodeTrait> {
    public:
        NODE_TYPES;

        Unknown() :
            Type<NodeTrait>(NodeTypeTag::unknown)
        {}

        bool is_resolved() { return resolved_type; }

        Type<NodeTrait>* resolved_type{ nullptr };
    };


    template<typename NodeTrait>
    class ErrorType : public Type<NodeTrait> {
    public:
        NODE_TYPES;
            
        ErrorType() :
            Type<NodeTrait>(NodeTypeTag::error)
        {}

        StringType message;
    };
}