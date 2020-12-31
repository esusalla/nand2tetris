#include <dirent.h>

#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "CompilationEngine.hpp"

bool get_filepaths(const std::string& input, std::vector<std::string>& filepaths);

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "Missing input" << std::endl;
        return 1;
    }

    std::string input(argv[1]);
    std::vector<std::string> filepaths;
    std::string outpath;

    if (!get_filepaths(input, filepaths)) return 1;

    for (const auto& filepath : filepaths) {
        std::ifstream infile(filepath);

        std::string filename(filepath.begin() + filepath.find_last_of('/'),
                             filepath.begin() + filepath.find_last_of('.'));
        outpath = filepath.substr(0, filepath.find_last_of('/')) + filename + ".vm";
        std::ofstream outfile(outpath);

        CompilationEngine engine(infile, outfile);
        engine.compile_class();

        outfile.close();
    }

    return 0;
}

bool ends_with(const std::string& str, const std::string& end)
{
    return str.size() >= end.size() && str.compare(str.size() - end.size(), end.size(), end) == 0;
}

bool get_filepaths(const std::string& input, std::vector<std::string>& filepaths)
{
    if (!ends_with(input, ".jack")) {
        // Handle directory
        DIR* dr = opendir(input.c_str());
        if (!dr) {
            std::cerr << "Cannot open directory: " << input << std::endl;
            return false;
        }

        std::string basepath = input.back() == '/' ? input : input + '/';
        struct dirent* de = nullptr;
        while ((de = readdir(dr))) {
            if (ends_with(de->d_name, ".jack")) filepaths.push_back(basepath + std::string(de->d_name));
        }
    } else {
        // Handle single file
        filepaths.push_back(input);
    }

    return true;
}
