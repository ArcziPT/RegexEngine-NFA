#ifndef REGEX_H
#define REGEX_H

#include "RegexCompiler.hpp"

#include <iterator>
#include <string_view>

struct Match{
    int pos;
    int len;

    friend std::ostream& operator<<(std::ostream& os, const Match& m){
        os<<"Match{pos="<<m.pos<<", len="<<m.len<<"}";
        return os;
    }
};

class Regex{
public:
    Regex(const std::string& regex);

    bool check(std::string_view sv);
    Match one_match(std::string_view sv);
    std::vector<Match> all_matches(std::string_view sv);

private:
    Frag nfa;

    std::vector<State*> walk_e(State* s);
    std::vector<State*> handle_state(State* s, char c);

    Match run_once(std::string_view sv);
};

#endif