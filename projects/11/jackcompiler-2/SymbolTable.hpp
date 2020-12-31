#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <string>
#include <unordered_map>

enum class Kind { STATIC, FIELD, ARG, VAR, NONE };

class SymbolTable
{
public:
    void start_subroutine() { subroutine_.clear(); }

    void define(const std::string& name, const std::string& type, Kind kind);

    Kind kind_of(const std::string& name) const;
    std::string type_of(const std::string& name) const;
    int index_of(const std::string& name) const;

private:
    struct Symbol {
        Symbol(Kind kind, const std::string& type, int idx) : kind(kind), type(type), idx(idx) {}

        Kind kind;
        std::string type;
        int idx;
    };

    int var_count_(Kind kind) const;

    std::unordered_map<std::string, Symbol> class_;
    std::unordered_map<std::string, Symbol> subroutine_;
};

#endif