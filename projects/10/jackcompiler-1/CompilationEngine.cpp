#include "CompilationEngine.hpp"

#include <fstream>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "JackTokenizer.hpp"

const std::unordered_map<Keyword, std::string> CompilationEngine::keyword_to_string_ = {
    {Keyword::CLASS, "class"},
    {Keyword::METHOD, "method"},
    {Keyword::FUNCTION, "function"},
    {Keyword::CONSTRUCTOR, "constructor"},
    {Keyword::INT, "int"},
    {Keyword::BOOLEAN, "boolean"},
    {Keyword::CHAR, "char"},
    {Keyword::VOID, "void"},
    {Keyword::VAR, "var"},
    {Keyword::STATIC, "static"},
    {Keyword::FIELD, "field"},
    {Keyword::LET, "let"},
    {Keyword::DO, "do"},
    {Keyword::IF, "if"},
    {Keyword::ELSE, "else"},
    {Keyword::WHILE, "while"},
    {Keyword::RETURN, "return "},
    {Keyword::TRUE, "true"},
    {Keyword::FALSE, "false"},
    {Keyword::NONE, "null"},
    {Keyword::THIS, "this"}};
const std::unordered_set<char> CompilationEngine::ops_binary_ = {'+', '-', '*', '/', '&', '|', '<', '>', '='};
const std::unordered_set<char> CompilationEngine::ops_unary_ = {'-', '~'};

CompilationEngine::CompilationEngine(std::ifstream& infile, std::ofstream& outfile)
    : tokenizer_(infile), outfile_(outfile)
{
}

void CompilationEngine::compile_class()
{
    tokenizer_.advance();

    output_tag_("class");
    output_keyword_();     // "class"
    output_identifier_();  // class name
    output_symbol_();      // "{"

    while (tokenizer_.has_more_tokens() && tokenizer_.token_type() == TokenType::KEYWORD) {
        switch (tokenizer_.keyword()) {
            case Keyword::STATIC:
            case Keyword::FIELD: compile_class_var_dec_(); break;
            case Keyword::CONSTRUCTOR:
            case Keyword::FUNCTION:
            case Keyword::METHOD: compile_subroutine_(); break;
            default: break;
        }
    }

    output_symbol_();  // "}"
    output_tag_("class", true);
}

void CompilationEngine::compile_class_var_dec_()
{
    output_tag_("classVarDec");
    output_keyword_();  // "static" or "field"

    compile_type_();

    output_identifier_();  // variable name

    while (tokenizer_.symbol() == ',') {
        output_symbol_();      // ","
        output_identifier_();  // variable name
    }

    output_symbol_();  // ";"
    output_tag_("classVarDec", true);
}

void CompilationEngine::compile_subroutine_()
{
    output_tag_("subroutineDec");
    output_keyword_();  // "constructor", "function", or "method"

    if (tokenizer_.token_type() == TokenType::KEYWORD && tokenizer_.keyword() == Keyword::VOID) {
        output_keyword_();  // "void"
    } else {
        compile_type_();
    }

    output_identifier_();  // subroutine name
    output_symbol_();      // "("

    compile_parameter_list_();

    output_symbol_();  // ")"

    compile_subroutine_body_();

    output_tag_("subroutineDec", true);
}

void CompilationEngine::compile_parameter_list_()
{
    output_tag_("parameterList");

    if (tokenizer_.token_type() != TokenType::SYMBOL) {
        compile_type_();

        output_identifier_();  // variable name
        while (tokenizer_.symbol() == ',') {
            output_symbol_();  // ","

            compile_type_();

            output_identifier_();  // variable name
        }
    }

    output_tag_("parameterList", true);
}

void CompilationEngine::compile_var_dec_()
{
    output_tag_("varDec");
    output_keyword_();  // "var"

    compile_type_();

    output_identifier_();  // variable name

    while (tokenizer_.symbol() == ',') {
        output_symbol_();      // ","
        output_identifier_();  // variable name
    }

    output_symbol_();  // ";"
    output_tag_("varDec", true);
}

void CompilationEngine::compile_statements_()
{
    output_tag_("statements");

    while (tokenizer_.token_type() == TokenType::KEYWORD) {
        Keyword keyword = tokenizer_.keyword();

        if (keyword == Keyword::LET) {
            compile_let_();
        } else if (keyword == Keyword::IF) {
            compile_if_();
        } else if (keyword == Keyword::WHILE) {
            compile_while_();
        } else if (keyword == Keyword::DO) {
            compile_do_();
        } else if (keyword == Keyword::RETURN) {
            compile_return_();
        }
    }

    output_tag_("statements", true);
}

void CompilationEngine::compile_let_()
{
    output_tag_("letStatement");
    output_keyword_();     // "let"
    output_identifier_();  // variable name

    if (tokenizer_.symbol() == '[') {
        output_symbol_();  // "["

        compile_expression_();

        output_symbol_();  // "]"
    }

    output_symbol_();  // "="

    compile_expression_();

    output_symbol_();  // ";"
    output_tag_("letStatement", true);
}

void CompilationEngine::compile_if_()
{
    output_tag_("ifStatement");
    output_keyword_();  // "if"
    output_symbol_();   // "("

    compile_expression_();

    output_symbol_();  // ")
    output_symbol_();  // "{"

    compile_statements_();

    output_symbol_();  // "}"

    if (tokenizer_.token_type() == TokenType::KEYWORD && tokenizer_.keyword() == Keyword::ELSE) {
        output_keyword_();  // "else"
        output_symbol_();   // "{"

        compile_statements_();

        output_symbol_();  // "}"
    }

    output_tag_("ifStatement", true);
}

