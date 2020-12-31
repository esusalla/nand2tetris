#include "JackTokenizer.hpp"

#include <algorithm>
#include <cstddef>
#include <fstream>
#include <regex>
#include <string>
#include <unordered_map>
#include <unordered_set>

const std::unordered_map<std::string, Keyword> JackTokenizer::string_to_keyword_ = {
    {"class", Keyword::CLASS},
    {"method", Keyword::METHOD},
    {"function", Keyword::FUNCTION},
    {"constructor", Keyword::CONSTRUCTOR},
    {"int", Keyword::INT},
    {"boolean", Keyword::BOOLEAN},
    {"char", Keyword::CHAR},
    {"void", Keyword::VOID},
    {"var", Keyword::VAR},
    {"static", Keyword::STATIC},
    {"field", Keyword::FIELD},
    {"let", Keyword::LET},
    {"do", Keyword::DO},
    {"if", Keyword::IF},
    {"else", Keyword::ELSE},
    {"while", Keyword::WHILE},
    {"return", Keyword::RETURN},
    {"true", Keyword::TRUE},
    {"false", Keyword::FALSE},
    {"null", Keyword::NONE},
    {"this", Keyword::THIS}};
const std::unordered_set<std::string> JackTokenizer::keywords_{
    "class", "method", "function", "constructor", "int",   "boolean", "char", "void",  "var",  "static", "field",
    "let",   "do",     "if",       "else",        "while", "return",  "true", "false", "null", "this"};
const std::unordered_set<std::string> JackTokenizer::symbols_{"{", "}", "(", ")", "[", "]", ".", ",", ";", "+",
                                                              "-", "*", "/", "&", "|", "<", ">", "=", "~"};

JackTokenizer::JackTokenizer(std::ifstream& infile) : idx_(-1)
{
    std::string group;

    while (infile >> group) {
        tokenize_group_(group, infile);
    }

    infile.close();
}

bool is_identifier(const std::string token)
{
    if (std::isdigit(token[0])) return false;
    return std::all_of(token.begin(), token.end(), [](char c) { return c == '_' || std::isalnum(c); });
}

TokenType JackTokenizer::token_type() const
{
    const std::string token = tokens_[idx_];

    if (keywords_.find(token) != keywords_.end()) {
        return TokenType::KEYWORD;
    } else if (token.length() == 1 && symbols_.find(token) != symbols_.end()) {
        return TokenType::SYMBOL;
    } else if (is_identifier(token)) {
        return TokenType::IDENTIFIER;
    } else if (std::all_of(token.begin(), token.end(), ::isdigit)) {
        return TokenType::INT_CONST;
    } else {
        return TokenType::STRING_CONST;
    }
}

void JackTokenizer::tokenize_group_(std::string& group, std::ifstream& infile)
{
    std::string line;
    std::string token;
    std::regex symbols("\\{|\\}|\\(|\\)|\\[|\\]|\\.|\\,|\\;|\\+|\\-|\\*|\\/|\\&|\\|\\<|\\>|\\=|\\~");

    // Skip single line comments
    if (group.rfind("//", 0) == 0) {
        std::getline(infile, line);
        return;
    }

    // Skip multi-line comments
    if (group.rfind("/*", 0) == 0) {
        while (group.find("*/") == std::string::npos) {
            infile >> std::ws >> group;
        }
        group = group.substr(group.find("*/") + 2);
    }

    // Handle string constants
    if (group.find('"') != std::string::npos) {
        std::getline(infile, line);
        group = group + line;

        const int idx1 = group.find('"');
        const int idx2 = group.find('"', idx1 + 1) + 1;

        std::string front(group.begin(), group.begin() + idx1);
        std::string str_const(group.begin() + idx1, group.begin() + idx2);
        std::string back(group.begin() + idx2, group.end());

        // Tokenize both sides around string constant
        tokenize_group_(front, infile);
        tokens_.push_back(str_const);
        tokenize_group_(back, infile);
        return;
    }

    std::stringstream tmp(std::regex_replace(group, symbols, " $0 "));
    while (tmp >> token) {
        tokens_.push_back(token);
    }
}