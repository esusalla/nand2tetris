#ifndef PARSER_HPP
#define PARSER_HPP

#include <fstream>
#include <sstream>
#include <vector>

enum class CommandType { C_ARITHMETIC, C_PUSH, C_POP, C_LABEL, C_GOTO, C_IF, C_FUNCTION, C_RETURN, C_CALL };

class Parser
{
public:
    Parser(std::ifstream& infile);

    bool has_more_commands() const { return !infile_.eof(); }
    void advance();
    void reset() { infile_.seekg(0); }

    CommandType command_type() const { return command_type_; }
    std::string arg1() const { return arg1_; }
    int arg2() const { return arg2_; }

private:
    static const std::vector<std::string> operators_;

    std::stringstream infile_;
    CommandType command_type_;
    std::string arg1_;
    int arg2_;
};

#endif