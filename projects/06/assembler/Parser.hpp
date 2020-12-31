#ifndef PARSER_HPP
#define PARSER_HPP

#include <fstream>
#include <sstream>

enum class CommandType { A_COMMAND, C_COMMAND, L_COMMAND };

class Parser
{
public:
    Parser(std::ifstream& infile);

    bool has_more_commands() const { return !infile_.eof(); }
    void advance() { infile_ >> command_ >> std::ws; }
    void reset() { infile_.seekg(0); }

    CommandType command_type() const;
    std::string symbol() const;
    std::string dest() const;
    std::string comp() const;
    std::string jump() const;

private:
    std::stringstream infile_;
    std::string command_;
};

#endif