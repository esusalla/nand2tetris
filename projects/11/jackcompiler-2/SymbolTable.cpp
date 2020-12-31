#include "SymbolTable.hpp"

#include <utility>

void SymbolTable::define(const std::string& name, const std::string& type, Kind kind)
{
    int idx = var_count_(kind);

    if (kind == Kind::STATIC || kind == Kind::FIELD) {
        class_.emplace(std::piecewise_construct, std::forward_as_tuple(name), std::forward_as_tuple(kind, type, idx));

    } else if (kind == Kind::ARG || kind == Kind::VAR) {
        subroutine_.emplace(std::piecewise_construct, std::forward_as_tuple(name),
                            std::forward_as_tuple(kind, type, idx));
    }
}

Kind SymbolTable::kind_of(const std::string& name) const
{
    if (subroutine_.find(name) != subroutine_.end()) return subroutine_.at(name).kind;
    if (class_.find(name) != class_.end()) return class_.at(name).kind;
    return Kind::NONE;
}

std::string SymbolTable::type_of(const std::string& name) const
{
    if (subroutine_.find(name) != subroutine_.end()) return subroutine_.at(name).type;
    if (class_.find(name) != class_.end()) return class_.at(name).type;
    return "";
}

int SymbolTable::index_of(const std::string& name) const
{
    if (subroutine_.find(name) != subroutine_.end()) return subroutine_.at(name).idx;
    if (class_.find(name) != class_.end()) return class_.at(name).idx;
    return -1;
}

int SymbolTable::var_count_(Kind kind) const
{
    int count = 0;

    if (kind == Kind::STATIC || kind == Kind::FIELD) {
        for (const auto& pair : class_) {
            if (pair.second.kind == kind) ++count;
        }

    } else if (kind == Kind::ARG || kind == Kind::VAR) {
        for (const auto& pair : subroutine_) {
            if (pair.second.kind == kind) ++count;
        }
    }

    return count;
}