#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>

class ParsingError: public std::exception{
public:
    ParsingError(int pos, char c): pos(pos), c(c) {
        std::string s = "Parsing error at ";
        s += c;
        s += " (position: ";
        s += std::to_string(pos);
        s += ")";
    };

    const char* what() const noexcept{
        return s.c_str();
    };

private:
    int pos;
    char c;
    std::string s;
};

#endif