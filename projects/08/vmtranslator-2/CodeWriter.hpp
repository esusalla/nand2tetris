#ifndef CODE_WRITER_HPP
#define CODE_WRITER_HPP

#include <fstream>
#include <stack>

#include "Parser.hpp"

class CodeWriter
{
public:
    CodeWriter(std::ofstream& outfile) : outfile_(outfile), jump_(0) {}

    void set_filename(const std::string& filename) { filename_ = filename; }

    void write_init();
    void write_arithmetic(const std::string& command);
    void write_push_pop(CommandType command_type, const std::string& segment, int idx);
    void write_label(const std::string& label);
    void write_goto(const std::string& label);
    void write_if(const std::string& label);
    void write_call(const std::string& name, int args);
    void write_return();
    void write_function(const std::string& name, int locals);

private:
    void push_();
    void push_from_segment_(int idx);

    void pop_();
    void pop_and_store_();
    void pop_to_segment_(int idx);

    void conditional_(const std::string& test);

    std::ofstream& outfile_;
    std::string filename_;
    int jump_;
    std::string function_;
};

#endif