#include "CodeWriter.hpp"

#include <fstream>
#include <string>

void CodeWriter::write_arithmetic(const std::string& command)
{
    if (command == "add") {
        pop_and_store_();
        pop_();
        outfile_ << "M=D+M" << '\n';
    } else if (command == "and") {
        pop_and_store_();
        pop_();
        outfile_ << "M=D&M" << '\n';
    } else if (command == "eq") {
        pop_and_store_();
        pop_();
        conditional_("EQ");
    } else if (command == "gt") {
        pop_and_store_();
        pop_();
        conditional_("GT");
    } else if (command == "lt") {
        pop_and_store_();
        pop_();
        conditional_("LT");
    } else if (command == "neg") {
        pop_();
        outfile_ << "M=-M" << '\n';
    } else if (command == "not") {
        pop_();
        outfile_ << "M=!M" << '\n';
    } else if (command == "or") {
        pop_and_store_();
        pop_();
        outfile_ << "M=D|M" << '\n';
    } else if (command == "sub") {
        pop_and_store_();
        pop_();
        outfile_ << "M=M-D" << '\n';
    }

    // Increment stack pointer
    outfile_ << "@SP" << '\n';
    outfile_ << "M=M+1" << '\n';
}

void CodeWriter::write_push_pop(CommandType command_type, const std::string& segment, int idx)
{
    if (command_type == CommandType::C_PUSH) {
        if (segment == "constant") {
            outfile_ << '@' << idx << '\n';
            outfile_ << "D=A" << '\n';
            push_();
        } else if (segment == "local") {
            outfile_ << "@LCL" << '\n';
            outfile_ << "D=M" << '\n';
            push_from_segment_(idx);
        } else if (segment == "argument") {
            outfile_ << "@ARG" << '\n';
            outfile_ << "D=M" << '\n';
            push_from_segment_(idx);
        } else if (segment == "this") {
            outfile_ << "@THIS" << '\n';
            outfile_ << "D=M" << '\n';
            push_from_segment_(idx);
        } else if (segment == "that") {
            outfile_ << "@THAT" << '\n';
            outfile_ << "D=M" << '\n';
            push_from_segment_(idx);
        } else if (segment == "pointer") {
            outfile_ << "@R3" << '\n';
            outfile_ << "D=A" << '\n';
            push_from_segment_(idx);
        } else if (segment == "temp") {
            outfile_ << "@R5" << '\n';
            outfile_ << "D=A" << '\n';
            push_from_segment_(idx);
        } else if (segment == "static") {
            outfile_ << '@' << filename_ << '.' << idx << '\n';
            outfile_ << "D=M" << '\n';
            push_();
        }
    } else {
        if (segment == "local") {
            outfile_ << "@LCL" << '\n';
            outfile_ << "D=M" << '\n';
            pop_to_segment_(idx);
        } else if (segment == "argument") {
            outfile_ << "@ARG" << '\n';
            outfile_ << "D=M" << '\n';
            pop_to_segment_(idx);
        } else if (segment == "this") {
            outfile_ << "@THIS" << '\n';
            outfile_ << "D=M" << '\n';
            pop_to_segment_(idx);
        } else if (segment == "that") {
            outfile_ << "@THAT" << '\n';
            outfile_ << "D=M" << '\n';
            pop_to_segment_(idx);
        } else if (segment == "pointer") {
            outfile_ << "@R3" << '\n';
            outfile_ << "D=A" << '\n';
            pop_to_segment_(idx);
        } else if (segment == "temp") {
            outfile_ << "@R5" << '\n';
            outfile_ << "D=A" << '\n';
            pop_to_segment_(idx);
        } else if (segment == "static") {
            pop_and_store_();
            outfile_ << '@' << filename_ << '.' << idx << '\n';
            outfile_ << "M=D" << '\n';
        }
    }
}

void CodeWriter::close()
{
    // Terminate with infinite loop
    outfile_ << "(JUMP" << jump_ << ")\n";
    outfile_ << "@JUMP" << jump_ << '\n';
    outfile_ << "0;JMP" << '\n';
}

void CodeWriter::push_()
{
    outfile_ << "@SP" << '\n';
    outfile_ << "A=M" << '\n';
    outfile_ << "M=D" << '\n';
    outfile_ << "@SP" << '\n';
    outfile_ << "M=M+1" << '\n';
}

void CodeWriter::push_from_segment_(int idx)
{
    outfile_ << '@' << idx << '\n';
    outfile_ << "A=D+A" << '\n';
    outfile_ << "D=M" << '\n';
    push_();
}

void CodeWriter::pop_()
{
    outfile_ << "@SP" << '\n';
    outfile_ << "M=M-1" << '\n';
    outfile_ << "A=M" << '\n';
}

void CodeWriter::pop_and_store_()
{
    pop_();
    outfile_ << "D=M" << '\n';
}

void CodeWriter::pop_to_segment_(int idx)
{
    outfile_ << '@' << idx << '\n';
    outfile_ << "D=D+A" << '\n';
    outfile_ << "@R13" << '\n';
    outfile_ << "M=D" << '\n';

    pop_and_store_();

    outfile_ << "@R13" << '\n';
    outfile_ << "A=M" << '\n';
    outfile_ << "M=D" << '\n';
}

void CodeWriter::conditional_(const std::string& test)
{
    const auto jump1 = "JUMP" + std::to_string(jump_++);
    const auto jump2 = "JUMP" + std::to_string(jump_++);

    outfile_ << "D=M-D" << '\n';

    // Jump to True section if test is true
    outfile_ << '@' << jump1 << '\n';
    outfile_ << "D;J" << test << '\n';

    // False section, jump past True section to push
    outfile_ << "@0" << '\n';
    outfile_ << "D=A" << '\n';
    outfile_ << '@' << jump2 << '\n';
    outfile_ << "0;JMP" << '\n';

    // True section
    outfile_ << "(" << jump1 << ")\n";
    outfile_ << "@1" << '\n';
    outfile_ << "D=-A" << '\n';

    // Push
    outfile_ << "(" << jump2 << ")\n";
    outfile_ << "@SP" << '\n';
    outfile_ << "A=M" << '\n';
    outfile_ << "M=D" << '\n';
}
