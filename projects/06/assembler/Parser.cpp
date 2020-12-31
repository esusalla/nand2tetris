#include "Parser.hpp"

#include <fstream>
#include <string>

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

CommandType Parser::command_type() const
{
    if (command_[0] == '@') return CommandType::A_COMMAND;
    if (command_[0] == '(') return CommandType::L_COMMAND;
    return CommandType::C_COMMAND;
}

std::string Parser::symbol() const
{
    if (command_[0] == '@') return std::string(command_.begin() + 1, command_.end());
    return std::string(command_.begin() + 1, command_.end() - 1);
}

std::string Parser::dest() const
{
    auto idx = command_.find('=');
    if (idx == std::string::npos) return "";
    return std::string(command_.begin(), command_.begin() + idx);
}

std::string Parser::comp() const
{
    auto idx1 = command_.find('=') != std::string::npos ? command_.find('=') + 1 : 0;
    auto idx2 = command_.find(';') != std::string::npos ? command_.find(';') : command_.length();
    return std::string(command_.begin() + idx1, command_.begin() + idx2);
}

std::string Parser::jump() const
{
    auto idx = command_.find(';');
    if (idx == std::string::npos) return "";
    return std::string(command_.begin() + idx + 1, command_.end());
}
