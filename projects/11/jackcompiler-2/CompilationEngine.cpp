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
const std::unordered_map<Kind, Segment> CompilationEngine::kind_to_segment_ = {{Kind::STATIC, Segment::STATIC},
                                                                               {Kind::FIELD, Segment::THIS},
                                                                               {Kind::ARG, Segment::ARG},
                                                                               {Kind::VAR, Segment::LOCAL}};
const std::unordered_map<char, Command> CompilationEngine::op_to_command_ = {
    {'+', Command::ADD}, {'-', Command::SUB}, {'&', Command::AND}, {'|', Command::OR},
    {'<', Command::LT},  {'>', Command::GT},  {'=', Command::EQ}};
const std::unordered_set<char> CompilationEngine::ops_binary_ = {'+', '-', '*', '/', '&', '|', '<', '>', '='};
const std::unordered_set<char> CompilationEngine::ops_unary_ = {'-', '~'};

void CompilationEngine::compile_class()
{
    tokenizer_.advance();
    tokenizer_.advance();  // keyword: "class"

    set_class_name_(tokenizer_.identifier());
    tokenizer_.advance();  // identifier: class name

    tokenizer_.advance();  // symbol: "{"

    int class_size = 0;

    while (tokenizer_.has_more_tokens() && tokenizer_.token_type() == TokenType::KEYWORD &&
           (tokenizer_.keyword() == Keyword::STATIC || tokenizer_.keyword() == Keyword::FIELD)) {
        class_size += compile_class_var_dec_();
    }

    set_class_size_(class_size);

    while (tokenizer_.has_more_tokens() && tokenizer_.token_type() == TokenType::KEYWORD &&
           (tokenizer_.keyword() == Keyword::CONSTRUCTOR || tokenizer_.keyword() == Keyword::FUNCTION ||
            tokenizer_.keyword() == Keyword::METHOD)) {
        compile_subroutine_();
    }

    tokenizer_.advance();  // symbol: "}"

    writer_.close();
}

int CompilationEngine::compile_class_var_dec_()
{
    Kind kind = tokenizer_.keyword() == Keyword::STATIC ? Kind::STATIC : Kind::FIELD;
    tokenizer_.advance();

    int var_count = kind == Kind::STATIC ? 0 : 1;  // Don't count static variable towards class size

    std::string type = get_type_();
    tokenizer_.advance();

    std::string name = tokenizer_.identifier();
    tokenizer_.advance();

    symbols_.define(name, type, kind);

    while (tokenizer_.symbol() == ',') {
        tokenizer_.advance();  // ","

        name = tokenizer_.identifier();
        tokenizer_.advance();

        symbols_.define(name, type, kind);

        if (kind == Kind::FIELD) ++var_count;  // Don't count static variables towards class size
    }

    tokenizer_.advance();  // ";"

    return var_count;
}

void CompilationEngine::compile_subroutine_()
{
    symbols_.start_subroutine();
    reset_counts_();

    Keyword keyword = tokenizer_.keyword();
    tokenizer_.advance();  // "constructor", "function", or "method"

    tokenizer_.advance();  // "void", "int", "char", "boolean", or class name

    std::string name = get_class_name_() + '.' + tokenizer_.identifier();
    tokenizer_.advance();  // subroutine name

    tokenizer_.advance();  // "("

    compile_parameter_list_(keyword == Keyword::METHOD ? true : false);

    tokenizer_.advance();  // ")"
    tokenizer_.advance();  // "{"

    int n_locals = 0;
    while (tokenizer_.token_type() == TokenType::KEYWORD && tokenizer_.keyword() == Keyword::VAR) {
        n_locals += compile_var_dec_();
    }

    writer_.write_function(name, n_locals);

    if (keyword == Keyword::CONSTRUCTOR) {
        // Allocate memory for object if constructor
        writer_.write_push(Segment::CONST, get_class_size_());
        writer_.write_call("Memory.alloc", 1);
        writer_.write_pop(Segment::POINTER, 0);
    } else if (keyword == Keyword::METHOD) {
        // Move reference to object (first argument) into THIS if method
        writer_.write_push(Segment::ARG, 0);
        writer_.write_pop(Segment::POINTER, 0);
    }

    compile_statements_();

    tokenizer_.advance();  // "}"
}

