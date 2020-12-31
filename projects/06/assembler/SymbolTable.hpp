#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <string>
#include <unordered_map>

class SymbolTable
{
public:
    SymbolTable()
        : table_({{"SP", 0},   {"LCL", 1},  {"ARG", 2},  {"THIS", 3},       {"THAT", 4},   {"R0", 0},
                  {"R1", 1},   {"R2", 2},   {"R3", 3},   {"R4", 4},         {"R5", 5},     {"R6", 6},
                  {"R7", 7},   {"R8", 8},   {"R9", 9},   {"R10", 10},       {"R11", 11},   {"R12", 12},
                  {"R13", 13}, {"R14", 14}, {"R15", 15}, {"SCREEN", 16384}, {"KBD", 24576}})
    {
    }

    void add_entry(const std::string& symbol, int address) { table_.insert({symbol, address}); }
    bool contains(const std::string& symbol) const { return table_.find(symbol) != table_.end(); }
    int get_address(const std::string& symbol) const { return (*table_.find(symbol)).second; }

private:
    std::unordered_map<std::string, int> table_;
};

#endif