void CompilationEngine::compile_while_()
{
    output_tag_("whileStatement");
    output_keyword_();  // "while"
    output_symbol_();   // "("

    compile_expression_();

    output_symbol_();  // ")""
    output_symbol_();  // "{""

    compile_statements_();

    output_symbol_();  // "}""
    output_tag_("whileStatement", true);
}

void CompilationEngine::compile_do_()
{
    output_tag_("doStatement");
    output_keyword_();     // "do"
    output_identifier_();  // subroutine name, class name, or variable name

    if (tokenizer_.symbol() == '.') {
        output_symbol_();      // "."
        output_identifier_();  // subroutine name
    }

    output_symbol_();  // "("

    compile_expression_list_();

    output_symbol_();  // ")"
    output_symbol_();  // ";""
    output_tag_("doStatement", true);
}

void CompilationEngine::compile_return_()
{
    output_tag_("returnStatement");
    output_keyword_();  // "return"

    if (tokenizer_.token_type() != TokenType::SYMBOL || tokenizer_.symbol() != ';') {
        compile_expression_();
    }

    output_symbol_();  // ";"
    output_tag_("returnStatement", true);
}

void CompilationEngine::compile_expression_()
{
    output_tag_("expression");

    compile_term_();

    while (tokenizer_.token_type() == TokenType::SYMBOL && ops_binary_.find(tokenizer_.symbol()) != ops_binary_.end()) {
        output_symbol_();  // binary operator

        compile_term_();
    }

    output_tag_("expression", true);
}

void CompilationEngine::compile_term_()
{
    output_tag_("term");

    if (tokenizer_.token_type() == TokenType::INT_CONST) {
        output_int_const_();  // integer constant
    } else if (tokenizer_.token_type() == TokenType::STRING_CONST) {
        output_str_const_();  // string constant
    } else if (tokenizer_.token_type() == TokenType::KEYWORD) {
        output_keyword_();  // "true", "false", "null", or "this"
    } else if (tokenizer_.token_type() == TokenType::IDENTIFIER) {
        output_identifier_();  // subroutine name, class name, or variable name

        if (tokenizer_.token_type() == TokenType::SYMBOL) {
            if (tokenizer_.symbol() == '[') {
                output_symbol_();  // "["

                compile_expression_();

                output_symbol_();  // "]"
            } else if (tokenizer_.symbol() == '(') {
                output_symbol_();  // "("

                compile_expression_list_();

                output_symbol_();  // ")"
            } else if (tokenizer_.symbol() == '.') {
                output_symbol_();      // "."
                output_identifier_();  // subroutine name
                output_symbol_();      // "("

                compile_expression_list_();

                output_symbol_();  // ")"
            }
        }
    } else if (tokenizer_.token_type() == TokenType::SYMBOL) {
        if (tokenizer_.symbol() == '(') {
            output_symbol_();  // "("

            compile_expression_();

            output_symbol_();  // ")"
        } else if (ops_unary_.find(tokenizer_.symbol()) != ops_unary_.end()) {
            output_symbol_();  // unary operator

            compile_term_();
        }
    }

    output_tag_("term", true);
}

void CompilationEngine::compile_expression_list_()
{
    output_tag_("expressionList");

    if (tokenizer_.token_type() != TokenType::SYMBOL || tokenizer_.symbol() != ')') {
        compile_expression_();

        while (tokenizer_.token_type() == TokenType::SYMBOL && tokenizer_.symbol() == ',') {
            output_symbol_();  // ","

            compile_expression_();
        }
    }

    output_tag_("expressionList", true);
}

void CompilationEngine::compile_type_()
{
    if (tokenizer_.token_type() == TokenType::KEYWORD) {
        output_keyword_();  // "int", "char", or "boolean"
    } else if (tokenizer_.token_type() == TokenType::IDENTIFIER) {
        output_identifier_();  // class name
    }
}

void CompilationEngine::compile_subroutine_body_()
{
    output_tag_("subroutineBody");
    output_symbol_();  // "{"

    while (tokenizer_.token_type() == TokenType::KEYWORD && tokenizer_.keyword() == Keyword::VAR) {
        compile_var_dec_();
    }

    compile_statements_();

    output_symbol_();  // "}"
    output_tag_("subroutineBody", true);
}

void CompilationEngine::output_tag_(const std::string name, bool closing)
{
    if (closing) dec_indent_();
    outfile_ << indent_ << "<" << (closing ? "/" : "") << name << ">" << std::endl;
    if (!closing) inc_indent_();
}

void CompilationEngine::output_keyword_()
{
    outfile_ << indent_ << "<keyword> " << keyword_to_string_.at(tokenizer_.keyword()) << " </keyword>" << std::endl;
    tokenizer_.advance();
}

std::string encode_symbol(char symbol)
{
    if (symbol == '<') return "&lt;";
    if (symbol == '>') return "&gt;";
    if (symbol == '"') return "&quot;";
    if (symbol == '&') return "&amp;";

    return std::string(1, symbol);
}

void CompilationEngine::output_symbol_()
{
    outfile_ << indent_ << "<symbol> " << encode_symbol(tokenizer_.symbol()) << " </symbol>" << std::endl;
    tokenizer_.advance();
}

void CompilationEngine::output_identifier_()
{
    outfile_ << indent_ << "<identifier> " << tokenizer_.identifier() << " </identifier>" << std::endl;
    tokenizer_.advance();
}

void CompilationEngine::output_int_const_()
{
    outfile_ << indent_ << "<integerConstant> " << tokenizer_.int_val() << " </integerConstant>" << std::endl;
    tokenizer_.advance();
}

void CompilationEngine::output_str_const_()
{
    outfile_ << indent_ << "<stringConstant> " << tokenizer_.string_val() << " </stringConstant>" << std::endl;
    tokenizer_.advance();
}
