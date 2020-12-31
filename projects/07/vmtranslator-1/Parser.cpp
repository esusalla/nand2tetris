#include "Parser.hpp"

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>

const std::vector<std::string> Parser::operators_{"add", "and", "eq", "gt", "lt", "neg", "not", "or", "sub"};

Parser::Parser(std::ifstream& infile)
{
    std::string line;

    infile >> std::ws;
    while (std::getline(infile, line)) {
        if (line.find("//") != 0) {
            infile_ << line.substr(0, line.find("//")) << '\n';
        }
        infile >> std::ws;
    }

    infile.close();
}

void Parser::advance()
{
    std::string line;
    std::getline(infile_, line);
    infile_ >> std::ws;

    std::string arg;
    std::stringstream command(line);

    command >> arg;
    if (std::find(operators_.begin(), operators_.end(), arg) != operators_.end()) {
        command_type_ = CommandType::C_ARITHMETIC;

        arg1_ = arg;
    } else if (arg == "push") {
        command_type_ = CommandType::C_PUSH;

        command >> arg1_;

        command >> arg;
        arg2_ = std::stoi(arg);
    } else if (arg == "pop") {
        command_type_ = CommandType::C_POP;

        command >> arg1_;

        command >> arg;
        arg2_ = std::stoi(arg);
    }
}