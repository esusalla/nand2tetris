#include "CodeWriter.hpp"

#include <fstream>
#include <string>

void CodeWriter::write_init()
{
    outfile_ << "@256" << '\n';
    outfile_ << "D=A" << '\n';
    outfile_ << "@SP" << '\n';
    outfile_ << "M=D" << '\n';

    write_call("Sys.init", 0);
}

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

void CodeWriter::write_label(const std::string& label) { outfile_ << '(' << function_ << '$' << label << ")\n"; }

void CodeWriter::write_goto(const std::string& label)
{
    outfile_ << '@' << function_ << '$' << label << '\n';
    outfile_ << "0;JMP" << '\n';
}

void CodeWriter::write_if(const std::string& label)
{
    pop_and_store_();
    outfile_ << '@' << function_ << '$' << label << '\n';
    outfile_ << "D;JNE" << '\n';
}

void CodeWriter::write_call(const std::string& name, int args)
{
    const auto jump = "JUMP" + std::to_string(jump_++);

    // Push return address and current frame state
    outfile_ << '@' << jump << '\n';
    outfile_ << "D=A" << '\n';
    push_();

    outfile_ << "@LCL" << '\n';
    outfile_ << "D=M" << '\n';
    push_();

    outfile_ << "@ARG" << '\n';
    outfile_ << "D=M" << '\n';
    push_();

    outfile_ << "@THIS" << '\n';
    outfile_ << "D=M" << '\n';
    push_();

    outfile_ << "@THAT" << '\n';
    outfile_ << "D=M" << '\n';
    push_();

    // Reposition ARG
    outfile_ << "@SP" << '\n';
    outfile_ << "D=M" << '\n';
    outfile_ << "@ARG" << '\n';
    outfile_ << "M=D" << '\n';
    outfile_ << '@' << args + 5 << '\n';
    outfile_ << "D=A" << '\n';
    outfile_ << "@ARG" << '\n';
    outfile_ << "M=M-D" << '\n';

    // Reposition LCL
    outfile_ << "@SP" << '\n';
    outfile_ << "D=M" << '\n';
    outfile_ << "@LCL" << '\n';
    outfile_ << "M=D" << '\n';

    outfile_ << '@' << name << '\n';
    outfile_ << "0;JMP" << '\n';

    outfile_ << '(' << jump << ")\n";
}

void CodeWriter::write_return()
{
    // Store LCL in temporary variable
    outfile_ << "@LCL" << '\n';
    outfile_ << "D=M" << '\n';
    outfile_ << "@R13" << '\n';
    outfile_ << "M=D" << '\n';

    // Store return address in temporary variable
    outfile_ << "@5" << '\n';
    outfile_ << "A=D-A" << '\n';
    outfile_ << "D=M" << '\n';
    outfile_ << "@R14" << '\n';
    outfile_ << "M=D" << '\n';

    // Reposition return value for caller
    outfile_ << "@SP" << '\n';
    outfile_ << "M=M-1" << '\n';
    outfile_ << "A=M" << '\n';
    outfile_ << "D=M" << '\n';
    outfile_ << "@ARG" << '\n';
    outfile_ << "A=M" << '\n';
    outfile_ << "M=D" << '\n';

    // Restore SP of caller
    outfile_ << "@ARG" << '\n';
    outfile_ << "D=M+1" << '\n';
    outfile_ << "@SP" << '\n';
    outfile_ << "M=D" << '\n';

    // Restore THAT of caller
    outfile_ << "@R13" << '\n';
    outfile_ << "D=M" << '\n';
    outfile_ << "@1" << '\n';
    outfile_ << "A=D-A" << '\n';
    outfile_ << "D=M" << '\n';
    outfile_ << "@THAT" << '\n';
    outfile_ << "M=D" << '\n';

    // Restore THIS of caller
    outfile_ << "@R13" << '\n';
    outfile_ << "D=M" << '\n';
    outfile_ << "@2" << '\n';
    outfile_ << "A=D-A" << '\n';
    outfile_ << "D=M" << '\n';
    outfile_ << "@THIS" << '\n';
    outfile_ << "M=D" << '\n';

    // Restore ARG of caller
    outfile_ << "@R13" << '\n';
    outfile_ << "D=M" << '\n';
    outfile_ << "@3" << '\n';
    outfile_ << "A=D-A" << '\n';
    outfile_ << "D=M" << '\n';
    outfile_ << "@ARG" << '\n';
    outfile_ << "M=D" << '\n';

    // Restore LCL of caller
    outfile_ << "@R13" << '\n';
    outfile_ << "D=M" << '\n';
    outfile_ << "@4" << '\n';
    outfile_ << "A=D-A" << '\n';
    outfile_ << "D=M" << '\n';
    outfile_ << "@LCL" << '\n';
    outfile_ << "M=D" << '\n';

    // Goto return address
    outfile_ << "@R14" << '\n';
    outfile_ << "A=M" << '\n';
    outfile_ << "0;JMP" << '\n';
}

void CodeWriter::write_function(const std::string& name, int locals)
{
    outfile_ << '(' << name << ")\n";
    outfile_ << "@0" << '\n';
    outfile_ << "D=A" << '\n';
    for (int i = 0; i < locals; ++i) {
        push_();
    }

    function_ = name;
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
    outfile_ << '(' << jump1 << ")\n";
    outfile_ << "@1" << '\n';
    outfile_ << "D=-A" << '\n';

    // Push
    outfile_ << '(' << jump2 << ")\n";
    outfile_ << "@SP" << '\n';
    outfile_ << "A=M" << '\n';
    outfile_ << "M=D" << '\n';
}