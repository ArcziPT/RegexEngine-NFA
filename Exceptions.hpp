#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>

class ParsingError: public std::exception{
public:
    ParsingError(int pos, char c): pos(pos), c(c) {};

    const char* what() const noexcept{
        std::string s = "Parsing error at ";
        s += c;
        s += " (position: ";
        s += pos;
        s += ")";
    };

private:
    int pos;
    char c;
};

#endif