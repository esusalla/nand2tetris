#include <dirent.h>

#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "CodeWriter.hpp"
#include "Parser.hpp"

bool get_filepaths(const std::string& input, std::vector<std::string>& filepaths, std::string& outpath);
bool parse_and_write(std::vector<std::string>& filepaths, const std::string& outpath);

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "Missing input" << std::endl;
        return 1;
    }

    std::string input(argv[1]);
    std::vector<std::string> filepaths;
    std::string outpath;

    if (!get_filepaths(input, filepaths, outpath)) return 1;

    if (!parse_and_write(filepaths, outpath)) return 1;

    return 0;
}
bool ends_with(const std::string& str, const std::string& end)
{
    return str.size() >= end.size() && str.compare(str.size() - end.size(), end.size(), end) == 0;
}

bool get_filepaths(const std::string& input, std::vector<std::string>& filepaths, std::string& outpath)
{
    if (!ends_with(input, ".vm")) {
        // Handle directory
        DIR* dr = opendir(input.c_str());
        if (!dr) {
            std::cerr << "Cannot open directory: " << input << std::endl;
            return false;
        }

        std::string basepath = input.back() == '/' ? input : input + '/';
        struct dirent* de = nullptr;
        while ((de = readdir(dr))) {
            if (ends_with(de->d_name, ".vm")) filepaths.push_back(basepath + std::string(de->d_name));
        }

        int idx = 0;
        while (basepath.find('/', idx + 1) != basepath.length() - 1) {
            idx = basepath.find('/', idx + 1);
        }

        std::string dir_name(basepath.begin() + idx + 1, basepath.end() - 1);
        outpath = basepath + dir_name + ".asm";
    } else {
        // Handle single file
        filepaths.push_back(input);
        outpath = input.substr(0, input.find_last_of(".vm") - 2) + ".asm";
    }
    std::cout << outpath << std::endl;
    return true;
}

bool parse_and_write(std::vector<std::string>& filepaths, const std::string& outpath)
{
    std::ofstream outfile(outpath);
    if (!outfile) {
        std::cerr << "Cannot open outpath file: " << outpath << std::endl;
        return false;
    }

    CodeWriter code_writer(outfile);

    for (const auto& filepath : filepaths) {
        const std::string filename(filepath.begin() + filepath.find_last_of('/') + 1,
                                   filepath.begin() + filepath.find_last_of('.'));
        code_writer.set_filename(filename);

        std::ifstream infile(filepath);
        if (!infile) {
            std::cerr << "Cannot open input file: " << filepath << std::endl;
            return false;
        }

        Parser parser(infile);
        while (parser.has_more_commands()) {
            parser.advance();

            auto command_type = parser.command_type();
            if (command_type == CommandType::C_ARITHMETIC) {
                code_writer.write_arithmetic(parser.arg1());
            } else if (command_type == CommandType::C_PUSH || command_type == CommandType::C_POP)
                code_writer.write_push_pop(command_type, parser.arg1(), parser.arg2());
        }
    }

    code_writer.close();
    return true;
}
