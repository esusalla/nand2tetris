#include <algorithm>
#include <bitset>
#include <cstdlib>
#include <fstream>
#include <iostream>

#include "Code.hpp"
#include "Parser.hpp"
#include "SymbolTable.hpp"

void first_pass(Parser& parser, SymbolTable& symbol_table);
void second_pass(Parser& parser, SymbolTable& symbol_table, const Code& code, const std::string& filename);

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "Missing input" << std::endl;
        return 1;
    }

    std::string filename(argv[1]);
    std::ifstream infile(filename);
    if (!infile) {
        std::cerr << "Cannot open input file: " << filename << std::endl;
        return 1;
    }

    Parser parser(infile);
    SymbolTable symbol_table;
    Code code;

    first_pass(parser, symbol_table);
    second_pass(parser, symbol_table, code, filename);

    return 0;
}

void first_pass(Parser& parser, SymbolTable& symbol_table)
{
    int rom = 0;

    while (parser.has_more_commands()) {
        parser.advance();
        if (parser.command_type() == CommandType::A_COMMAND || parser.command_type() == CommandType::C_COMMAND) {
            ++rom;
        } else {
            symbol_table.add_entry(parser.symbol(), rom);
        }
    }

    parser.reset();
}

void second_pass(Parser& parser, SymbolTable& symbol_table, const Code& code, const std::string& filename)
{
    int ram = 16;
    std::string outpath = filename.substr(0, filename.find_last_of(".asm") - 3) + ".hack";
    std::ofstream outfile(outpath);

    while (parser.has_more_commands()) {
        parser.advance();
        std::string output = "";

        if (parser.command_type() == CommandType::A_COMMAND) {
            auto symbol = parser.symbol();
            if (std::all_of(symbol.begin(), symbol.end(), [](char c) { return std::isdigit(c); })) {
                output = std::bitset<16>(std::stoi(symbol)).to_string();
            } else {
                if (!symbol_table.contains(symbol)) symbol_table.add_entry(symbol, ram++);
                output = std::bitset<16>(symbol_table.get_address(symbol)).to_string();
            }
        } else if (parser.command_type() == CommandType::C_COMMAND) {
            output = "111" + code.comp(parser.comp()) + code.dest(parser.dest()) + code.jump(parser.jump());
        } else {
            continue;
        }

        outfile << output << '\n';
    }

    outfile.close();
}
