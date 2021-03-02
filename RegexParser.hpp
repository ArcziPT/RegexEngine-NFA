#ifndef REGEX_PARSER_H
#define REGEX_PARSER_H

#include <iostream>
#include <sstream>
#include <vector>
#include <array>

/**
 * Type == CHAR:
 *      -if input == a then go to out1
 * Type == RANGE:
 *      -if input between a and b go to out1
 * Type == ALT:
 *      -epsilon-transition to both out1 and out2
 * Type == ACCEPT:
 *      -accept string
 */ 
struct State{
    enum Type{
        CHAR,
        RANGE,
        ANY,
        ALT,
        ACCEPT
    } type;

    char a;
    char b;

    State* out1;
    State* out2;
};

struct Frag{
    State* state;
    std::vector<State*> outs;
};

struct Node{
    std::string val;
    std::vector<Node*> child;
    bool visited = false;

    Frag compiled;

    enum Type{
        MAIN,
        RE,
        UNION,
        SIMPLE,
        CONCAT,
        BASIC,
        BASIC_OP,
        ELEMENT,
        GROUP,
        ANY,
        CHAR,
        SET,
        POS_SET_TAIL,
        NEG_SET_TAIL,
        SET_ITEMS,
        SET_ITEMS_TAIL,
        SET_ITEM,
        SET_ITEM_TAIL,
        RANGE
    } type;

    Node(const std::string& val, Type type): val(val), type(type), child() {}

    friend std::ostream& operator<<(std::ostream& os, const Node& n);
};

class RegexParser{
public:
    RegexParser() = default;

    Node* parse(const std::string& regex);

private:
    char next(std::stringstream& ss);
    void match(std::stringstream& ss);
    void re(std::stringstream& ss, Node* n);
    void re_union(std::stringstream& ss, Node* n);
    void simple_re(std::stringstream& ss, Node* n);
    void concat(std::stringstream& ss, Node* n);
    void basic_re(std::stringstream& ss, Node* n);
    void basic_re_op(std::stringstream& ss, Node* n);
    void elementary_re(std::stringstream& ss, Node* n);
    void group(std::stringstream& ss, Node* n);
    void any(std::stringstream& ss, Node* n);
    void eos(std::stringstream& ss, Node* n);
    void ch(std::stringstream& ss, Node* n);
    void set(std::stringstream& ss, Node* n);
    void set_tail(std::stringstream& ss, Node* n);
    void set_items(std::stringstream& ss, Node* n);
    void set_items_tail(std::stringstream& ss, Node* n);
    void set_item(std::stringstream& ss, Node* n);
    void set_item_tail(std::stringstream& ss, Node* n);
    void range(std::stringstream& ss, Node* n);
};

#endif