void CompilationEngine::compile_parameter_list_(bool is_method)
{
    // If method, insert a placeholder value for the passed object
    // reference in order to properly offset declared arguments
    if (is_method) symbols_.define("", "", Kind::ARG);

    if (tokenizer_.token_type() != TokenType::SYMBOL) {
        std::string type = get_type_();
        tokenizer_.advance();  // "int", "char", "boolean", or class name

        std::string name = tokenizer_.identifier();
        tokenizer_.advance();  // variable name

        symbols_.define(name, type, Kind::ARG);

        while (tokenizer_.symbol() == ',') {
            tokenizer_.advance();  // ","

            type = get_type_();
            tokenizer_.advance();  // "int", "char", "boolean", or class name

            name = tokenizer_.identifier();
            tokenizer_.advance();  // variable name

            symbols_.define(name, type, Kind::ARG);
        }
    }
}

int CompilationEngine::compile_var_dec_()
{
    int n_locals = 1;

    tokenizer_.advance();  // "var"

    std::string type = get_type_();
    tokenizer_.advance();  // "int", "char", "boolean", or class name

    std::string name = tokenizer_.identifier();
    tokenizer_.advance();  // variable name

    symbols_.define(name, type, Kind::VAR);

    while (tokenizer_.symbol() == ',') {
        tokenizer_.advance();  // ","

        name = tokenizer_.identifier();
        tokenizer_.advance();  // variable name

        symbols_.define(name, type, Kind::VAR);

        ++n_locals;
    }

    tokenizer_.advance();  // ";"

    return n_locals;
}

void CompilationEngine::compile_statements_()
{
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
}

void CompilationEngine::compile_let_()
{
    tokenizer_.advance();  // "let"

    std::string name = tokenizer_.identifier();
    tokenizer_.advance();  // variable name

    Segment seg = kind_to_segment_.at(symbols_.kind_of(name));
    int idx = symbols_.index_of(name);

    // Array indexing
    if (tokenizer_.symbol() == '[') {
        tokenizer_.advance();  // "["

        compile_expression_();  // Compute and push array index onto stack

        tokenizer_.advance();  // "]"

        writer_.write_push(seg, idx);            // Push array base address onto stack
        writer_.write_arithmetic(Command::ADD);  // Offset array base address by index and leave on stack

        tokenizer_.advance();  // "="

        compile_expression_();  // Computer and push result to be assigned onto stack

        writer_.write_pop(Segment::TEMP, 0);     // Temporarily store result to be assigned
        writer_.write_pop(Segment::POINTER, 1);  // Set THAT segment to previously calculated address in array
        writer_.write_push(Segment::TEMP, 0);    // Move result to be assigned back onto stack
        writer_.write_pop(Segment::THAT, 0);     // Move result to be assigned into variable
    } else {
        tokenizer_.advance();  // "="

        compile_expression_();  // Compute and push result to be assigned onto stack

        writer_.write_pop(seg, idx);  // Move result to be assigned into variable
    }

    tokenizer_.advance();  // ";"
}

void CompilationEngine::compile_if_()
{
    tokenizer_.advance();  // "if"
    tokenizer_.advance();  // "("

    compile_expression_();  // Compute and push conditional result onto stack

    tokenizer_.advance();  // ")
    tokenizer_.advance();  // "{"

    std::string label_id = get_if_count_();

    writer_.write_if("IF_TRUE" + label_id);
    writer_.write_goto("IF_FALSE" + label_id);
    writer_.write_label("IF_TRUE" + label_id);

    compile_statements_();

    tokenizer_.advance();  // "}"

    // Else clause
    if (tokenizer_.token_type() == TokenType::KEYWORD && tokenizer_.keyword() == Keyword::ELSE) {
        writer_.write_goto("IF_END" + label_id);
        writer_.write_label("IF_FALSE" + label_id);

        tokenizer_.advance();  // "else"
        tokenizer_.advance();  // "{"

        compile_statements_();

        tokenizer_.advance();  // "}"

        writer_.write_label("IF_END" + label_id);
    } else {
        writer_.write_label("IF_FALSE" + label_id);
    }
}

