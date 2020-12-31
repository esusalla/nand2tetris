#ifndef COMPILATION_ENGINE_HPP
#define COMPILATION_ENGINE_HPP

#include <fstream>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "JackTokenizer.hpp"
#include "SymbolTable.hpp"
#include "VMWriter.hpp"

class CompilationEngine
{
public:
    CompilationEngine(std::ifstream& infile, std::ofstream& outfile)
        : tokenizer_(infile), writer_(outfile), class_size_(0), if_count_(0), while_count_(0)
    {
    }

    void compile_class();

private:
    static const std::unordered_map<Keyword, std::string> keyword_to_string_;
    static const std::unordered_map<Kind, Segment> kind_to_segment_;
    static const std::unordered_map<char, Command> op_to_command_;
    static const std::unordered_set<char> ops_binary_;
    static const std::unordered_set<char> ops_unary_;

    // Functions defined in API spec
    int compile_class_var_dec_();  // Returns number of class variable declarations
    void compile_subroutine_();
    void compile_parameter_list_(bool is_method);  // Adjusts starting index for arguments if function is a method
    int compile_var_dec_();                        // Returns the number of local variable declarations
    void compile_statements_();
    void compile_do_();
    void compile_let_();
    void compile_while_();
    void compile_return_();
    void compile_if_();
    void compile_expression_();
    void compile_term_();
    int compile_expression_list_();  // Returns the number of function arguments

    // Helpers
    std::string get_if_count_() { return std::to_string(if_count_++); }
    std::string get_while_count_() { return std::to_string(while_count_++); }
    void reset_counts_();

    std::string get_type_();

    std::string get_class_name_() const { return class_name_; }
    void set_class_name_(const std::string& name) { class_name_ = name; }

    int get_class_size_() const { return class_size_; }
    void set_class_size_(int class_size) { class_size_ = class_size; }

    JackTokenizer tokenizer_;
    VMWriter writer_;
    SymbolTable symbols_;

    std::string class_name_;
    int class_size_;
    int if_count_;
    int while_count_;
};

#endif
