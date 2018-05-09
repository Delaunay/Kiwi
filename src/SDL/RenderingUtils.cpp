#include "RenderingUtils.h"

#include "../AST/Builder.h"
#include "../AST/Visitor.h"

/*
template<typename T, typename S, typename fun>
static std::vector<T> map(std::vector<S> const& s, fun op) {
std::vector<T> v(s.size());

for (std::size_t i = 0; i < s.size(); ++i) {
v[i] = fun(s[i]);
}

return v;
}*/


namespace kiwi {
#undef log_trace
#define log_trace(...) LOG_INTERNAL(LogLevel::trace, __VA_ARGS__)

    RenderTrait::StringType convert_string(ASTTrait::StringType const& str) {
        return str;
    }

    std::string convert_stdstring(ASTTrait::StringType const& str) {
        return str;
    }

    class TypeConverterAST2Render : public StaticTypeVisitor<TypeConverterAST2Render, ASTTrait, generic::Root<Type<RenderTrait>>> {
    public:
        static return_value run(Type<ASTTrait>* expr) {
            TypeConverterAST2Render eval;
            return eval.convert(expr);
        }

        return_value convert(Type<ASTTrait>* expr) {
            return traverse(expr);
        }

        return_value arrow(Arrow<ASTTrait>* x) {
            std::vector<Type<ASTTrait>*>& s = x->args;
            std::vector<Type<RenderTrait>*> v(s.size());

            for (std::size_t i = 0; i < s.size(); ++i) {
                v[i] = this->convert(s[i]);
            }

            return Builder<RenderTrait>::arrow(v, convert_type(x->return_type));
        }

        return_value builtin(Builtin<ASTTrait>* e) {
            return Builder<RenderTrait>::builtin(e->name);
        }

        return_value error(ErrorType<ASTTrait>* e) {
            return nullptr;
        }

        return_value unkown(Unknown<ASTTrait>* e) {
            return nullptr;
        }

        return_value record(Record<ASTTrait>* e) {
            return nullptr;
        }

        return_value error(ErrorNode<ASTTrait>* e) {
            return nullptr;
        }
    };

    RenderTypePtr convert_type(ASTTypePtr expr) {
        return TypeConverterAST2Render::run(expr);
    }

    class ConverterAST2Render : public StaticVisitor<ConverterAST2Render, ASTTrait, generic::Root<Expression<RenderTrait>>> {
    public:
        static return_value run(Expression<ASTTrait>* expr) {
            ConverterAST2Render converter;
            auto cv = converter.traverse(expr);
            log_trace();
            return cv;
        }

        return_value convert(Expression<ASTTrait>* expr) {
            log_trace();
            return traverse(expr);
        }

        return_value function(Function<ASTTrait>* f) {
            log_trace();
            RenderExpressionPtr r = Builder<RenderTrait>::function(convert_stdstring(f->name), convert(f->body), convert_type(f->type));

            Function<RenderTrait>* ff = static_cast<Function<RenderTrait>*>(r.get());
            for (const ASTTrait::StringType& str : f->args) {
                ff->add_arg(convert_stdstring(str));
            }

            log_trace();
            return r;
        }

        return_value function_call(FunctionCall<ASTTrait>* x) {
            const Array<Expression<ASTTrait>*>& s = x->args;
            Array<Expression<RenderTrait>*> v(s.size());

            for (std::size_t i = 0; i < s.size(); ++i) {
                v[i] = this->convert(s[i]);
            }
            log_trace();
            return Builder<RenderTrait>::call(convert_stdstring(x->name), v);
        }

        return_value binary_call(BinaryCall<ASTTrait>* x) {
            log_trace();
            auto r = Builder<RenderTrait>::binary_call(x->name, convert(x->lhs), convert(x->rhs));
            log_trace();
            return r;
        }

        return_value unary_call(UnaryCall<ASTTrait>* x) {
            log_trace();
            return Builder<RenderTrait>::unary_call(x->name, convert(x->expr));
        }

        return_value borrow(Borrow<ASTTrait>* b) {
            log_trace();
            return Builder<RenderTrait>::borrow(convert(b->expr));
        }

        return_value value(Value<ASTTrait>* x) {
            log_trace();
            return Builder<RenderTrait>::value(x->as<f64>());
        }

        return_value placeholder(Placeholder<ASTTrait>* x) {
            log_trace();
            return Builder<RenderTrait>::placeholder(x->name);
        }

        return_value error(ErrorNode<ASTTrait>* e) {
            log_trace();
            return Builder<RenderTrait>::error(e->message);
        }
    };

    RenderExpressionPtr convert(ASTExpressionPtr expr) {
        return ConverterAST2Render::run(expr);
    }
}