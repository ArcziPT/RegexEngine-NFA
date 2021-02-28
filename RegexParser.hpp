#ifndef REGEX_PARSER_H
#define REGEX_PARSER_H

#include <iostream>
#include <sstream>
#include <vector>

struct Node{
    std::string val;
    std::vector<Node*> child;
    Node* parent;

    Node(const std::string& val, Node* parent): val(val), parent(parent), child() {}
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