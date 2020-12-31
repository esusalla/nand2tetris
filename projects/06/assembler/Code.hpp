#ifndef CODE_HPP
#define CODE_HPP

#include <string>
#include <unordered_map>

class Code
{
public:
    std::string dest(const std::string& mnemonic) const { return dest_.at(mnemonic); }
    std::string comp(const std::string& mnemonic) const { return comp_.at(mnemonic); }
    std::string jump(const std::string& mnemonic) const { return jump_.at(mnemonic); }

private:
    static const std::unordered_map<std::string, std::string> dest_;
    static const std::unordered_map<std::string, std::string> comp_;
    static const std::unordered_map<std::string, std::string> jump_;
};

#endif
