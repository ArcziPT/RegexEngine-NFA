#include "RegexParser.hpp"
#include "Exceptions.hpp"

#include <stack>
#include <cctype>

//Productions
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

//prod - production
//n - parent in AST 
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
        case Node::Type::BASIC_OP:
            os<<"BASIC_OP"<<n.val;
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
    RegexString regex_str = {
        .ss = std::stringstream(std::move(regex)),
        .pos = 0
    };
    std::stack<Context> prods;
    
    //init AST
    auto root = new Node("main", Node::Type::MAIN);
    prods.push({PROD::RE, root});

    while(!regex_str.ss.eof() && !prods.empty()){
        auto ctx = prods.top();
        prods.pop();
        auto c = next(regex_str); //lookahead

        //"execute" production rule
        switch(ctx.prod)
        {
            case RE:{
                //add new node to AST
                auto nn = new Node("", Node::Type::RE);
                ctx.n->child.push_back(nn);
                
                //push component rules
                prods.push({UNION, nn});
                prods.push({SIMPLE, nn});
                break;
            }
            
            case UNION:{
                if(c == '|'){
                    match(regex_str, '|');
                    prods.push({RE, ctx.n});
                }
                break;
            }

            case SIMPLE:{
                auto nn = new Node("", Node::Type::SIMPLE);
                ctx.n->child.push_back(nn);

                prods.push({CONCAT, nn});
                prods.push({BASIC, nn});
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
                
                prods.push({SIMPLE, ctx.n});
                break;
            }

            case BASIC:{
                auto nn = new Node("", Node::Type::BASIC);
                ctx.n->child.push_back(nn);
                
                prods.push({BASIC_OP, nn});
                prods.push({ELEMENTARY, nn});
                break;
            }
            
            case BASIC_OP:{
                if(c == '*'){
                    auto nn = new Node("*", Node::Type::BASIC_OP);
                    ctx.n->child.push_back(nn);
                    match(regex_str, '*');
                }else if(c == '+'){
                    auto nn = new Node("+", Node::Type::BASIC_OP);
                    ctx.n->child.push_back(nn);
                    match(regex_str, '+');
                }
                break;
            }

            case ELEMENTARY:{
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
                prods.push({RPAR, ctx.n});
                prods.push({RE, ctx.n});
                prods.push({LPAR, ctx.n});
                break;
            }

            case ANY:{
                auto nn = new Node("", Node::Type::ANY);
                ctx.n->child.push_back(nn);
                
                match(regex_str, '.');
                break;
            }

            case LPAR:{
                match(regex_str, '(');
                break;
            }

            case RPAR:{
                match(regex_str, ')');
                break;
            }

            case CHAR:{
                if(c == '\\'){
                    std::string s = "";
                    match(regex_str, '\\');

                    if(std::isalpha(next(regex_str)))
                        throw ParsingError(regex_str.pos, next(regex_str));

                    s += next(regex_str);
                    match(regex_str, next(regex_str));

                    auto nn = new Node(s, Node::Type::CHAR);
                    ctx.n->child.push_back(nn);
                }else{
                    std::string s = "";
                    s += c;
                    auto nn = new Node(s, Node::Type::CHAR);
                    ctx.n->child.push_back(nn);
                    match(regex_str, next(regex_str));
                }
                break;
            }

            case SET:{
                prods.push({RSQRBR, ctx.n});
                prods.push({SET_TAIL, ctx.n});
                prods.push({LSQRBR, ctx.n});
                break;
            }

            case RSQRBR:{
                match(regex_str, ']');
                break;
            }

            case LSQRBR:{
                match(regex_str, '[');
                break;
            }

            case SET_TAIL:{
                if(c == '^'){
                    match(regex_str, '^');
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
                    match(regex_str, '-');
                    prods.push({CHAR, ctx.n});
                }
            }
        }

        regex_str.pos++;
    }

    return root;
}

char RegexParser::next(RegexString& regex_str){
    regex_str.pos++;
    return regex_str.ss.peek();
}

void RegexParser::match(RegexString& regex_str, char c){
    char x;
    x = regex_str.ss.get();

    if(x != c)
        throw ParsingError(regex_str.pos, x);
}