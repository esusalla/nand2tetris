#ifndef COMPILATION_ENGINE_HPP
#define COMPILATION_ENGINE_HPP

#include <fstream>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "JackTokenizer.hpp"

class CompilationEngine
{
public:
    CompilationEngine(std::ifstream& infile, std::ofstream& outfile);

    void compile_class();

private:
    static const std::unordered_map<Keyword, std::string> keyword_to_string_;
    static const std::unordered_set<char> ops_binary_;
    static const std::unordered_set<char> ops_unary_;

    // Functions defined in API spec
    void compile_class_var_dec_();
    void compile_subroutine_();
    void compile_parameter_list_();
    void compile_var_dec_();
    void compile_statements_();
    void compile_do_();
    void compile_let_();
    void compile_while_();
    void compile_return_();
    void compile_if_();
    void compile_expression_();
    void compile_term_();
    void compile_expression_list_();

    // Helpers
    void compile_type_();
    void compile_subroutine_body_();

    void output_tag_(const std::string name, bool closing = false);
    void output_keyword_();
    void output_symbol_();
    void output_identifier_();
    void output_int_const_();
    void output_str_const_();

    void inc_indent_() { indent_ += "  "; }
    void dec_indent_() { indent_ = indent_.substr(2); }

    JackTokenizer tokenizer_;
    std::ofstream& outfile_;
    std::string indent_;
};

#endif
