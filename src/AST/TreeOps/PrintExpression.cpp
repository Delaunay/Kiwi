#include "PrintExpression.h"

namespace kiwi {

std::ostream &PrintType::builtin_type(BuiltinType *type, std::ostream &out, u64 depth) {
    out << type->name;
    return out;
}
std::ostream &PrintType::function_type(FunctionType *type, std::ostream &out, u64 depth) {
    out << "(";
    for(Type *item : type->args) {
        visit_type(item, out, depth);
        out << ", ";
    }
    out << ") -> ";
    visit_type(type->return_type, out, depth);
    return out;
}
std::ostream &PrintType::union_type(UnionType *type, std::ostream &out, u64 depth) {
    return out << type->definition->name;
}
std::ostream &PrintType::struct_type(StructType *type, std::ostream &out, u64 depth) {
    return out << type->definition->name;
}

std::ostream &PrintType::unhandled_type(Type *type, std::ostream &out, u64 depth) {
    log_info("");
    return out << to_string(type->tag);
}

std::ostream &PrintExpression::unary_call(UnaryCall *call, std::ostream &out, u64 depth) {
    out << "(";
    visit_expression(call->fun, out, depth);
    out << " ";

    for(u64 i = 0; i < call->args_size(); ++i) {
        visit_expression(call->arg(i), out, depth);
    }

    out << ")";
    return out;
}
std::ostream &PrintExpression::binary_call(BinaryCall *call, std::ostream &out, u64 depth) {
    out << "(";
    visit_expression(call->fun, out, depth);
    out << " ";

    for(u64 i = 0; i < call->args_size(); ++i) {
        visit_expression(call->arg(i), out, depth);
    }

    out << ")";
    return out;
}
std::ostream &PrintExpression::function_call(FunctionCall *call, std::ostream &out, u64 depth) {
    out << "(";
    visit_expression(call->fun, out, depth);
    out << " ";

    for(u64 i = 0; i < call->args_size(); ++i) {
        visit_expression(call->arg(i), out, depth);
    }

    out << ")";
    return out;
}
std::ostream &PrintExpression::match(Match *m, std::ostream &out, u64 depth) {
    visit_expression(m->target, out, depth);
    out << "match\n";

    for(auto &item : m->branches) {
        out << std::string((depth + 1) * 4, ' ');
        out << "case ";
        visit_expression(std::get<0>(item), out, depth);
        out << " => ";
        visit_expression(std::get<1>(item), out, depth);
        out << "\n";
    }

    return out;
}

std::ostream &PrintExpression::block(Block *b, std::ostream &out, u64 depth) {
    log_info("");

    for(auto &item : b->statements) {
        out << std::string(depth * 4, ' ');
        PrintStatement().visit_statement(item, out, depth);
        out << "\n";
    }
    return out;
}
std::ostream &PrintExpression::placeholder(Placeholder *p, std::ostream &out, u64 depth) {
    out << p->name;

    if(p->type) {
        out << ": ";
        visit_expression(p->type, out, depth);
    }
    return out;
}
std::ostream &PrintExpression::value(Value *, std::ostream &out, u64 depth) { return out; }

std::ostream &PrintExpression::unhandled_expression(Expression *expr, std::ostream &out,
                                                    u64 depth) {
    log_info("");
    return out << to_string(expr->tag);
}

std::ostream &PrintDefinition::function_def(FunctionDefinition *def, std::ostream &out, u64 depth) {
    out << std::string(depth * 4, ' ');
    out << "def " << def->name << "(";

    for(auto &item : def->args)
        out << item << "";

    out << "):\n";
    PrintExpression().visit_expression(def->body, out, depth + 1);
    return out;
}
std::ostream &PrintDefinition::macro_def(MacroDefinition *def, std::ostream &out, u64 depth) {
    out << std::string(depth * 4, ' ');
    out << "macro " << def->name << "(";

    for(auto &item : def->args)
        out << item << "";

    out << "):\n";
    PrintExpression().visit_expression(def->body, out, depth + 1);
    return out;
}

std::ostream &PrintDefinition::struct_def(StructDefinition *def, std::ostream &out, u64 depth) {
    out << std::string(depth * 4, ' ');
    out << "struct " << def->name << "(";

    u64 size = def->meta_types.size();

    for(u64 i = 0; i < size - 1; ++i) {
        auto &item = def->meta_types[i];
        out << std::get<0>(item) << ": ";
        PrintStatement().visit_statement(std::get<1>(item), out, depth + 1);
        out << ", ";
    }
    auto &item = def->meta_types[size - 1];
    out << std::get<0>(item) << ": ";
    PrintStatement().visit_statement(std::get<1>(item), out, depth + 1);

    out << "):\n";

    for(auto &item : def->attributes) {
        out << std::string((depth + 1) * 4, ' ') << std::get<0>(item) << ": ";
        PrintStatement().visit_statement(std::get<1>(item), out, depth + 1);
        out << "\n";
    }

    return out;
}
std::ostream &PrintDefinition::union_def(UnionDefinition *def, std::ostream &out, u64 depth) {
    out << std::string(depth * 4, ' ');
    out << "union " << def->name << "(";

    u64 size = def->meta_types.size();

    for(u64 i = 0; i < size - 1; ++i) {
        auto &item = def->meta_types[i];
        out << std::get<0>(item) << ": ";
        PrintStatement().visit_statement(std::get<1>(item), out, depth + 1);
        out << ", ";
    }
    auto &item = def->meta_types[size - 1];
    out << std::get<0>(item) << ": ";
    PrintStatement().visit_statement(std::get<1>(item), out, depth + 1);

    out << "):\n";

    for(auto &item : def->attributes) {
        out << std::string((depth + 1) * 4, ' ') << std::get<0>(item) << ": ";
        PrintStatement().visit_statement(std::get<1>(item), out, depth + 1);
        out << "\n";
    }

    return out;
}

std::ostream &PrintDefinition::unhandled_definition(Definition *def, std::ostream &out, u64 depth) {
    log_info("");
    return out << to_string(def->tag);
}

std::ostream &PrintType::nullptr_type(std::ostream &out, u64 depth) {
    return out << "NoneType(nullptr)";
}

std::ostream &PrintDefinition::nullptr_definition(std::ostream &out, u64 depth) {
    return out << "NoneDefinition(nullptr)";
}

std::ostream &PrintExpression::nullptr_expression(std::ostream &out, u64 depth) {
    return out << "NoneExpression(nullptr)";
}

} // namespace kiwi
