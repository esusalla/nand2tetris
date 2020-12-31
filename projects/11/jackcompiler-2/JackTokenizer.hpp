#ifndef JACK_TOKENIZER_HPP
#define JACK_TOKENIZER_HPP

#include <fstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

enum class TokenType { KEYWORD, SYMBOL, IDENTIFIER, INT_CONST, STRING_CONST };
enum class Keyword {
    CLASS,
    METHOD,
    FUNCTION,
    CONSTRUCTOR,
    INT,
    BOOLEAN,
    CHAR,
    VOID,
    VAR,
    STATIC,
    FIELD,
    LET,
    DO,
    IF,
    ELSE,
    WHILE,
    RETURN,
    TRUE,
    FALSE,
    NONE,  // using NONE instead of NULL to avoid issues with C++ NULL
    THIS
};

class JackTokenizer
{
public:
    JackTokenizer(std::ifstream& infile);

    bool has_more_tokens() const { return idx_ < static_cast<int>(tokens_.size()) - 1; }
    void advance() { ++idx_; }

    TokenType token_type() const;

    Keyword keyword() const { return string_to_keyword_.at(tokens_[idx_]); }
    char symbol() const { return tokens_[idx_][0]; }
    std::string identifier() const { return tokens_[idx_]; }
    int int_val() const { return std::stoi(tokens_[idx_]); }
    std::string string_val() const { return tokens_[idx_].substr(1, tokens_[idx_].length() - 2); }

private:
    static const std::unordered_map<std::string, Keyword> string_to_keyword_;
    static const std::unordered_set<std::string> keywords_;
    static const std::unordered_set<std::string> symbols_;

    void tokenize_group_(std::string& group, std::ifstream& infile);

    int idx_;
    std::vector<std::string> tokens_;
};

#endif
