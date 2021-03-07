#ifndef REGEX_PARSER_H
#define REGEX_PARSER_H

#include <iostream>
#include <sstream>
#include <vector>
#include <array>
#include <bitset>

struct State;

struct Transition{
    std::bitset<256> is_accepted;
    State* next;
};

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
        ACCEPT,
        NOP,
        STEP
    } type;

    std::vector<Transition> transitions;
    std::vector<State*> e_transition;
};

struct Frag{
    State* begin;
    State* end;
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

    static Node* parse(const std::string& regex);

private:
    static char next(std::stringstream& ss);
    static void match(std::stringstream& ss);
    static void re(std::stringstream& ss, Node* n);
    static void re_union(std::stringstream& ss, Node* n);
    static void simple_re(std::stringstream& ss, Node* n);
    static void concat(std::stringstream& ss, Node* n);
    static void basic_re(std::stringstream& ss, Node* n);
    static void basic_re_op(std::stringstream& ss, Node* n);
    static void elementary_re(std::stringstream& ss, Node* n);
    static void group(std::stringstream& ss, Node* n);
    static void any(std::stringstream& ss, Node* n);
    static void eos(std::stringstream& ss, Node* n);
    static void ch(std::stringstream& ss, Node* n);
    static void set(std::stringstream& ss, Node* n);
    static void set_tail(std::stringstream& ss, Node* n);
    static void set_items(std::stringstream& ss, Node* n);
    static void set_items_tail(std::stringstream& ss, Node* n);
    static void set_item(std::stringstream& ss, Node* n);
    static void set_item_tail(std::stringstream& ss, Node* n);
    static void range(std::stringstream& ss, Node* n);
};

#endif