#include "VMWriter.hpp"

#include <fstream>
#include <string>
#include <unordered_map>

const std::unordered_map<Segment, std::string> VMWriter::segment_to_string_ = {
    {Segment::CONST, "constant"},  {Segment::ARG, "argument"}, {Segment::LOCAL, "local"},
    {Segment::STATIC, "static"},   {Segment::THIS, "this"},    {Segment::THAT, "that"},
    {Segment::POINTER, "pointer"}, {Segment::TEMP, "temp"}};
const std::unordered_map<Command, std::string> VMWriter::command_to_string_ = {
    {Command::ADD, "add"}, {Command::SUB, "sub"}, {Command::NEG, "neg"}, {Command::EQ, "eq"},  {Command::GT, "gt"},
    {Command::LT, "lt"},   {Command::AND, "and"}, {Command::OR, "or"},   {Command::NOT, "not"}};

void VMWriter::write_push(Segment seg, int idx)
{
    outfile_ << "push " << segment_to_string_.at(seg) << ' ' << idx << std::endl;
}

void VMWriter::write_pop(Segment seg, int idx)
{
    outfile_ << "pop " << segment_to_string_.at(seg) << ' ' << idx << std::endl;
}

void VMWriter::write_arithmetic(Command cmd) { outfile_ << command_to_string_.at(cmd) << std::endl; }

void VMWriter::write_label(const std::string& label) { outfile_ << "label " << label << std::endl; }

void VMWriter::write_goto(const std::string& label) { outfile_ << "goto " << label << std::endl; }

void VMWriter::write_if(const std::string& label) { outfile_ << "if-goto " << label << std::endl; }

void VMWriter::write_call(const std::string& label, int n_args)
{
    outfile_ << "call " << label << ' ' << n_args << std::endl;
}

void VMWriter::write_function(const std::string& label, int n_locals)
{
    outfile_ << "function " << label << ' ' << n_locals << std::endl;
}

void VMWriter::write_return() { outfile_ << "return" << std::endl; }