#ifndef REGEX_H
#define REGEX_H

#include "RegexCompiler.hpp"

class Regex{
public:
    Regex(const std::string& regex);

    bool match(const std::string& s);

private:
    Frag nfa;

    std::vector<State*> walk_e(State* s);
    std::vector<State*> handle_state(State* s, char c);
};

#endif