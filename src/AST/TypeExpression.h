#include "../Types.h"
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
        X(arrow, Arrow)    \
        X(record, Record)  \
        X(unknown, Unknown)\
        X(error, ErrorType)

    enum class NodeTypeTag {
    #define X(name, object) name,
        KIWI_TYPE_NODES
    #undef X
    };

    // Thanks gcc for forcing us to use such beautiful macro
    #define NODE_TYPES\
        typedef typename NodeTrait::StringType StringType;\
        typedef typename NodeTrait::StringView StringView;\
        typedef typename NodeTrait::StringArgument StringArgument;\
        typedef typename NodeTrait::ArgNames   ArgNames;\
        typedef typename NodeTrait::ArgTypes   ArgTypes;\
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

        Arrow(const ArgTypes& args, Type<NodeTrait>* return_type) :
            Type<NodeTrait>(NodeTypeTag::arrow), args(args), return_type(return_type)
        {}

        size_t args_size() { return args.size(); }
        Type<NodeTrait>* arg(size_t i) { return args[i]; }

        ArgTypes args;
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
    //  Functional
    //
    //      class MyType(my_x: Int, my_u: Int)
    //
    // We allow the users to defined methods. 
    // Methods are functions that always have at leat one argument
    // The first agurment is always a reference of type `object`
    // Since methods are namespaced functions we do not support 
    // virtual calls and as such we do not support OOP (:D)
    //

    // (name: Type * name: Type)
    template<typename NodeTrait>
    class Record : public Type<NodeTrait> {
    public:
        NODE_TYPES;

        Record(String const& name, bool is_union = false):
            Type<NodeTrait>(NodeTypeTag::record), name(name), is_union(is_union)
        {}

        bool insert_attribute(String const& name, Type<NodeTrait*> type) {
            using NameIndexResult = std::pair<Dict<String, size_t>::iterator, bool>;

            size_t size = index_name.size();
            NameIndexResult result = name_index.insert(std::make_pair(name, size));

            // insertion was successful
            if (result.second) {
                index_name.push_back(std::make_tuple(name, type));
                return true;
            }
            else {
                return false;
            }
        }

        size_t               get_index(StringType const& name) const { return name_index[name];           }
        StringType const&    get_name(size_t index)            const { return index_name[index].first;    }
        Type<NodeTrait>*     get_type(size_t index)            const { return index_name[index].second;   }
        Type<NodeTrait>*     get_type(StringType const& name)  const { return get_type(get_index(name));  }

        size_t arg_size() const { return index_name.size();  }

        const StringType name;
        const bool is_union;

    private:
        Array<Tuple<StringType, Type<NodeTrait>*>> index_name;
        Dict<StringType, size_t> name_index;
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