#ifndef REGEX_COMPILER_H
#define REGEX_COMPILER_H

#include <utility>
#include "RegexParser.hpp"
#include <memory>

class RegexCompiler{
public:
    RegexCompiler() = default;

    static Frag compile(Node* root);

private:
    static Frag handle_char(Node* root);
    static Frag handle_any(Node* root);
    static Frag handle_pos_set_tail(Node* root);
    static Frag handle_neg_set_tail(Node* root);
    static Frag handle_basic(Node* root);
    static Frag handle_re(Node* root);
    static Frag handle_simple(Node* root);
    static Frag handle_set_item(Node* root);
};

#endif