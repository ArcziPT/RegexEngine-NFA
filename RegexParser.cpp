#include "RegexParser.hpp"

#include <stack>

enum PROD{
    RE,
    UNION,
    SIMPLE,
    CONCAT,
    BASIC,
    BASIC_OP,
    ELEMENTARY,
    GROUP,
    ANY,
    CHAR,
    SET,
    SET_TAIL,
    SET_ITEMS,
    SET_ITEMS_TAIL,
    SET_ITEM,
    SET_ITEM_TAIL,
    RANGE,
    LPAR,
    RPAR,
    LSQRBR,
    RSQRBR
};

struct Context{
    PROD prod;
    Node* n;
};

std::ostream& operator<<(std::ostream& os, const Node& n){
    switch (n.type){
        case Node::Type::CHAR:
            os<<"CHAR -> "<<n.val;
            break;
        case Node::Type::ANY:
            os<<"ANY";
            break;
        case Node::Type::BASIC:
            os<<"BASIC";
            break;
        case Node::Type::CONCAT:
            os<<"CONCAT";
            break;
        case Node::Type::ELEMENT:
            os<<"ELEMENT";
            break;
        case Node::Type::GROUP:
            os<<"GROUP";
            break;
        case Node::Type::NEG_SET_TAIL:
            os<<"NEG_SET_TAIL";
            break;
        case Node::Type::POS_SET_TAIL:
            os<<"POS_SET_TAIL";
            break;
        case Node::Type::RANGE:
            os<<"RANGE";
            break;
        case Node::Type::RE:
            os<<"RE";
            break;
        case Node::Type::SET:
            os<<"SET";
            break;
        case Node::Type::SET_ITEM:
            os<<"SET_ITEM";
            break;
        case Node::Type::SET_ITEM_TAIL:
            os<<"SET_ITEM_TAIL";
            break;
        case Node::Type::SET_ITEMS:
            os<<"SET_ITEMS";
            break;
        case Node::Type::SET_ITEMS_TAIL:
            os<<"SET_ITEMS_TAIL";
            break;
        case Node::Type::SIMPLE:
            os<<"SIMPLE";
            break;
        case Node::Type::UNION:
            os<<"UNION";
            break;
    }

    return os;
}