void CompilationEngine::compile_while_()
{
    std::string label_id = get_while_count_();

    writer_.write_label("WHILE_EXP" + label_id);

    tokenizer_.advance();  // "while"
    tokenizer_.advance();  // "("

    compile_expression_();

    writer_.write_arithmetic(Command::NOT);
    writer_.write_if("WHILE_END" + label_id);

    tokenizer_.advance();  // ")""
    tokenizer_.advance();  // "{""

    compile_statements_();

    tokenizer_.advance();  // "}""

    writer_.write_goto("WHILE_EXP" + label_id);
    writer_.write_label("WHILE_END" + label_id);
}

void CompilationEngine::compile_do_()
{
    int n_args = 0;

    tokenizer_.advance();  // "do"

    std::string name = tokenizer_.identifier();
    tokenizer_.advance();  // subroutine name, class name, or variable name

    /* Constructors and functions are called on classes rather than objects. If the name
    before the '.' is a variable, then it can be determined the function is a method since
    it's not directly being called on the class. If only the subroutine name is provided,
    then it can be also be determined that the function is a method.
    */

    // Check if calling subroutine on class or object
    if (tokenizer_.symbol() == '.') {
        tokenizer_.advance();  // "."

        // If object, replace variable name with type (class name) and setup method call
        if (symbols_.kind_of(name) != Kind::NONE) {
            Segment seg = kind_to_segment_.at(symbols_.kind_of(name));
            int idx = symbols_.index_of(name);
            name = symbols_.type_of(name);

            // Method call setup, adjust index based on if method is called and reference to object is first argument
            writer_.write_push(seg, idx);

            ++n_args;  // Increase number of arguments to account for passing of object reference
        }

        name += '.' + tokenizer_.identifier();
        tokenizer_.advance();  // subroutine name
    } else {
        // Only subroutine name provided, so add type (class name) and setup method call
        name = get_class_name_() + '.' + name;

        // Method call setup
        writer_.write_push(Segment::POINTER, 0);

        ++n_args;  // Increase number of arguments to account for passing of object reference
    }

    tokenizer_.advance();  // "("

    n_args += compile_expression_list_();

    tokenizer_.advance();  // ")"

    writer_.write_call(name, n_args);
    writer_.write_pop(Segment::TEMP, 0);

    tokenizer_.advance();  // ";""
}

void CompilationEngine::compile_return_()
{
    tokenizer_.advance();  // "return"

    if (tokenizer_.token_type() != TokenType::SYMBOL || tokenizer_.symbol() != ';') {
        compile_expression_();
    } else {
        writer_.write_push(Segment::CONST, 0);  // assumes "void" function when nothing is returned
    }

    writer_.write_return();

    tokenizer_.advance();  // ";"
}

void CompilationEngine::compile_expression_()
{
    compile_term_();

    while (tokenizer_.token_type() == TokenType::SYMBOL && ops_binary_.find(tokenizer_.symbol()) != ops_binary_.end()) {
        char op = tokenizer_.symbol();
        tokenizer_.advance();  // binary operator

        compile_term_();

        if (op == '*') {
            writer_.write_call("Math.multiply", 2);  // Multiplication handled by OS
        } else if (op == '/') {
            writer_.write_call("Math.divide", 2);  // Division handled by OS
        } else {
            writer_.write_arithmetic(op_to_command_.at(op));
        }
    }
}

