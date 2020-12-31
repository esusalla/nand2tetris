#ifndef VM_WRITER_HPP
#define VM_WRITER_HPP

#include <fstream>
#include <string>
#include <unordered_map>

enum class Segment { CONST, ARG, LOCAL, STATIC, THIS, THAT, POINTER, TEMP };
enum class Command { ADD, SUB, NEG, EQ, GT, LT, AND, OR, NOT };

class VMWriter
{
public:
    VMWriter(std::ofstream& outfile) : outfile_(outfile) {}

    void write_push(Segment seg, int idx);
    void write_pop(Segment seg, int idx);
    void write_arithmetic(Command cmd);
    void write_label(const std::string& label);
    void write_goto(const std::string& label);
    void write_if(const std::string& label);
    void write_call(const std::string& name, int n_args);
    void write_function(const std::string& name, int n_locals);
    void write_return();

    void close() { outfile_.close(); }

private:
    static const std::unordered_map<Segment, std::string> segment_to_string_;
    static const std::unordered_map<Command, std::string> command_to_string_;

    std::ofstream& outfile_;
};

#endif