Node* RegexParser::parse(const std::string& regex){
    std::stringstream ss(std::move(regex));
    std::stack<Context> prods;
    
    auto root = new Node("main", Node::Type::MAIN);
    prods.push({PROD::RE, root});

    while(!ss.eof() && !prods.empty()){
        auto ctx = prods.top();
        prods.pop();
        auto c = next(ss);

        switch(ctx.prod)
        {
            case RE:{
                auto nn = new Node("", Node::Type::RE);
                ctx.n->child.push_back(nn);
                
                prods.push({UNION, nn});
                prods.push({SIMPLE, nn});
                break;
            }
            
            case UNION:{
                if(c == '|'){
                    match(ss);
                    
                    auto nn = new Node("", Node::Type::UNION);
                    ctx.n->child.push_back(nn);

                    prods.push({RE, nn});
                }
                break;
            }

            case SIMPLE:{
                //auto nn = new Node("", Node::Type::SIMPLE);
                //ctx.n->child.push_back(nn);

                prods.push({CONCAT, ctx.n});
                prods.push({BASIC, ctx.n});
                break;
            }

            case CONCAT: {
                auto set = {')', ']', '*', '+', '|'};
                bool is_in = false;
                for(auto& s : set){
                    if(c == s)
                        is_in = true;
                }

                if(is_in)
                    break;

                auto nn = new Node("", Node::Type::CONCAT);
                ctx.n->child.push_back(nn);
                
                prods.push({SIMPLE, nn});
                break;
            }

            case BASIC:{
                //auto nn = new Node("", Node::Type::BASIC);
                //ctx.n->child.push_back(nn);
                
                prods.push({BASIC_OP, ctx.n});
                prods.push({ELEMENTARY, ctx.n});
                break;
            }
            
            case BASIC_OP:{
                if(c == '*'){
                    auto nn = new Node("*", Node::Type::BASIC_OP);
                    ctx.n->child.push_back(nn);
                    match(ss);
                }else if(c == '+'){
                    auto nn = new Node("+", Node::Type::BASIC_OP);
                    ctx.n->child.push_back(nn);
                    match(ss);
                }
                break;
            }

            case ELEMENTARY:{
                //auto nn = new Node("", Node::Type::ELEMENT);
                //ctx.n->child.push_back(nn);
                
                switch(c)
                {
                case '(':
                    prods.push({GROUP, ctx.n});
                    break;
                case '.':
                    prods.push({ANY, ctx.n});
                    break;
                case '[':
                    prods.push({SET, ctx.n});
                    break;
                default:
                    prods.push({CHAR, ctx.n});
                }
                break;
            }

            case GROUP:{
                //auto nn = new Node("", Node::Type::GROUP);
                //ctx.n->child.push_back(nn);
                
                prods.push({RPAR, ctx.n});
                prods.push({RE, ctx.n});
                prods.push({LPAR, ctx.n});
                break;
            }

            case ANY:{
                auto nn = new Node("", Node::Type::ANY);
                ctx.n->child.push_back(nn);
                
                match(ss);
                break;
            }

            case LPAR:{
                match(ss);
                break;
            }

            case RPAR:{
                match(ss);
                break;
            }

            case CHAR:{
                if(c == '\\'){
                    std::string s = "";
                    match(ss);
                    s += next(ss);
                    match(ss);

                    auto nn = new Node(s, Node::Type::CHAR);
                    ctx.n->child.push_back(nn);
                }else{
                    std::string s = "";
                    s += c;
                    auto nn = new Node(s, Node::Type::CHAR);
                    ctx.n->child.push_back(nn);
                    match(ss);
                }
                break;
            }

            case SET:{
                //auto nn = new Node("", Node::Type::SET);
                //ctx.n->child.push_back(nn);

                prods.push({RSQRBR, ctx.n});
                prods.push({SET_TAIL, ctx.n});
                prods.push({LSQRBR, ctx.n});
                break;
            }

            case RSQRBR:{
                match(ss);
                break;
            }

            case LSQRBR:{
                match(ss);
                break;
            }

            case SET_TAIL:{
                if(c == '^'){
                    match(ss);
                    auto nn = new Node("", Node::Type::NEG_SET_TAIL);
                    ctx.n->child.push_back(nn);
                    
                    prods.push({SET_ITEMS, nn});
                }else{
                    auto nn = new Node("", Node::Type::POS_SET_TAIL);
                    ctx.n->child.push_back(nn);
                    
                    prods.push({SET_ITEMS, nn});
                }
                break;
            }
                
            case SET_ITEMS:{
                //auto nn = new Node("set-items", Node::Type::SET_ITEMS);
                //ctx.n->child.push_back(nn);
                
                prods.push({SET_ITEMS_TAIL, ctx.n});
                prods.push({SET_ITEM, ctx.n});
                break;
            }

            case SET_ITEMS_TAIL:{
                auto set = {'(', ')', '[', ']', '*', '+', '|', '-', '^', '$', '.'};
                bool is_in = false;
                for(auto& s : set){
                    if(s == c)
                        is_in = true;
                }

                if(is_in)
                    break;

                //auto nn = new Node("", Node::Type::SET_ITEMS_TAIL);
                //ctx.n->child.push_back(nn);

                prods.push({SET_ITEMS, ctx.n});
                break;
            }

            case SET_ITEM:{
                auto nn = new Node("", Node::Type::SET_ITEM);
                ctx.n->child.push_back(nn);
                
                prods.push({RANGE, nn});
                prods.push({CHAR, nn});
                break;
            }

            case RANGE:{
                if(c == '-'){
                    auto nn = new Node("", Node::Type::RANGE);
                    ctx.n->child.push_back(nn);
                    
                    match(ss);
                    prods.push({CHAR, nn});
                }
            }
        }
    }

    return root;
}

char RegexParser::next(std::stringstream& ss){
    return ss.peek();
}

void RegexParser::match(std::stringstream& ss){
    char x;
    x = ss.get();
}