void CompilationEngine::compile_term_()
{
    if (tokenizer_.token_type() == TokenType::INT_CONST) {
        writer_.write_push(Segment::CONST, tokenizer_.int_val());
        tokenizer_.advance();  // integer constant
    } else if (tokenizer_.token_type() == TokenType::STRING_CONST) {
        std::string str_const = tokenizer_.string_val();
        tokenizer_.advance();  // string constant

        writer_.write_push(Segment::CONST, str_const.length());
        writer_.write_call("String.new", 1);
        for (const char& c : str_const) {
            writer_.write_push(Segment::CONST, c);
            writer_.write_call("String.appendChar", 2);
        }
    } else if (tokenizer_.token_type() == TokenType::KEYWORD) {
        Keyword keyword = tokenizer_.keyword();
        tokenizer_.advance();  // "true", "false", "null", or "this"

        if (keyword == Keyword::TRUE) {
            writer_.write_push(Segment::CONST, 0);
            writer_.write_arithmetic(Command::NOT);
        } else if (keyword == Keyword::FALSE || keyword == Keyword::NONE) {
            writer_.write_push(Segment::CONST, 0);
        } else if (keyword == Keyword::THIS) {
            writer_.write_push(Segment::POINTER, 0);
        }
    } else if (tokenizer_.token_type() == TokenType::IDENTIFIER) {
        std::string name = tokenizer_.identifier();
        tokenizer_.advance();  // subroutine name, class name, or variable name

        if (tokenizer_.token_type() == TokenType::SYMBOL) {
            if (tokenizer_.symbol() == '[') {
                // Array indexing
                Segment seg = kind_to_segment_.at(symbols_.kind_of(name));
                int idx = symbols_.index_of(name);

                tokenizer_.advance();  // "["

                compile_expression_();  // Compute and push array index onto stack

                tokenizer_.advance();  // "]"

                writer_.write_push(seg, idx);            // Push array base address onto stack
                writer_.write_arithmetic(Command::ADD);  // Offset array base address by index and leave on stack
                writer_.write_pop(Segment::POINTER, 1);  // Set THAT segment to calculated address in array
                writer_.write_push(Segment::THAT, 0);    // Push value at array index onto stack
            } else if (tokenizer_.symbol() == '.' || tokenizer_.symbol() == '(') {
                // Check if calling subroutine on class or object
                int n_args = 0;

                if (tokenizer_.symbol() == '.') {
                    tokenizer_.advance();  // "."

                    // If object, replace variable name with type (class name) and setup method call
                    if (symbols_.kind_of(name) != Kind::NONE) {
                        Segment seg = kind_to_segment_.at(symbols_.kind_of(name));
                        int idx = symbols_.index_of(name);
                        name = symbols_.type_of(name);

                        // Method call setup, adjust index based on if method is called and reference to object is first
                        // argument
                        writer_.write_push(seg, idx);

                        ++n_args;  // Increase number of arguments to account for passing of object reference
                    }

                    name += '.' + tokenizer_.identifier();
                    tokenizer_.advance();  // subroutine name
                } else {
                    // Only subroutine name provided, so add type (class name) and setup method call
                    name = get_class_name_() + '.' + name;

                    // Method call setup
                    writer_.write_push(Segment::POINTER, 0);

                    ++n_args;  // Increase number of arguments to account for passing of object reference
                }

                tokenizer_.advance();  // "("

                n_args += compile_expression_list_();

                tokenizer_.advance();  // ")"

                writer_.write_call(name, n_args);
            } else {
                // Variable name only
                Segment seg = kind_to_segment_.at(symbols_.kind_of(name));
                int idx = symbols_.index_of(name);
                writer_.write_push(seg, idx);  // Push variable onto stack
            }
        }
    } else if (tokenizer_.token_type() == TokenType::SYMBOL) {
        if (tokenizer_.symbol() == '(') {
            tokenizer_.advance();  // "("

            compile_expression_();

            tokenizer_.advance();  // ")"
        } else if (ops_unary_.find(tokenizer_.symbol()) != ops_unary_.end()) {
            char op = tokenizer_.symbol();
            tokenizer_.advance();  // unary operator

            compile_term_();

            writer_.write_arithmetic(op == '-' ? Command::NEG : Command::NOT);
        }
    }
}

int CompilationEngine::compile_expression_list_()
{
    int n_args = 0;

    if (tokenizer_.token_type() != TokenType::SYMBOL || tokenizer_.symbol() != ')') {
        compile_expression_();

        ++n_args;
        while (tokenizer_.token_type() == TokenType::SYMBOL && tokenizer_.symbol() == ',') {
            tokenizer_.advance();  // ","

            compile_expression_();

            ++n_args;
        }
    }

    return n_args;
}

void CompilationEngine::reset_counts_()
{
    if_count_ = 0;
    while_count_ = 0;
}

std::string CompilationEngine::get_type_()
{
    if (tokenizer_.token_type() == TokenType::KEYWORD) return keyword_to_string_.at(tokenizer_.keyword());
    return tokenizer_.identifier();
}