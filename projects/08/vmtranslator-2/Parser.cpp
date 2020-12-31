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
    } else if (arg == "label") {
        command_type_ = CommandType::C_LABEL;
        command >> arg1_;
    } else if (arg == "goto") {
        command_type_ = CommandType::C_GOTO;
        command >> arg1_;
    } else if (arg == "if-goto") {
        command_type_ = CommandType::C_IF;
        command >> arg1_;
    } else if (arg == "function") {
        command_type_ = CommandType::C_FUNCTION;
        command >> arg1_;
        command >> arg;
        arg2_ = std::stoi(arg);
    } else if (arg == "return") {
        command_type_ = CommandType::C_RETURN;
    } else if (arg == "call") {
        command_type_ = CommandType::C_CALL;
        command >> arg1_;
        command >> arg;
        arg2_ = std::stoi(arg);
    }
}